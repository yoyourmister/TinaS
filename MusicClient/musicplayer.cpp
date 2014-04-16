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
    this->CreateConnections();
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
    //set global flag for host connection (is client connected to host?)
    isConnected = false;
}

void MusicPlayer::CreateConnections()
{
  if ( ui )
  {
    //general playback controls (always active)
    connect( ui->but_Mute, SIGNAL(clicked()), this, SLOT(on_but_Mute_toggled()) );
    //plackback info update
    connect(&mediaPlayer, SIGNAL(positionChanged(qint64)), this, SLOT(updatePlaytime(qint64)));
    connect(&mediaPlayer, SIGNAL(durationChanged(qint64)), this, SLOT(updateSongDuration(qint64)));
    connect(&mediaPlayer, SIGNAL(stateChanged(QMediaPlayer::State)), this, SLOT(playerStateChanged(QMediaPlayer::State)));
    //user control of playback
    connect(ui->list_Tracks, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(on_track_doubleclicked(QModelIndex)));
    //connect(ui->slider_Playtime, SIGNAL(sliderMoved(int)), this, SLOT(on_trackPositionChanged(sliderMoved(int))));
  }
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
    delete socket;
}

void MusicPlayer::updateSongDuration(qint64 length)
{
    length = qRound(length/1000.0);
    ui->slider_Playtime->setRange(0, length);
    QTime duration(0, length / 60, length % 60);
    ui->label_Duration->setText(duration.toString(tr("mm:ss")));
}

void MusicPlayer::updatePlaytime(qint64 position)
{
    position = qRound(position/1000.0);
    ui->slider_Playtime->setValue(position);
    QTime duration(0, position / 60, position % 60);
    ui->label_Playtime->setText(duration.toString(tr("mm:ss")));
}

//void MusicPlayer::on_trackPositionChanged(int position)
//{
//    if(mediaPlayer.state()==QMediaPlayer::PlayingState)
//        ui->slider_Playtime->setValue(position);
//}

void MusicPlayer::playerStateChanged(QMediaPlayer::State state) {
    qDebug()<<state;
}

void MusicPlayer::on_track_doubleclicked(QModelIndex index)
{
    mediaPlayer.stop();
    playlist->setCurrentIndex(index.row());
    mediaPlayer.play();
    ui->but_play->setText("Pause");
}

void MusicPlayer::on_slider_Volume_valueChanged(int value)
{
    mediaPlayer.setVolume(value);
    ui->label_Volume->setText(QString::number(value));
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

    socket->connectToHost(hostname,7777);

    qDebug()<<"waiting...";

    if(!socket->waitForConnected(1000))
    {
        qDebug()<<"Error:"<< socket->errorString();
    }
    //qDebug()<<"established";
}


void MusicPlayer::addMusicFile(QString dir)
{
    QDir directory = QDir(dir);
    //filter for mp3/mp4 files only
    directory.setNameFilters(QStringList() << "*.avi" << "*.wav" << "*.mp3" << "*.mp4");
    QFileInfoList musicDir=directory.entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=directory.entryInfoList(QDir::Files);

    for (int i=2; i<musicDir.size(); i++)
    {
        musicFiles.append(QDir(musicDir.at(i).absoluteFilePath()).entryInfoList(QDir::Files));
    }
    QList<QMediaContent> musicList;
    QMediaContent musicfile;
    for (int i=0; i<musicFiles.size(); i++)
    {
        musicfile=QMediaContent(QUrl::fromLocalFile(musicFiles.at(i).absoluteFilePath()));
        musicList.append(musicfile);
    }
    playlist=new QMediaPlaylist();
    playlist->setPlaybackMode(QMediaPlaylist::Loop);
    playlist->addMedia(musicList);
    for (int i=0; i<playlist->mediaCount(); i++)
    {
        QString filename=playlist->media(i).canonicalUrl().toString();
        filename=filename.mid(filename.lastIndexOf("/")+1);
        ui->list_Tracks->addItem(filename);
        qDebug() << i << " " << filename ;
    }
    mediaPlayer.setPlaylist(playlist);

    ui->label_error->setText(mediaPlayer.errorString());
}

void MusicPlayer::connected()
{
    qDebug()<<"Connected";
    //set global flag for host connection
    isConnected = true;
    qDebug()<<"isConnected=" << isConnected;
    //deactivate every GUI element for client playback control
    ui->but_play->setEnabled(false);
    ui->but_stop->setEnabled(false);
    ui->list_Tracks->setEnabled(false);
    ui->but_connect->setText("Disconnect");
}

void MusicPlayer::disconnected()
{
    qDebug()<<"Disconnected";
    //unset global flag for host connection
    isConnected = false;
    qDebug()<<"isConnected=" << isConnected;
    //activate every GUI element for client playback control
    ui->but_play->setEnabled(true);
    ui->but_stop->setEnabled(true);
    ui->list_Tracks->setEnabled(true);
    ui->but_connect->setText("Connect");
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
    if (isConnected) {
        socket->disconnectFromHost();

    } else
    {
        IPaddress=ui->editIP->text();
        clientConnect(IPaddress);
    }
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
}

void MusicPlayer::on_but_play_clicked()
{
    qDebug()<<"Audio avail.: "<<mediaPlayer.isAudioAvailable();
    if (mediaPlayer.state()==QMediaPlayer::PlayingState) {
        mediaPlayer.pause();
        ui->but_play->setText("Play");
    } else if (mediaPlayer.state()==QMediaPlayer::StoppedState && mediaPlayer.isAudioAvailable())
    {
        //the currently selected item will be played next
        int index = ui->list_Tracks->currentIndex().row();
        //if nothing is selected first track is played
        if (index<0) index = 0;
        playlist->setCurrentIndex(index);
        mediaPlayer.play();
        ui->but_play->setText("Pause");
    } else if (mediaPlayer.state()==QMediaPlayer::PausedState && mediaPlayer.isAudioAvailable())
    {
        //player is paused and should now play from last position
        mediaPlayer.play();
        ui->but_play->setText("Pause");
    }
}

void MusicPlayer::on_but_stop_clicked()
{
    mediaPlayer.stop();
    ui->but_play->setText("Play");
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

    connect(m_decoder,SIGNAL(bufferReady()),this,SLOT(decodeDone()));
    connect(m_decoder,SIGNAL(finished()),this,SLOT(decodeFinished()));
    //decoder->setSourceFilename("J:/Musik/Snap - Oops Up.mp3");
    QString musicfile=QDir::currentPath()+"/Erdenstern - Snow Queen.mp3";  //"/Snap - Oops Up.mp3";//
    qDebug()<<musicfile;
    m_decoder->setSourceFilename(musicfile);
    m_decoder->start();
    qDebug()<<"decode started";
}

void MusicPlayer::finishedPlaying(QAudio::State state)
{
    if (state == QAudio::IdleState) {
        qDebug()<<"Finished Playing";
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
    }

    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(finishedPlaying(QAudio::State)));
    vf->start();
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
    audioOutput->stop();
    vf->stop();
    delete m_decoder;
    delete audioOutput;
    delete vf;
    arrCnt=0;
    ui->but_stopDevice->setEnabled(false);
    ui->but_device->setEnabled(true);
    ui->box_devices->setEnabled(true);
}
