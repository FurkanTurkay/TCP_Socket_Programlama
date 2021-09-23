#include "mainwindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_server = new QTcpServer();

    if(m_server->listen(QHostAddress::Any, 8080))
    {
       connect(this, &MainWindow::newMessage, this, &MainWindow::replyMessage);
       connect(m_server, &QTcpServer::newConnection, this, &MainWindow::newConnection);
       ui->statusBar->showMessage("Sunucu çalışıyor...");
    }
    else
    {
        QMessageBox::critical(this,"QTCPServer",QString("Sunucu başlatılamıyor: %1.").arg(m_server->errorString()));
        exit(EXIT_FAILURE);
    }

    clientNumber=0;
}

MainWindow::~MainWindow()
{
    foreach (QTcpSocket* socket, connection_set)
    {
        socket->close();
        socket->deleteLater();
    }

    m_server->close();
    m_server->deleteLater();

    delete ui;
}

void MainWindow::newConnection()
{
    while (m_server->hasPendingConnections())
        appendToSocketList(m_server->nextPendingConnection());

}

void MainWindow::appendToSocketList(QTcpSocket* socket)
{
    connection_set.insert(socket);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);
    receiverClientList<<QString::number(socket->socketDescriptor());
    label=label+"\n"+QString::number(socket->socketDescriptor());
    ui->label_receiverClientList->setText(label);
    replyMessage(QString("INFO :: Client: %1 bağlandı ! ").arg(socket->socketDescriptor()));
    clientNumber++;
    ui->label_clientNumber->setText(QString::number(clientNumber));
}

void MainWindow::readSocket()
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

void MainWindow::discardSocket()
{
    QTcpSocket* socket = reinterpret_cast<QTcpSocket*>(sender());
    QSet<QTcpSocket*>::iterator it = connection_set.find(socket);
    qDebug()<<socket->socketDescriptor()<<"  Client ayrıldı";
    if (it != connection_set.end()){
        replyMessage(QString("INFO :: Bir client ayrıldı. %1").arg(socket->socketDescriptor()));
        connection_set.remove(*it);
    }
    clientNumber--;
    refreshLabel();
    deleteSocketID();

    socket->deleteLater();
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPServer", "Host bulunamadı. Lütfen host ve bağlantı noktası ayarlarını kontrol edin.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPServer", "Bağlantı  reddedildi. QTCPServer'ın çalıştığından emin olun. Host ve bağlantı noktası ayarlarının doğru olduğunu kontrol edin.");
        break;
        default:
            QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
            QMessageBox::information(this, "QTCPServer", QString("Bir hata oluştu: %1.").arg(socket->errorString()));
        break;
    }
}

void MainWindow::sendMessage(QTcpSocket* socket)
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
            QMessageBox::critical(this,"QTCPServer","Socket kapalı.");
    }
    else
        QMessageBox::critical(this,"QTCPServer","Client, sunucuya bağlı değil.");
}

void MainWindow::moneyTransferMessage(int receiverID)
{
    qDebug()<<socketList;
    receiverDescriptor="-1";
    for (int i=0;i<socketList.count();i++) {
        if(socketList[i].contains(" :: "))
        {
            if(socketList[i].split(" :: ")[1]==QString::number(receiverID))
            {

                receiverDescriptor=socketList[i].split(" :: ")[0];
                qDebug()<<" receiverDescriptor=socketList["<<i<<"] -> "<<receiverDescriptor;
            }

        }

    }
    qDebug()<<" receiverDescriptor="<<receiverDescriptor;



    if(receiverDescriptor!=""){
        d.findCustomer(receiverID);
        strPost="moneyTransferHasArrived,"+         //0
                QString::number(d.customerID)+","+  //1
                d.name+","+                         //2
                d.bank+","+                         //3
                QString::number(d.balance);         //4

        foreach (QTcpSocket* socket,connection_set)
        {
            if(socket->socketDescriptor() == receiverDescriptor.toLongLong())
            {
                sendMessage(socket);
                break;
            }

        }
    }

}


void MainWindow::replyMessage(const QString& str)
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
                if(!sameUserError(ID))
                {
                    strPost="UserLogin,"+QString::number(d.customerID)+","+d.name+","+d.bank+","+QString::number(d.balance);
    //                strPost="UserLogin,\nMusteri No:"+QString::number(d.customerID)+
    //                        "\nisim:"+d.name+
    //                        "\nBanka:"+d.bank+
    //                        "\nBakiye:"+QString::number(d.balance);
                    AutoSentMessage();

                    matchIDwithSocket(ID,true);
                }
                else
                {
                    strPost="UserLogin,sameUserError";

                    AutoSentMessage();
                }


            }
            else
            {
                strPost="UserLogin,incorrect username or password";
                AutoSentMessage();
            }

        }
        else if(strList[0]=="userLogout")
        {
            matchIDwithSocket(strList[1].toInt(),false);    //delete the pairing

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

                                moneyTransferMessage(strList[2].toInt());


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


void MainWindow::refreshLabel(){

    label="";
    receiverClientList.clear();
    ui->label_receiverClientList->setText(label);
    ui->label_clientNumber->setText(QString::number(clientNumber));
    foreach(QTcpSocket* socket, connection_set)
    {
        receiverClientList<<QString::number(socket->socketDescriptor());
        label=label+"\n"+QString::number(socket->socketDescriptor());
        ui->label_receiverClientList->setText(label);
        ui->label_clientNumber->setText(QString::number(clientNumber));

    }


}

void MainWindow::AutoSentMessage()
{

//        foreach (QTcpSocket* socket,connection_set)
//        {
//            sendMessage(socket);
//        }

    //
    //multi-client connection bug solved.
    foreach (QTcpSocket* socket,connection_set)
    {
        if(socket->socketDescriptor() == strClientPort[0].toLongLong())
        {
            sendMessage(socket);
            break;
        }

    }

}

void MainWindow::matchIDwithSocket(int clientID,bool operation)
{
//
//
    strSocket= strClientPort[0]+" :: "+QString::number(clientID);

    if(operation) //match ID with Socket Number
    {
        socketList<<strSocket;
    }
    else
        socketList.removeOne(strSocket);//remove client

//    qDebug()<<socketList;
//    qDebug()<<socketList.count();

    //to refresh label
    text_clientNumberwithID="";
    for (int i=0;i<socketList.count();i++ ) {
        text_clientNumberwithID=text_clientNumberwithID+socketList[i]+"\n";
    }
    ui->label_client_ID->setText(text_clientNumberwithID);
    //**********

    qDebug()<<"ın matchIDwithSocket function: "<<socketList;

}

void MainWindow::deleteSocketID()   //To report the server if the user close the client program
{
//    socketListStr="";
//    for (int i = 0; i<socketList.count();i++)
//    {
//    socketListStr=socketListStr+socketList[i]+",";
//    }


//    if(socketListStr.contains(" "))

    m_socketList.clear();
    for (int i=0;i<socketList.count();i++)
    {
        if(socketList[i].contains(" "))
            m_socketList<<socketList[i].split(' ')[0];
    }
    qDebug()<<"before remove receviverClientList: "<<m_socketList;
    qDebug()<<"in delete socketID recevier clients list:"<<receiverClientList;


    for (int i=0;i<receiverClientList.count();i++) {
        m_socketList.removeOne(receiverClientList[i]);
    }

    qDebug()<<"m_socketList: "<<m_socketList;
    if(!m_socketList.isEmpty())
        socketNumberLeavingServer=m_socketList[0];
    qDebug()<<" socketNumberLeavingServer:" <<socketNumberLeavingServer;


    for (int i=0;i<socketList.count();i++ ) {
        if(socketList[i].contains(socketNumberLeavingServer+" :: "))
        socketList.removeAt(i);
    }

    //to refresh label
    text_clientNumberwithID="";
    for (int i=0;i<socketList.count();i++ ) {
        text_clientNumberwithID=text_clientNumberwithID+socketList[i]+"\n";
    }
    ui->label_client_ID->setText(text_clientNumberwithID);
    //**********

//    for (int i=0;i<socketList.count();i++ ) {
//        text_clientNumberwithID=text_clientNumberwithID+socketList[i]+"\n";
//        socketListStr=socketListStr+socketList[i]+",";
//    }

    ui->label_client_ID->setText(text_clientNumberwithID);

}


bool MainWindow::sameUserError(int clientID)//If it returns true, login is not allowed
{

    //qDebug()<<socketListStr;
    qDebug()<<"sameUserError: socket list "<<socketList;
    bool returnValue =false;
    for (int i=0;i<socketList.count();i++ ) {
        if(socketList[i].contains(" :: "+QString::number(clientID))){
            qDebug()<<"sameUserError in for: "<<i<<socketList[i];
            returnValue = true;

        }
    }
    return returnValue;

}


