#include "musicplayer.h"
#include "ui_musicplayer.h"

bool MusicPlayer::loadConfigFile() {
    QString configFile=QDir::currentPath()+"config.ini";
    QFile file(configFile);
    if (!file.open(QIODevice::ReadOnly)) {
        log("IO","Load config file: could not open file",MsgType::WARNING_LOG);
        return false;
    }
    QDataStream in(&file);
    in.setVersion(QDataStream::Qt_5_1);

    in >> defaultdir;

    file.close();
    log("IO","Config file loaded",MsgType::SUCCESS_LOG);
    return true;
}

bool MusicPlayer::saveConfigFile() {
    QString configFile=QDir::currentPath()+"config.ini";
    QFile file(configFile);
    if (!file.open(QIODevice::WriteOnly)) {
        log("IO","Save config file: could not open file",MsgType::ERROR_LOG);
        return false;
    }
    QDataStream out(&file);
    out.setVersion(QDataStream::Qt_5_1);

    out << defaultdir;

    file.close();
    log("IO","Config file saved",MsgType::SUCCESS_LOG);
    return true;
}

MusicPlayer::MusicPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);
    //hide logging stuff
    ui->logWidget->hide();
    ui->check_advancedFields->hide();
    //hide legend labels of logging
    ui->label_log_i->hide();
    ui->label_log_s->hide();
    ui->label_log_w->hide();
    ui->label_log_e->hide();
    ui->label_log_u->hide();
    //hide unused stuff
    ui->but_device->hide();
    ui->box_devices->hide();
    ui->but_stopDevice->hide();
    ui->label_audioDevicce->hide();
    ui->progressBar->hide();

    //hide advanced fields in log window
    ui->logWidget->hideColumn(0);
    ui->logWidget->hideColumn(2);

    //creating the manual signal slot connections
    this->CreateConnections();

    socket=new QTcpSocket(this);

    defaultdir="/home";
    loadConfigFile();
    log("Constructor","Default dir: "+defaultdir,MsgType::INFO_LOG);

    QList<QAudioDeviceInfo> infoList=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo &deviceInfo, infoList) {
        ui->box_devices->addItem(deviceInfo.deviceName());
        log("Constructor","Device name: "+deviceInfo.deviceName()+" Codecs: "+deviceInfo.supportedCodecs().join(" "),MsgType::INFO_LOG);
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
    connect(&playlist, SIGNAL(currentIndexChanged(int)), this, SLOT(playlistIndexChanged(int)));
    //connect(&currentPlaylist, SIGNAL(currentIndexChanged(int)), this, SLOT(curPlaylistIndexChanged()));
    //user control of playback
    connect(ui->list_Tracks, SIGNAL(doubleClicked(QModelIndex)), this, SLOT(track_doubleclicked(QModelIndex)));
    //connect(ui->slider_Playtime, SIGNAL(sliderMoved(int)), this, SLOT(on_trackPositionChanged(sliderMoved(int))));
    //logging advanced fields checkbox
    connect(ui->check_advancedFields, SIGNAL(stateChanged(int)), this, SLOT(advFieldsCheck()));
  }
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
    delete socket;
}

void MusicPlayer::advFieldsCheck()
{
    if(ui->check_advancedFields->isChecked())
    {
        //show advanced fields in log window
        ui->logWidget->showColumn(0);
        ui->logWidget->showColumn(2);
    }else
    {
        //hide advanced fields in log window
        ui->logWidget->hideColumn(0);
        ui->logWidget->hideColumn(2);
    }
}

void MusicPlayer::playlistIndexChanged(int index)
{
    if (ui->list_Tracks->currentRow()!=-1)
        ui->list_Tracks->currentItem()->setTextColor(QColor("black"));
    ui->list_Tracks->setCurrentRow(index);
    ui->list_Tracks->currentItem()->setTextColor(QColor("green"));
}

void MusicPlayer::curPlaylistIndexChanged()
{
    if (ui->list_Tracks->currentRow()!=-1)
        ui->list_Tracks->currentItem()->setTextColor(QColor("black"));
    // kinda brute force... if songs have same name we have a problem else this works fine :)
    QString currentItemName = ui->list_Tracks->currentItem()->text();
    int listWidgetSize = ui->list_Tracks->count();
    for (int k = 0; k < listWidgetSize; ++k)
    {
        if (ui->list_Tracks->item(k)->text().startsWith(currentItemName))
        {
            ui->list_Tracks->setCurrentRow(k);
            ui->list_Tracks->currentItem()->setTextColor(QColor("green"));
        }
    }
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
    log("MediaPlayer","State changed: "+QString::number(state),MsgType::INFO_LOG);
}

void MusicPlayer::track_doubleclicked(QModelIndex index)
{
    int curIndex = playlist.currentIndex();
    mediaPlayer.stop();
    if (curIndex != -1 && curIndex!=index.row())
        ui->list_Tracks->item(curIndex)->setTextColor(QColor("black"));
    playlist.setCurrentIndex(index.row());
    mediaPlayer.play();
    ui->but_play->setText("Pause");
}

void MusicPlayer::on_slider_Volume_valueChanged(int value)
{
    mediaPlayer.setVolume(value);
    ui->label_Volume->setText(QString::number(value));
    log("MediaPlayer","Volume changed: "+QString::number(mediaPlayer.volume()),MsgType::INFO_LOG);
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
    log("MediaPlayer","Mute "+QString::number(mediaPlayer.isMuted()),MsgType::INFO_LOG);
}

void MusicPlayer::clientConnect(QString hostname)
{
    connect(socket,SIGNAL(connected()), this, SLOT(connected()));
    connect(socket,SIGNAL(disconnected()), this, SLOT(disconnected()));
    connect(socket,SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket,SIGNAL(bytesWritten(qint64)),this, SLOT(bytesWritten(qint64)));

    log("Socket","Connecting...",MsgType::INFO_LOG);

    socket->connectToHost(hostname,7777);

    log("Socket","Waiting...",MsgType::INFO_LOG);

    if(!socket->waitForConnected(1000))
    {
        log("Socket","Error: "+socket->errorString(),MsgType::ERROR_LOG);
    }else
    {
        log("Socket","Established",MsgType::SUCCESS_LOG);
    }
}


void MusicPlayer::addMusicFile(QString dir)
{
    QDir directory = QDir(dir);
    //filter for mp3/mp4 files only
    //directory.setNameFilters(QStringList() << "*.avi" << "*.wav" << "*.mp3" << "*.mp4");
    QFileInfoList musicDir=directory.entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=directory.entryInfoList(QDir::Files);

    for (int i=2; i<musicDir.size(); i++)
    {
        /*if (musicDir.at(i).isDir()) {
            musicDir.append(QDir(musicDir.at(i)).entryInfoList(QDir::))
        }*/
        musicFiles.append(QDir(musicDir.at(i).absoluteFilePath()).entryInfoList(QDir::Files));
    }

    QList<QMediaContent> musicList;
    QMediaContent musicfile;
    for (int i=0; i<musicFiles.size(); i++)
    {
        musicfile=QMediaContent(QUrl::fromLocalFile(musicFiles.at(i).absoluteFilePath()));
        musicList.append(musicfile);
    }
    playlist.clear();
    ui->list_Tracks->clear();
    playlist.setPlaybackMode(QMediaPlaylist::Loop);
    playlist.addMedia(musicList);
    for (int i=0; i<playlist.mediaCount(); i++)
    {
        QString filename=playlist.media(i).canonicalUrl().toString();
        filename=filename.mid(filename.lastIndexOf("/")+1);
        ui->list_Tracks->addItem(filename);
        log("MediaPlayer","Add to list: "+QString::number(i)+" "+filename,MsgType::INFO_LOG);
    }
    mediaPlayer.setPlaylist(&playlist);

}

void MusicPlayer::connected()
{
    log("Socket","Connected",MsgType::SUCCESS_LOG);
    //set global flag for host connection
    isConnected = true;
    //deactivate every GUI element for client playback control
    ui->but_play->setEnabled(false);
    ui->but_stop->setEnabled(false);
    ui->list_Tracks->setEnabled(false);
    ui->but_addFolder->setEnabled(false);
    ui->but_next->setEnabled(false);
    ui->but_connect->setText("Disconnect");
}

void MusicPlayer::disconnected()
{
    log("Socket","Disconnected",MsgType::WARNING_LOG);
    //unset global flag for host connection
    isConnected = false;
    //activate every GUI element for client playback control
    ui->but_play->setEnabled(true);
    ui->but_stop->setEnabled(true);
    ui->list_Tracks->setEnabled(true);
    ui->but_addFolder->setEnabled(true);
    ui->but_next->setEnabled(true);
    ui->but_connect->setText("Connect");
}

void MusicPlayer::bytesWritten(qint64 bytes)
{
    log("Socket","Wrote: "+QString::number(bytes),MsgType::INFO_LOG);
}


void MusicPlayer::readyRead()
{
    log("Socket","Reading...",MsgType::INFO_LOG);
    QByteArray readbytes=socket->readAll();
    QString readString(readbytes);
    log("Socket",QString(readbytes),MsgType::INFO_LOG);

    if (readString.left(4)=="play")
    {
        log("Socket","Recieved: play",MsgType::INFO_LOG);
        QString title=readString.mid(4);
        for (int i=0; i<mediaPlayer.playlist()->mediaCount(); i++)
        {
            QString filename=mediaPlayer.playlist()->media(i).canonicalUrl().toString();
            filename=filename.mid(filename.lastIndexOf("/")+1);
            if (filename.compare(title)==0) {
                log("MediaPlayer","Play: "+filename,MsgType::INFO_LOG);
                mediaPlayer.playlist()->setCurrentIndex(i);
            }
        }
        mediaPlayer.play();
    }

    if (readString.left(4)=="list") {
        log("Socket","Recieved: list",MsgType::INFO_LOG);
        QString list=readString.mid(4);
        QStringList titles=list.split(":");
        //currentPlaylist=new QMediaPlaylist();
        currentPlaylist.clear();
        for (int i=0; i<playlist.mediaCount(); i++)
        {
            QString filename=playlist.media(i).canonicalUrl().toString();
            filename=filename.mid(filename.lastIndexOf("/")+1);
            log("MediaPlayer","Playlist filename: "+filename,MsgType::INFO_LOG);
            for (int j=0; j<titles.size(); ++j) {
                log("MediaPlayer","Current playlist add: "+titles.at(j),MsgType::INFO_LOG);
                if (filename==titles.at(j)) {
                    currentPlaylist.addMedia(playlist.media(i));
                }
            }
        }
        mediaPlayer.setPlaylist(&currentPlaylist);
        mediaPlayer.play();
    }

    if (readString.left(4)=="sync") {
        log("Socket","Recieved: sync",MsgType::INFO_LOG);
        QStringList properties=readString.mid(4).split(":");
        mediaPlayer.playlist()->setCurrentIndex(properties.at(0).toInt());
        mediaPlayer.setPosition(properties.at(1).toLongLong());
    }

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

void MusicPlayer::on_but_addFolder_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),defaultdir, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    log("IO","AddFolder: "+dir,MsgType::INFO_LOG);
    if (dir!="") {
        addMusicFile(dir);
        defaultdir=dir;
        saveConfigFile();
        calculateMD5();
    }
}

void MusicPlayer::on_but_next_clicked()
{
    if (mediaPlayer.playlist())
    {
        mediaPlayer.playlist()->next();
        log("MediaPlayer","Playlist index: "+QString::number(mediaPlayer.playlist()->currentIndex()),MsgType::INFO_LOG);
    }
}

void MusicPlayer::on_but_play_clicked()
{
    log("MediaPlayer","Audio avail.: "+QString::number(mediaPlayer.isAudioAvailable()),MsgType::INFO_LOG);
    if (mediaPlayer.state()==QMediaPlayer::PlayingState) {
        mediaPlayer.pause();
        ui->but_play->setText("Play");
    } else if (mediaPlayer.state()==QMediaPlayer::StoppedState && mediaPlayer.isAudioAvailable())
    {
        //the currently selected item will be played next
        int index = ui->list_Tracks->currentIndex().row();
        //if nothing is selected first track is played
        if (index<0) index = 0;
        playlist.setCurrentIndex(index);
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
    log("Decoder","Finished",MsgType::SUCCESS_LOG);
}

void MusicPlayer::decodeDone() {
    log("Decoder","Done "+QString::number(arrCnt),MsgType::SUCCESS_LOG);
    QAudioFormat qaf=m_decoder->audioFormat();
    log("Decoder",QString::number(qaf.channelCount())
        + qaf.codec() +
        QString::number(qaf.sampleRate()) +
        QString::number(qaf.sampleSize()) +
        QString::number(qaf.byteOrder()) +
        QString::number(qaf.sampleType()),MsgType::INFO_LOG);
    m_arr[arrCnt]=m_decoder->read();
    vf->writeData(static_cast<const char*>(m_arr[arrCnt].data()),m_arr[arrCnt].byteCount());
    if (arrCnt==0) {
        audioOutput->start(vf);
    }
    arrCnt++;
}

void MusicPlayer::decode()
{
    log("Decoder","Starting decode...",MsgType::INFO_LOG);
    m_decoder=new QAudioDecoder();

    connect(m_decoder,SIGNAL(bufferReady()),this,SLOT(decodeDone()));
    connect(m_decoder,SIGNAL(finished()),this,SLOT(decodeFinished()));
    //decoder->setSourceFilename("J:/Musik/Snap - Oops Up.mp3");
    QString musicfile=QDir::currentPath()+"/Erdenstern - Snow Queen.mp3";  //"/Snap - Oops Up.mp3";//
    log("Decoder","File: "+musicfile,MsgType::INFO_LOG);
    m_decoder->setSourceFilename(musicfile);
    m_decoder->start();
    log("Decoder","Decode started",MsgType::INFO_LOG);
}

void MusicPlayer::finishedPlaying(QAudio::State state)
{
    if (state == QAudio::IdleState) {
        log("MusicPlayer","Finished playing",MsgType::SUCCESS_LOG);
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

    log("Remote Playback","Start output",MsgType::INFO_LOG);
    QList<QAudioDeviceInfo> infoList=QAudioDeviceInfo::availableDevices(QAudio::AudioOutput);
    foreach(const QAudioDeviceInfo &deviceInfo, infoList) {
        if (deviceInfo.deviceName()==curDeviceName) {
            log("Remote Playback",deviceInfo.deviceName(),MsgType::INFO_LOG);
            audioOutput = new QAudioOutput(deviceInfo,format,this);
        }
    }

    connect(audioOutput,SIGNAL(stateChanged(QAudio::State)),this,SLOT(finishedPlaying(QAudio::State)));
    vf->start();
    log("Remote Playback","Output started",MsgType::INFO_LOG);
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
    log("AudioDevice","Changed to "+curDeviceName,MsgType::INFO_LOG);
}

void MusicPlayer::on_but_stopDevice_clicked()
{
    m_decoder->stop();
    log("Decoder","Decode stopped",MsgType::INFO_LOG);
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

//just a simple test method
void MusicPlayer::calculateMD5()
{
    QFile file;
    QByteArray hashData;
    QString filename;
    //loop over playlist entries and generate for each file the MD5 hash
    for (int i=0; i<playlist.mediaCount(); i++)
    {
        filename = playlist.media(i).canonicalUrl().toLocalFile();
        file.setFileName(filename);
        if (file.open(QIODevice::ReadOnly)) {
            hashData = QCryptographicHash::hash(file.readAll(), QCryptographicHash::Md5);
            log("MD5 Hash",playlist.media(i).canonicalUrl().fileName()+" => "+QString(hashData.toHex()),MsgType::INFO_LOG);
        }
        file.close();
    }
}

void MusicPlayer::log(QString cat, QString entry, MsgType type)
{
    //QColor textColor;
    QString typeString;
    QBrush textColor;

    switch (type)
    {
    case ERROR_LOG:
        textColor = Qt::red; //color => red
        typeString="error";
        break;

    case WARNING_LOG:
        textColor = Qt::darkYellow; //color => gold
        typeString="warning";
        break;

    case INFO_LOG:
        textColor = Qt::black; //color => black
        typeString="info";
        break;

    case SUCCESS_LOG:
        textColor = Qt::darkGreen; //color => green
        typeString="success";
        break;

    default:
        textColor = Qt::blue; //color => blue
        typeString="unknown";
    }

    //get system time
    QTableWidgetItem* itemTime = new QTableWidgetItem(QTime::currentTime().toString(tr("hh:mm:ss.zzz")));
    itemTime->setForeground(textColor);
    QTableWidgetItem* itemCat = new QTableWidgetItem(cat);
    itemCat->setForeground(textColor);
    QTableWidgetItem* itemType = new QTableWidgetItem(typeString);
    itemType->setForeground(textColor);
    QTableWidgetItem* itemMsg = new QTableWidgetItem(entry);
    itemMsg->setForeground(textColor);

    //write to console as debug output
    qDebug() << entry;

    //write into table widget as new entry
    ui->logWidget->insertRow(0);
    //fill row of table with event informations
    ui->logWidget->setItem(0,0, itemTime);
    ui->logWidget->setItem(0,1, itemCat);
    ui->logWidget->setItem(0,2, itemType);
    ui->logWidget->setItem(0,3, itemMsg);
}

void MusicPlayer::on_but_showlog_clicked()
{
    if(ui->but_showlog->isChecked())
    {
        ui->logWidget->show();
        ui->check_advancedFields->show();
        ui->but_showlog->setText("↓ Log ↓");
        //show legend labels
        ui->label_log_i->show();
        ui->label_log_s->show();
        ui->label_log_w->show();
        ui->label_log_e->show();
        ui->label_log_u->show();
    }else
    {
        ui->logWidget->hide();
        ui->check_advancedFields->hide();
        ui->but_showlog->setText("Show Log");
        //hide legend labels
        ui->label_log_i->hide();
        ui->label_log_s->hide();
        ui->label_log_w->hide();
        ui->label_log_e->hide();
        ui->label_log_u->hide();
    }
}

void MusicPlayer::on_but_log2clip_clicked()
{
    //TODO
}
