#ifndef MUSICSERVER_H
#define MUSICSERVER_H

#include <QTcpServer>
#include <QThreadPool>
#include <QDebug>
#include "serverthread.h"

class MusicServer : public QTcpServer
{
    Q_OBJECT
public:
    explicit MusicServer(QObject *parent = 0);
    void startServer();
    void sendPlaylist(QList<QString> songNames);
    void sendSong(QString songName);
    void sendSound(QString soundName);

    void dumpDebugInfo();

private:
    QList<ServerThread*> threadlist;


signals:
    void sendSignal(QString data);

public slots:
    void gotData(int ID, QByteArray data);
    void disconnectConnection(int ID);

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // MUSICSERVER_H
