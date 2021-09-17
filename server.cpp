#include "server.h"

server::server(QObject *parent) : QObject(parent)
{





    tcpServer = new QTcpServer(this);
    //bind + listen
    tcpServer->listen(QHostAddress::Any, 4321); //listen all network interfaces, and listen port 8888

    connect(tcpServer, &QTcpServer::newConnection, [=]
    {
        tcpSocket = tcpServer->nextPendingConnection(); //wait for the next pending connection

        //get the information of the current socket, ip address, port number
        QString ip = tcpSocket->peerAddress().toString();
        quint16 portNum = tcpSocket->peerPort();

        QString str = QString("%1:%2 Connected successfully!").arg(ip).arg(portNum);
        qDebug()<<str;

        connect(tcpSocket, &QTcpSocket::readyRead, [=]
        {
            //get the text from the text edit write box
            getStr = tcpSocket->readAll();

            //append the new str to the current read box
            qDebug()<<getStr;
        });
    });
}



void server::send2client()
{
    if(!tcpSocket) return;
    //get the current text in the text box
    tcpSocket =tcpServer->nextPendingConnection();

    tcpSocket->write(postStr.toStdString().data());
    tcpSocket->flush();

    //then write it to the socket stack

}

void server::close()
{
    if(!tcpSocket) return;
    //the tcp socket will disconected from the server
    tcpSocket->disconnectFromHost();
    //close the current tcp socket
    tcpSocket->close();
    tcpSocket = NULL;
}
