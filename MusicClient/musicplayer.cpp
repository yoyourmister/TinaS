#include "musicplayer.h"
#include "ui_musicplayer.h"

MusicPlayer::MusicPlayer(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicPlayer)
{
    ui->setupUi(this);

    socket=new QTcpSocket(this);
}

MusicPlayer::~MusicPlayer()
{
    delete ui;
    if (playlist)
    {
        delete playlist;
    }
    delete socket;
}

void MusicPlayer::on_slider_Volume_valueChanged(int value)
{
    if (mediaPlayer.state()==QMediaPlayer::PlayingState)
    {
        mediaPlayer.setVolume(value);
    }
    qDebug()<<mediaPlayer.volume();
}

void MusicPlayer::on_but_Mute_clicked()
{
    if (mediaPlayer.isMuted())
    {
        mediaPlayer.setMuted(false);
    }
    else
    {
        mediaPlayer.setMuted(true);
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


void MusicPlayer::addMusicFile()
{
    QFileInfoList musicDir=QDir("C:/Users/test/Desktop/Musik").entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=QDir("C:/Users/test/Desktop/Musik").entryInfoList(QDir::Files);

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
    mediaPlayer.play();
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
}

void MusicPlayer::on_but_connect_clicked()
{
    IPaddress=ui->editIP->text(); //="127.0.0.1";
    clientConnect(IPaddress);
}

void MusicPlayer::on_pushButton_clicked()
{
    addMusicFile();
}

void MusicPlayer::on_pushButton_2_clicked()

{
    mediaPlayer.playlist()->next();
    qDebug()<<(mediaPlayer.playlist()->currentIndex());
    //playlist->mediaInserted();
}
