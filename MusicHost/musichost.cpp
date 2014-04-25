#include "musichost.h"
#include "ui_musichost.h"
#include "QMessageBox.h"

MusicHost::MusicHost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicHost)
{
    ui->setupUi(this);
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

void MusicHost::on_but_getIP_clicked()
{

    //get the external ip over
    QNetworkAccessManager *accessManager = new QNetworkAccessManager();
    accessManager->get(QNetworkRequest(QUrl("http://icanhazip.com/")));
    connect(accessManager, SIGNAL(finished(QNetworkReply*)), SLOT(assignIP(QNetworkReply*)));

    //QHostAddress address = this->getLocalIP();
//    qDebug() << address.toString();
//    if (!address.isNull())
//    {
//        ui->lineEdit_CurIP->setText(address.toString());
//        //select complete IP address in lineEditField
//        ui->lineEdit_CurIP->selectAll();
//        //copy address to clipboard
//        ui->lineEdit_CurIP->copy();
//        //set the focus to the lineEditField
//        ui->lineEdit_CurIP->setFocus();
//    }
}

//QHostAddress MusicHost::getLocalIP()
//{
//    QHostAddress localAddress;
//    foreach (const QHostAddress &address, QNetworkInterface::allAddresses()) {
//        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != QHostAddress(QHostAddress::LocalHost))
//            localAddress = address;

//    }
//    return localAddress;
//}

void MusicHost::assignIP(QNetworkReply *reply)
{
    if(reply->error() == QNetworkReply::NoError)
    {
        ui->lineEdit_CurIP->setText(reply->readAll());
        //select complete IP address in lineEditField
        ui->lineEdit_CurIP->selectAll();
        //copy address to clipboard
        ui->lineEdit_CurIP->copy();
        //set the focus to the lineEditField
        ui->lineEdit_CurIP->setFocus();
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not get your external IP address from http://icanhazip.com/");
    }
    reply->deleteLater();
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

void MusicHost::on_but_removeFromList_clicked()
{
    QList<QListWidgetItem*> list=ui->listWidget_playlist->selectedItems();
    for (int i=0; i<list.size(); ++i) {
        ui->listWidget_playlist->takeItem(i);
    }
}

void MusicHost::on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item)
{
        delete ui->listWidget_playlist->item(ui->listWidget_playlist->row(item));
}

void MusicHost::on_but_debug_clicked()
{
    if (!musicserver) {
        return;
    }
    musicserver->dumpDebugInfo();
}
