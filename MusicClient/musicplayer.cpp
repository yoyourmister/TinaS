#include "musicplayer.h"
#include "ui_musicplayer.h"

bool MusicPlayer::loadConfigFile() {
    QString configFile=QDir::currentPath()+"config.ini";
    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "file not open";
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_1);

    in >> defaultdir;

    file.close();
    return true;
}

bool MusicPlayer::saveConfigFile() {
    QString configFile=QDir::currentPath()+"config.ini";
    QFile file(configFile);
    if (!file.open(QIODevice::WriteOnly)) {
        qDebug() << "file not open";
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_1);

    out << defaultdir;

    file.close();
    return true;
}

MusicPlayer::MusicPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);

    socket=new QTcpSocket(this);

    defaultdir="/home";
    loadConfigFile();
    qDebug()<<defaultdir;

    QList<QAudioDeviceInfo> infoList=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo &deviceInfo, infoList) {
        ui->box_devices->addItem(deviceInfo.deviceName());
        qDebug() << "Device name: " << deviceInfo.deviceName() << deviceInfo.supportedCodecs();
    }
    curDeviceName=ui->box_devices->itemText(0);

    connect( ui->but_Mute, SIGNAL(clicked()), this, SLOT(on_but_Mute_toggled()) );
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
    /*if (playlist)
    {
        delete playlist;
    }
    if (vf) {
        delete vf;
    }*/
    delete socket;
}

void MusicPlayer::on_slider_Volume_valueChanged(int value)
{
    //if (mediaPlayer.state()==QMediaPlayer::PlayingState)
    //{
        mediaPlayer.setVolume(value);
    //}
    qDebug()<<mediaPlayer.volume();
}

void MusicPlayer::on_but_Mute_toggled()
{
    if (ui->but_Mute->isChecked())
    {
        mediaPlayer.setMuted(true);
    }
    else
    {
        mediaPlayer.setMuted(false);
    }
    qDebug() << "Mute" << mediaPlayer.isMuted();
}

void MusicPlayer::clientConnect(QString hostname)
{
    connect(socket,SIGNAL(connected()), this, SLOT(connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));

    qDebug()<<"Connecting...";

    //socket->connectToHost("google.com",80);
    socket->connectToHost(hostname,7777);

    qDebug()<<"waiting...";

    if(!socket->waitForConnected(1000))
    {
        qDebug()<<"Error:"<< socket->errorString();
    }
    qDebug()<<"established";
}


void MusicPlayer::addMusicFile(QString dir)
{
    QFileInfoList musicDir=QDir(dir).entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=QDir(dir).entryInfoList(QDir::Files);
    //QFileInfoList musicDir=QDir("C:/Users/test/Desktop/Musik").entryInfoList(QDir::Dirs);
    //QFileInfoList musicFiles=QDir("C:/Users/test/Desktop/Musik").entryInfoList(QDir::Files);

    //QFileInfoList musicDir=QDir("G:/Manga/Spiele/(000)NewDownloads/Musik").entryInfoList(QDir::Dirs);
    //QFileInfoList musicFiles=QDir("G:/Manga/Spiele/(000)NewDownloads/Musik").entryInfoList(QDir::Files);
    for (int i=2; i<musicDir.size(); i++)
    {
        //qDebug() << musicDir.at(i).absoluteFilePath();
        musicFiles.append(QDir(musicDir.at(i).absoluteFilePath()).entryInfoList(QDir::Files));
    }
    QList<QMediaContent> musicList;
    QMediaContent musicfile;
    for (int i=0; i<musicFiles.size(); i++)
    {
        //qDebug()<<musicFiles.at(i).absoluteFilePath();
        musicfile=QMediaContent(QUrl::fromLocalFile(musicFiles.at(i).absoluteFilePath()));
        musicList.append(musicfile);
    }
    playlist=new QMediaPlaylist();
    playlist->addMedia(musicList);
    for (int i=0; i<playlist->mediaCount(); i++)
    {
        QString filename=playlist->media(i).canonicalUrl().toString();
        filename=filename.mid(filename.lastIndexOf("/")+1);
        ui->list_Tracks->addItem(filename);
        qDebug() << i << " " << filename ;
    }
    mediaPlayer.setPlaylist(playlist);
    //QString filename=QFileDialog::getOpenFileName(this,"Open Music File");
    //mediaPlayer.play();

    ui->label_error->setText(mediaPlayer.errorString());
}

void MusicPlayer::connected()
{
    qDebug()<<"Connected";

    //socket->write("HEAD / HTTP/1.0\r\n\r\n\r\n\r\n");
}

void MusicPlayer::disconnected()
{
    qDebug()<<"Disconnected";
}

void MusicPlayer::bytesWritten(qint64 bytes)
{
    qDebug()<<"we wrote: " << bytes;
}


void MusicPlayer::readyRead()
{
    qDebug()<<"Reading...";
    QByteArray readbytes=socket->readAll();
    QString readString(readbytes);
    qDebug()<<readbytes;
    ui->label_debug->setText(readbytes);
    if (readString.left(4)=="play")
    {
        QString title=readString.mid(4);
        for (int i=0; i<mediaPlayer.playlist()->mediaCount(); i++)
        {
            QString filename=mediaPlayer.playlist()->media(i).canonicalUrl().toString();
            filename=filename.mid(filename.lastIndexOf("/")+1);
            if (filename.compare(title)==0) {
                qDebug() << "play" << filename ;
                mediaPlayer.playlist()->setCurrentIndex(i);
            }
        }
        mediaPlayer.play();
    }
    if (readString.left(4)=="list") {
        QString list=readString.mid(4);
        QStringList titles=list.split(":");
        currentPlaylist=new QMediaPlaylist();
        for (int i=0; i<playlist->mediaCount(); i++)
        {
            QString filename=playlist->media(i).canonicalUrl().toString();
            filename=filename.mid(filename.lastIndexOf("/")+1);
            qDebug()<<filename;
            for (int j=0; j<titles.size(); ++j) {
                qDebug()<<titles.at(j);
                if (filename==titles.at(j)) {
                    currentPlaylist->addMedia(playlist->media(i));
                }
            }
        }
        mediaPlayer.setPlaylist(currentPlaylist);
        mediaPlayer.play();
    }
    if (readString.left(4)=="sync") {
        QStringList properties=readString.mid(4).split(":");
        mediaPlayer.playlist()->setCurrentIndex(properties.at(0).toInt());
        mediaPlayer.setPosition(properties.at(1).toLongLong());
    }

    ui->label_error->setText(mediaPlayer.errorString());
}

void MusicPlayer::on_but_connect_clicked()
{
    IPaddress=ui->editIP->text();
    clientConnect(IPaddress);
}

void MusicPlayer::on_pushButton_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),defaultdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    qDebug()<<dir;
    if (dir!="") {
        addMusicFile(dir);
        defaultdir=dir;
        saveConfigFile();
    }
}

void MusicPlayer::on_pushButton_2_clicked()
{
    mediaPlayer.playlist()->next();
    qDebug()<<(mediaPlayer.playlist()->currentIndex());
    //playlist->mediaInserted();
}

void MusicPlayer::on_but_play_clicked()
{
    qDebug()<<mediaPlayer.isAudioAvailable();
    if (mediaPlayer.state()==QMediaPlayer::PlayingState) {
        mediaPlayer.stop();
        ui->but_play->setText("Play");
    } else if (mediaPlayer.state()==QMediaPlayer::StoppedState && mediaPlayer.isAudioAvailable()) {
        mediaPlayer.play();
        ui->but_play->setText("Stop");
    }
}

void MusicPlayer::on_but_resync_clicked()
{
    socket->write("resync");
}



//QAudioDecoder *decoder;
QAudioOutput *audioOutput;
QAudioBuffer m_arr[1000];

int arrCnt=0;
int m_arrPos=0;
QAudioFormat format;

void MusicPlayer::decodeFinished() {
    qDebug()<<"Finished";
    for (int i=0; i<arrCnt; ++i) {
        //outputFile->write(static_cast<const char*>(m_arr[i].data()),m_arr[i].byteCount());
        //vf->writeData(static_cast<const char*>(m_arr[i].data()),m_arr[i].byteCount());
    }
    qDebug()<<"Finished writing";
    //audioOutput->start(vf);
}

void MusicPlayer::decodeDone() {
    qDebug()<<"done"<<arrCnt;
    QAudioFormat qaf=m_decoder->audioFormat();
    qDebug()<<qaf.channelCount() << qaf.codec() << qaf.sampleRate() << qaf.sampleSize() << qaf.byteOrder() << qaf.sampleType();
    m_arr[arrCnt]=m_decoder->read();
    vf->writeData(static_cast<const char*>(m_arr[arrCnt].data()),m_arr[arrCnt].byteCount());
    if (arrCnt==0) {
        audioOutput->start(vf);
    }
    arrCnt++;
}

void MusicPlayer::decode()
{
    qDebug()<<"start decode";
    m_decoder=new QAudioDecoder();

    //decoder->setAudioFormat(format);
    connect(m_decoder,SIGNAL(bufferReady()),SLOT(decodeDone()));
    connect(m_decoder,SIGNAL(finished()),SLOT(decodeFinished()));
    //decoder->setSourceFilename("J:/Musik/Snap - Oops Up.mp3");
    QString musicfile=QDir::currentPath()+"/Erdenstern - Snow Queen.mp3";//"/Snap - Oops Up.mp3";//
    qDebug()<<musicfile;
    m_decoder->setSourceFilename(musicfile);
    m_decoder->start();
    qDebug()<<"decode started";
}

void MusicPlayer::finishedPlaying(QAudio::State state)
{
    if (state == QAudio::IdleState) {
        audioOutput->stop();
        vf->close();
        delete audioOutput;
    }
}

void MusicPlayer::startPlaying()
{
    vf=new VirtualFile(this);

    format.setSampleRate(44100);
    format.setChannelCount(2);
    format.setSampleSize(16);
    format.setCodec("audio/pcm");
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setSampleType(QAudioFormat::SignedInt);

    QAudioDeviceInfo info(QAudioDeviceInfo::defaultOutputDevice());
    if (!info.isFormatSupported(format)) {
        qWarning()<<"raw audio format not supported by backend, cannot play audio.";
        format = info.nearestFormat(format);
        //return;
    }

    qDebug()<<"start Output";
    QList<QAudioDeviceInfo> infoList=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo &deviceInfo, infoList) {
        if (deviceInfo.deviceName()==curDeviceName) {
            qDebug()<<deviceInfo.deviceName();
            audioOutput = new QAudioOutput(deviceInfo,format,this);
        }
        //qDebug() << "Device name: " << deviceInfo.deviceName() << deviceInfo.supportedCodecs();
    }
    //audioOutput = new QAudioOutput(QAudioDeviceInfo::defaultOutputDevice(),format,this);
    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),SLOT(finishedPlaying(QAudio::State)));
    vf->start();
    //outputFile=audioOutput->start();
    qDebug()<<"output started";
}


void MusicPlayer::on_but_device_clicked()
{
    startPlaying();
    decode();
    ui->but_stopDevice->setEnabled(true);
    ui->but_device->setEnabled(false);
    ui->box_devices->setEnabled(false);
}


void MusicPlayer::on_box_devices_currentIndexChanged(const QString &arg1)
{
    curDeviceName=arg1;
    qDebug()<<curDeviceName;
}

void MusicPlayer::on_but_stopDevice_clicked()
{
    m_decoder->stop();
    qDebug()<<"decode stopped";
    //disconnect decoder signals
    disconnect(m_decoder,SIGNAL(bufferReady()),this,SLOT(decodeDone()));
    disconnect(m_decoder,SIGNAL(finished()),this,SLOT(decodeFinished()));
    audioOutput->stop();
    vf->stop();
    delete m_decoder;
    delete audioOutput;
    delete vf;
    arrCnt=0;
    //vf->clear();
    ui->but_stopDevice->setEnabled(false);
    ui->but_device->setEnabled(true);
    ui->box_devices->setEnabled(true);
}
