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
        hosting=false;
    }
    else
    {
        qDebug()<<"Listening...";
        hosting=true;
    }
}

void MusicServer::dumpDebugInfo() {
    qDebug()<<"Info: "<<threadlist.size();
    for (int i=0; i<threadlist.size(); i++)
    {
        qDebug()<<threadlist.at(i)->socketDescriptor;//<<" "<<threadlist.at(i)->isRunning();//<<" "<<threadlist.at(i)->isFinished();
    }
}

void MusicServer::sendPlaylist(QList<QString> songNames)
{
    if (!hosting) {
        qDebug()<<"Host not active";
        return;
    }
    qDebug()<<"Send List... threadlistSize:"<<threadlist.size();
    QString sendList="list";
    for (int i=0; i<songNames.size(); ++i) {
        sendList.append(songNames.at(i));
    }
    //for (int i=0; i<threadlist.size(); i++)
    //{
        qDebug()<<"Send List to";//<<threadlist.at(i)->socketDescriptor;
        emit sendSignal(sendList);
        //threadlist.at(i)->sendData(sendList);
    //}
    qDebug()<<"List sent!";
}

void MusicServer::sendSong(QString songName)
{
    if (!hosting) {
        qDebug()<<"Host not active";
        return;
    }
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
    if (data.left(strlen("client"))=="client") {

    }
    if (data=="resync") {

    }
    /*for (int i=0; i<threadlist.size(); ++i) {
        if (threadlist.at(i)->socketDescriptor==ID) {
            threadlist.at(i)->sendData("");
        }
    }*/
}

void MusicServer::disconnectConnection(int ID)
{
    qDebug() << "Disconnect from Server: " << ID << " threadlist Size: " << threadlist.size();
    for (int i=0; i<threadlist.size(); ++i) {
        if (threadlist.at(i)->socketDescriptor==ID) {
            //threadlist.at(i)->deleteLater();
            threadlist.at(i)->exit();
            threadlist.removeAt(i);
            qDebug()<<"removed: "<<i;
        }
    }
    dumpDebugInfo();
}


void MusicServer::incomingConnection(int socketDescriptor)
{
    qDebug() << "Connecting..." << socketDescriptor;
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    threadlist.append(thread);
    qDebug()<<threadlist.size();
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(disconnect(int)), this, SLOT(disconnectConnection(int)));
    connect(thread, SIGNAL(dataReady(int,QByteArray)), this, SLOT(gotData(int, QByteArray)));
    connect(this, SIGNAL(sendSignal(QString)), thread, SLOT(sendData(QString)));
    thread->start();
}
