#include "musicserver.h"

MusicServer::MusicServer(QObject *parent) :
    QTcpServer(parent)
{
}

void MusicServer::startServer()
{
    if(!this->listen(QHostAddress::Any,7777))
    {
        qDebug()<<"Could not start Server";
    }
    else
    {
        qDebug()<<"Listening...";
    }
}

void MusicServer::sendPlaylist(QList<QString> songNames)
{
    qDebug()<<"Send List...";
    QString sendList="list";
    for (int i=0; i<songNames.size(); ++i) {
        sendList.append(songNames.at(i));
    }
    for (int i=0; i<threadlist.size(); i++)
    {
        threadlist.at(i)->sendData(sendList);
    }
    qDebug()<<"List sent!";
}

void MusicServer::sendSong(QString songName)
{
    qDebug()<<"Send Song...";
    for (int i=0; i<threadlist.size(); i++)
    {
        threadlist.at(i)->sendData("play"+songName);
    }
}

void MusicServer::sendSound(QString soundName)
{
}

void MusicServer::gotData(int ID, QByteArray data)
{
    qDebug() << ID << " " << data;
    if (data=="resync") {

    }
    for (int i=0; i<threadlist.size(); i++) {
        if (threadlist.at(i)->socketDescriptor==ID) {
            threadlist.at(i)->sendData("");
        }
    }
}


void MusicServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "Connecting..." << socketDescriptor;
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    threadlist.append(thread);
    connect(thread, SIGNAL(finished()),thread, SLOT(deleteLater()));
    thread->start();
}
