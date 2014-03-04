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
private:
    QList<ServerThread*> threadlist;

signals:

public slots:

protected:
    void incomingConnection(int socketDescriptor);

};

#endif // MUSICSERVER_H
