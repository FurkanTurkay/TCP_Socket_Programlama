#include "server.h"

server::server(QObject *parent) : QObject(parent)
{
    m_server = new QTcpServer();

    if(m_server->listen(QHostAddress::Any, 8080))
    {
       connect(this, &server::newMessage, this, &server::replyMessage);
       connect(m_server, &QTcpServer::newConnection, this, &server::newConnection);
       qDebug()<<"Sunucu çalışıyor...";
    }
    else
    {
        qDebug()<<QString("Sunucu başlatılamıyor: %1.").arg(m_server->errorString());
        exit(EXIT_FAILURE);
    }
}

server::~server()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    m_server->close();
    m_server->deleteLater();
}

void server::newConnection()
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());
}

void server::appendToSocketList(QTcpSocket* socket)
{
    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &server::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &server::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &server::displayError);
    infoText=infoText+"\n"+QString::number(socket->socketDescriptor());

    replyMessage(QString("INFO :: Client: %1 bağlandı ! ").arg(socket->socketDescriptor()));
}

void server::readSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());

    QByteArray buffer;

    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    socketStream.startTransaction();
    socketStream >> buffer;

    if(!socketStream.commitTransaction())
    {
        QString message = QString("%1 :: Veri gelmesi bekleniyor..").arg(socket->socketDescriptor());
        emit newMessage(message);
        return;
    }


    buffer = buffer.mid(128);

    QString message = QString("%1;%2").arg(socket->socketDescriptor()).arg(QString::fromStdString(buffer.toStdString()));
    emit newMessage(message);

}

void server::discardSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    if (it != connection_set.end()){
        replyMessage(QString("INFO :: Bir client ayrıldı.").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    infoDebug();


    socket->deleteLater();
}

void server::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            qDebug()<< "Host bulunamadı. Lütfen host ve bağlantı noktası ayarlarını kontrol edin.";
        break;
        case QAbstractSocket::ConnectionRefusedError:
            qDebug()<< "Bağlantı  reddedildi. QTCPServer'ın çalıştığından emin olun. Host ve bağlantı noktası ayarlarının doğru olduğunu kontrol edin.";
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            qDebug()<< QString("Bir hata oluştu: %1.").arg(socket->errorString());
        break;
    }
}

void server::sendMessage(QTcpSocket* socket)
{
    if(socket)
    {
        if(socket->isOpen())
        {
            QString str = strPost;

            QDataStream socketStream(socket);
            socketStream.setVersion(QDataStream::Qt_5_15);

            QByteArray header;
            header.prepend(QString("fileType:message,fileName:null,fileSize:%1;").arg(str.size()).toUtf8());
            header.resize(128);

            QByteArray byteArray = str.toUtf8();
            byteArray.prepend(header);

            socketStream.setVersion(QDataStream::Qt_5_15);
            socketStream << byteArray;
        }
        else
            qDebug()<<"Socket kapalı.";
    }
    else
        qDebug()<<"Client, sunucuya bağlı değil.";
}

void server::replyMessage(const QString& str)
{

    qDebug()<<str;
    if(str.contains(';'))
    {
//        strPost="str.contains(';')";
//        AutoSentMessage();
        strClientPort=str.split(';');
        qDebug()<<strClientPort;
        // example output "1372;UserLogin,furkan,1234"

        strList= strClientPort[1].split(',');
        qDebug()<<strList;
        for(int i=0;i<strList.count();i++)
        {
            qDebug()<<strList[i];
        }


        if(strList[0] == "UserLogin"){

            d.userLogin(strList[1],strList[2]);
            if(d.message=="true"){
                ID=d.customerID;
                strPost="UserLogin,"+QString::number(d.customerID)+","+d.name+","+d.bank+","+QString::number(d.balance);
//                strPost="UserLogin,\nMusteri No:"+QString::number(d.customerID)+
//                        "\nisim:"+d.name+
//                        "\nBanka:"+d.bank+
//                        "\nBakiye:"+QString::number(d.balance);
                AutoSentMessage();
            }
            else
            {
                strPost="UserLogin,incorrect username or password";
                AutoSentMessage();
            }

        }

        else if(strList[0]=="depositMoney")
        {
           d.depositMoney(strList[1].toInt(),strList[2].toDouble(),strList[3].toDouble());
           strPost="depositMoney,success,"+QString::number(d.balance);
           AutoSentMessage();

        }

        else if(strList[0]=="withdrawMoney")
        {
           if(d.withdrawMoney(strList[1].toInt(),strList[2].toDouble(),strList[3].toDouble()))
           {
               strPost="withdrawMoney,success,"+QString::number(d.balance);
               AutoSentMessage();

           }
           else
           {
               strPost="withdrawMoney,insufficientBalance,"+QString::number(d.balance);
               AutoSentMessage();

           }
        }

        else if(strList[0]=="transferMoney")
        {
            d.name="";
            d.findCustomer(strList[2].toInt());
            if(strList[1]!=strList[2])
            {
                if(d.name!=""){
                        if(d.name==strList[5])// check receiver name
                        {
                            if(d.transferMoney(strList[1].toInt(),strList[2].toInt(),strList[4].toDouble()))
                            {
                                d.findCustomer(strList[1].toInt());
                                strPost="transferMoney,success,"+d.info+","+QString::number(d.balance);
                                AutoSentMessage();
                            }

                            else
                            {
                                d.findCustomer(strList[1].toInt());
                                strPost="transferMoney,insufficientBalance,"+ QString::number(d.balance);
                                AutoSentMessage();
                            }
                        }

                        else
                        {   //receiverNameIsIncorrect
                            strPost="transferMoney,receiverNameIsIncorrect";
                            AutoSentMessage();
                        }

                }
                else
                {
                    strPost="transferMoney,receiverIsNotFound";
                    AutoSentMessage();
                }
            }
            else
            {
                strPost="transferMoney,invalidReceiver";
                AutoSentMessage();

            }


        }
        else if(strList[0]=="customerIDCheck")
        {
            d.name="";
            d.findCustomer(strList[1].toInt());

           if(d.name!="")
           {
               strPost="customerIDCheck,"+d.name;
               AutoSentMessage();

           }
           else
           {
               strPost="customerIDCheck,-";
               AutoSentMessage();
           }
        }


        else
        {
            strPost="error";
            AutoSentMessage();

        }
    }

}

void server::infoDebug(){

    infoText="BroadCast";

    foreach(QTcpSocket* socket, connection_set)
    {
        infoText=infoText+"\n"+QString::number(socket->socketDescriptor());
        qDebug()<<infoText;
    }

}
void server::AutoSentMessage()
{

    foreach (QTcpSocket* socket,connection_set)
    {
        if(socket->socketDescriptor() == strClientPort[0].toLongLong())
        {
            sendMessage(socket);
            break;
        }
    }

}
