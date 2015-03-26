#include "musicserver.h"
#include "musichost.h"

MusicServer::MusicServer(QObject *parent) :
    QTcpServer(parent)
{
    host = dynamic_cast<MusicHost*>(parent);
}

void MusicServer::startServer()
{
    if(!this->listen(QHostAddress::Any,7777))
    {
        host->log("Server","Could not start Server",MusicHost::MsgType::ERROR_LOG);
        //qDebug()<<"Could not start Server";
    }
    else
    {
        host->log("Server","Listening...",MusicHost::MsgType::INFO_LOG);
        //qDebug()<<"Listening...";
    }
}

void MusicServer::dumpDebugInfo() {
    host->log("Server","Thread list size: " + threadlist.size(), MusicHost::MsgType::INFO_LOG);
    //qDebug()<<"Info: "<<threadlist.size();
    for (int i=0; i<threadlist.size(); i++)
    {
        host->log("Server","Socket descriptor: " + threadlist.at(i)->socketDescriptor, MusicHost::MsgType::INFO_LOG);
        //qDebug()<<threadlist.at(i)->socketDescriptor;//<<" "<<threadlist.at(i)->isRunning();//<<" "<<threadlist.at(i)->isFinished();
    }
}

void MusicServer::sendPlaylist(QList<QString> songNames)
{
    host->log("Server","Send List... threadlistSize: " + threadlist.size(), MusicHost::MsgType::INFO_LOG);
    //qDebug()<<"Send List... threadlistSize:"<<threadlist.size();
    QString sendList="list";
    for (int i=0; i<songNames.size(); ++i) {
        sendList.append(songNames.at(i));
    }
    for (int i=0; i<threadlist.size(); i++)
    {
        host->log("Server","Send List to " + threadlist.at(i)->socketDescriptor, MusicHost::MsgType::INFO_LOG);
        //qDebug()<<"Send List to"<<threadlist.at(i)->socketDescriptor;
        emit sendSignal(sendList);
        //threadlist.at(i)->sendData(sendList);

    }
    host->log("Server","List sent", MusicHost::MsgType::SUCCESS_LOG);
    //qDebug()<<"List sent!";
}

void MusicServer::sendMessage(QString message)
{
    host->log("Server","Sending Message...", MusicHost::MsgType::INFO_LOG);
    //qDebug()<<"Sending Message...";
    for (int i=0; i<threadlist.size(); i++)
    {
        emit sendSignal("mesg"+message);
        //threadlist.at(i)->sendData("mesg"+message);
    }
}

void MusicServer::sendSong(QString songName)
{
    host->log("Server","Sending Song...", MusicHost::MsgType::INFO_LOG);
    //qDebug()<<"Send Song...";
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
    host->log("Server", "ID and Data: " + QString::number(ID) + " " + data, MusicHost::MsgType::INFO_LOG);
    //qDebug() << ID << " " << data;
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
    host->log("Server", "Disconnect from Server: " + QString::number(ID) + " threadlist Size: " + QString::number(threadlist.size()), MusicHost::MsgType::INFO_LOG);
    //qDebug() << "Disconnect from Server: " << ID << " threadlist Size: " << threadlist.size();
    for (int i=0; i<threadlist.size(); ++i) {
        if (threadlist.at(i)->socketDescriptor==ID) {
            //threadlist.at(i)->deleteLater();
            threadlist.removeAt(i);
            host->log("Server", "removed: " + i, MusicHost::MsgType::INFO_LOG);
            //qDebug()<<"removed: "<<i;
        }
    }
    dumpDebugInfo();
}


void MusicServer::incomingConnection(int socketDescriptor)
{
    host->log("Server", "Connecting..." + socketDescriptor, MusicHost::MsgType::INFO_LOG);
    //qDebug() << "Connecting..." << socketDescriptor;
    ServerThread *thread = new ServerThread(socketDescriptor, this);
    threadlist.append(thread);
    host->log("Server", "Thread list size: " + threadlist.size(), MusicHost::MsgType::INFO_LOG);
    //qDebug()<<threadlist.size();
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(thread, SIGNAL(disconnect(int)), this, SLOT(disconnectConnection(int)));
    connect(thread, SIGNAL(dataReady(int,QByteArray)), this, SLOT(gotData(int, QByteArray)));
    connect(this, SIGNAL(sendSignal(QString)), thread, SLOT(sendData(QString)));
    thread->start();
}
