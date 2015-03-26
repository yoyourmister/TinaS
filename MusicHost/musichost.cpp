#include "musichost.h"
#include "ui_musichost.h"
#include "QMessageBox.h"

MusicHost::MusicHost(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MusicHost)
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
    //hide advanced fields in log window
    ui->logWidget->hideColumn(0);
    ui->logWidget->hideColumn(2);

    //creating the manual signal slot connections
    this->CreateConnections();
}

MusicHost::~MusicHost()
{
    delete ui;
}

void MusicHost::CreateConnections()
{
  if ( ui )
  {
    //logging advanced fields checkbox
    connect(ui->check_advancedFields, SIGNAL(stateChanged(int)), this, SLOT(advFieldsCheck()));
  }
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
    log("IP","Trying to retriev IP...",MsgType::INFO_LOG);
    if(reply->error() == QNetworkReply::NoError)
    {
        ui->lineEdit_CurIP->setText(reply->readAll());
        //select complete IP address in lineEditField
        ui->lineEdit_CurIP->selectAll();
        //copy address to clipboard
        ui->lineEdit_CurIP->copy();
        //set the focus to the lineEditField
        ui->lineEdit_CurIP->setFocus();
        log("IP","IP retrieved",MsgType::SUCCESS_LOG);
    }
    else
    {
        QMessageBox::warning(this, "Error", "Could not get your external IP address from http://icanhazip.com/");
        log("IP","Error retrieving IP",MsgType::ERROR_LOG);
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

    if(size>0) {
        log("Host","Sending list to clients",MsgType::INFO_LOG);
        musicserver->sendPlaylist(list);
    } else {
        log("Host","List is empty",MsgType::WARNING_LOG);
    }
}

void MusicHost::on_but_sendSound_clicked()
{
    log("Host","Sending sound not implemented",MsgType::WARNING_LOG);
    musicserver->sendSound("dummy");
}

void MusicHost::on_but_addFolder_clicked()
{
    //QString dir = QFileDialog::getExistingDirectory(this, tr("Open Directory"),"/home", QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    //addMusicFiles(dir);
    addMusicFiles("C:/Users/test/Desktop/Musik");
    log("File list","Trying to add files from \"C:/Users/test/Desktop/Musik\"",MsgType::INFO_LOG);
}

void MusicHost::addMusicFiles(QString dir)
{
    QMediaPlaylist *playlist;
    QFileInfoList musicDir=QDir(dir).entryInfoList(QDir::Dirs);
    QFileInfoList musicFiles=QDir(dir).entryInfoList(QDir::Files);

    if (musicDir.size()>0) {
        log("File list","Adding directory with subdirectories...",MsgType::INFO_LOG);
        for (int i=2; i<musicDir.size(); i++)
        {
            musicFiles.append(QDir(musicDir.at(i).absoluteFilePath()).entryInfoList(QDir::Files));
        }
    } else {
        log("File list","No directry found",MsgType::WARNING_LOG);
    }
    QList<QMediaContent> musicList;
    QMediaContent musicfile;

    if (musicDir.size()>0) {
        log("File list","Appending files from directory...",MsgType::INFO_LOG);
        for (int i=0; i<musicFiles.size(); i++)
        {
            musicfile=QMediaContent(QUrl::fromLocalFile(musicFiles.at(i).absoluteFilePath()));
            musicList.append(musicfile);
        }
    } else {
        log("File list","No files in directory",MsgType::WARNING_LOG);
    }
    playlist=new QMediaPlaylist();
    playlist->addMedia(musicList);
    for (int i=0; i<playlist->mediaCount(); i++)
    {
        QString filename=playlist->media(i).canonicalUrl().toString();
        filename=filename.mid(filename.lastIndexOf("/")+1);
        ui->listWidget_allSongs->addItem(filename);
        log("File list","Adding \"" + filename + "\"",MsgType::INFO_LOG);
        //qDebug() << i << " " << filename ;
    }
    //mediaPlayer.setPlaylist(playlist);
    //QString filename=QFileDialog::getOpenFileName(this,"Open Music File");
    //mediaPlayer.play();
}

void MusicHost::on_but_addToList_clicked()
{
    QList<QListWidgetItem*> list=ui->listWidget_allSongs->selectedItems();
    if (list.size()>0) {
    for (int i=0; i<list.size(); ++i) {
        log("Client playlist","Adding \"" + list.at(i)->text() + "\"",MsgType::INFO_LOG);
        ui->listWidget_playlist->addItem(list.at(i)->text());
    }
    } else {
        log("Client playlist","No files selected to add",MsgType::WARNING_LOG);
    }
}

void MusicHost::on_but_removeFromList_clicked()
{
    QList<QListWidgetItem*> list=ui->listWidget_playlist->selectedItems();

    if (list.size()>0) {
        for (int i=0; i<list.size(); ++i) {
            log("Client playlist","Removing \"" + list.at(i)->text() + "\"",MsgType::INFO_LOG);
            ui->listWidget_playlist->takeItem(ui->listWidget_playlist->row(list.at(i)));
        }
        log("Client playlist",list.size() + " titles removed",MsgType::INFO_LOG);
    } else {
        log("Client playlist","No files selected to remove",MsgType::WARNING_LOG);
    }
}

void MusicHost::on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item)
{
    log("Client playlist","Removed doubled-clicked title \"" + item->text() + "\"",MsgType::INFO_LOG);
    delete ui->listWidget_playlist->item(ui->listWidget_playlist->row(item));
}

void MusicHost::on_but_debug_clicked()
{
    if (!musicserver) {
        return;
    }
    musicserver->dumpDebugInfo();
}

void MusicHost::advFieldsCheck()
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
    musicserver->sendMessage("AdvancedFieldsClicked test message");
}

void MusicHost::log(QString cat, QString entry, MsgType type)
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

void MusicHost::on_but_showlog_clicked()
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
