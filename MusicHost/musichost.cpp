#include "musichost.h"
#include "ui_musichost.h"

MusicHost::MusicHost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicHost)
{
    ui->setupUi(this);
    mediaPlayer=new QMediaPlayer();
}

MusicHost::~MusicHost()
{
    delete ui;
}

void MusicHost::on_but_host_clicked()
{
    musicserver=new MusicServer(this);
    musicserver->startServer();
}

void MusicHost::on_but_sendList_clicked()
{
    QList<QString> list;
    int size=ui->listWidget_playlist->count();
    for (int i=0; i<size; ++i) {
        if (i<size-1) {
            list.append(ui->listWidget_playlist->item(i)->text()+":");
        } else {
            list.append(ui->listWidget_playlist->item(i)->text());
        }

    }
    musicserver->sendPlaylist(list);
}

void MusicHost::on_but_sendSound_clicked()
{
    musicserver->sendSound("dummy");
}

void MusicHost::on_but_addFolder_clicked()
{
    //QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //addMusicFiles(dir);
    addMusicFiles("C:/Users/test/Desktop/Musik");
}

void MusicHost::addMusicFiles(QString dir)
{
    QMediaPlaylist *playlist;
    QFileInfoList musicDir=QDir(dir).entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=QDir(dir).entryInfoList(QDir::Files);

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
    playlist->addMedia(musicList);
    for (int i=0; i<playlist->mediaCount(); i++)
    {
        QString filename=playlist->media(i).canonicalUrl().toString();
        filename=filename.mid(filename.lastIndexOf("/")+1);
        ui->listWidget_allSongs->addItem(filename);
        //qDebug() << i << " " << filename ;
    }
    //mediaPlayer.setPlaylist(playlist);
    //QString filename=QFileDialog::getOpenFileName(this,"Open Music File");
    //mediaPlayer.play();
}

void MusicHost::on_but_addToList_clicked()
{
    QList<QListWidgetItem*> list=ui->listWidget_allSongs->selectedItems();
    for (int i=0; i<list.size(); ++i) {
        ui->listWidget_playlist->addItem(list.at(i)->text());
    }
}

void MusicHost::on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item)
{
        delete ui->listWidget_playlist->item(ui->listWidget_playlist->row(item));
}
