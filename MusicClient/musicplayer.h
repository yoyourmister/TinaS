#ifndef MUSICPLAYER_H
#define MUSICPLAYER_H

#include <QWidget>
#include <QtCore>
#include <QtGui>
#include <QtMultimedia>
#include <QListWidget>
#include <QtNetwork>
#include <QFileDialog>
//#include <QMediaPlayer>

namespace Ui {
class MusicPlayer;
}

class MusicPlayer : public QWidget
{
    Q_OBJECT
    
public:
    explicit MusicPlayer(QWidget *parent = 0);
    ~MusicPlayer();
    
    void clientConnect(QString hostname);
    void addMusicFile();

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();

private slots:
    void on_but_Mute_clicked();

    void on_but_connect_clicked();

    void on_pushButton_clicked();

    void on_slider_Volume_valueChanged(int value);

    void on_pushButton_2_clicked();

private:
    Ui::MusicPlayer *ui;
    QMediaPlayer mediaPlayer;
    QMediaPlaylist *playlist;
    QMediaPlaylist *currentPlaylist;
    QTcpSocket *socket;
    QString IPaddress;
    QHash<QString,int> hashPlaylist;
};

#endif // MUSICPLAYER_H
