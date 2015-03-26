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

#include "virtualfile.h"

namespace Ui {
class MusicPlayer;
}

class MusicPlayer : public QWidget
{
    Q_OBJECT
    
public:
    explicit MusicPlayer(QWidget *parent = 0);
    ~MusicPlayer();

    enum MsgType{
        ERROR_LOG,
        WARNING_LOG,
        INFO_LOG,
        SUCCESS_LOG,
        UNKNOWN_LOG
    };

    bool loadConfigFile();
    bool saveConfigFile();
    void clientConnect(QString hostname);
    void addMusicFile(QString dir);

    void decode();
    void startPlaying();

public slots:
    void connected();
    void disconnected();
    void bytesWritten(qint64 bytes);
    void readyRead();
    void finishedPlaying(QAudio::State state);

private slots:
    void deletePlaylistItem();

    void advFieldsCheck();

    void on_but_log2clip_clicked();

    void on_but_connect_clicked();

    void on_but_addFolder_clicked();

    void on_but_resync_clicked();

    void on_but_device_clicked();

    void decodeDone();

    void decodeFinished();

    void on_box_devices_currentIndexChanged(const QString &arg1);

    void on_but_stopDevice_clicked();

    void on_but_showlog_clicked();

    //user control of playback
    void on_but_play_clicked();
    void track_doubleclicked(QModelIndex index);
    void on_but_stop_clicked();
    void on_but_next_clicked();
    //void on_trackPositionChanged(int position);

    //plackback info update
    void updatePlaytime(qint64 position);
    void updateSongDuration(qint64 length);
    void playerStateChanged(QMediaPlayer::State state);
    void playlistIndexChanged(int index);
    void curPlaylistIndexChanged();

    //general playback
    void on_but_Mute_toggled();
    void on_slider_Volume_valueChanged(int value);

private:
    void calculateMD5();
    void CreateConnections();
    void log(QString cat, QString entry, MsgType type = MsgType::UNKNOWN_LOG);

    Ui::MusicPlayer *ui;
    QMediaPlayer mediaPlayer;
    QPointer<QAudioDecoder> m_decoder;
    QMediaPlaylist playlist;
    QMediaPlaylist currentPlaylist;
    QTcpSocket *socket;
    QString IPaddress;
    QHash<QString,int> hashPlaylist;
    QString defaultdir;

    QPointer<VirtualFile> vf;
    QPointer<QAudioOutput> audioOutput; // class member.
    QString curDeviceName;
    bool isConnected;
};

#endif // MUSICPLAYER_H
