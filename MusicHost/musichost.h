#ifndef MUSICHOST_H
#define MUSICHOST_H

#include <QWidget>
#include <QtMultimedia>
#include <QFileInfoList>
#include <QListWidget>

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

    enum MsgType{
        ERROR_LOG,
        WARNING_LOG,
        INFO_LOG,
        SUCCESS_LOG,
        UNKNOWN_LOG
    };

    void log(QString cat, QString entry, MsgType type = MsgType::UNKNOWN_LOG);

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

    void on_but_showlog_clicked();

    void advFieldsCheck();

private:
    //QHostAddress getLocalIP();
    void CreateConnections();

    Ui::MusicHost *ui;
    MusicServer *musicserver;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist *globalPlaylist;
    QMediaPlaylist *currentPlaylist;

    void addMusicFiles(QString dir);
};

#endif // MUSICHOST_H
