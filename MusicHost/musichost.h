#ifndef MUSICHOST_H
#define MUSICHOST_H

#include <QWidget>
#include <QtMultimedia>
#include <QFileInfoList>
#include <QListWidget>
#include <QFileDialog>

#include "musicserver.h"

namespace Ui {
class MusicHost;
}

class MusicHost : public QWidget
{
    Q_OBJECT
    
public:
    explicit MusicHost(QWidget *parent = 0);
    ~MusicHost();


private slots:
    void on_but_host_clicked();

    void on_but_getIP_clicked();

    void on_but_sendList_clicked();

    void on_but_sendSound_clicked();

    void on_but_addFolder_clicked();

    void on_but_addToList_clicked();

    void on_but_removeFromList_clicked();

    void on_listWidget_playlist_itemDoubleClicked(QListWidgetItem *item);

    void assignIP(QNetworkReply *reply);

    void on_but_debug_clicked();

    void on_but_clearSelection_clicked();

    void clientConnection(int descriptor, bool disconnect);

private:
    //QHostAddress getLocalIP();

    Ui::MusicHost *ui;
    MusicServer *musicserver;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist *globalPlaylist;
    QMediaPlaylist *currentPlaylist;

    void addMusicFiles(QString dir);
};

#endif // MUSICHOST_H
