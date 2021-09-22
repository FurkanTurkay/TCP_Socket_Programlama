#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->tabWidget->setTabVisible(1,false);
    ui->tabWidget->setTabVisible(2,false);
    ui->tabWidget->setTabVisible(3,false);
    ui->tabWidget->setTabVisible(4,false);

    ui->lineEdit_transferMoney_customerID->setValidator(new QIntValidator());
    ui->lineEdit_withdrawMoney_amount->setValidator(new QIntValidator());
    ui->lineEdit_depositMoney_amount->setValidator(new QIntValidator());
    ui->lineEdit_transferMoney_amount->setValidator(new QIntValidator());

    ui->label_loginInfo->setText("Hoşgeldiniz. Lütfen giriş yapınız.");



    socket = new QTcpSocket(this);

    connect(this, &MainWindow::newMessage, this, &MainWindow::replyMessage);
    connect(socket, &QTcpSocket::readyRead, this, &MainWindow::readSocket);
    connect(socket, &QTcpSocket::disconnected, this, &MainWindow::discardSocket);
    connect(socket, &QAbstractSocket::errorOccurred, this, &MainWindow::displayError);

    socket->connectToHost(QHostAddress::LocalHost,8080);

    if(socket->waitForConnected())
        ui->statusBar->showMessage("Sunucuya bağlandı.");
    else{
        QMessageBox::critical(this,"QTCPClient", QString("Bir hata oluştu %1.").arg(socket->errorString()));
        exit(EXIT_FAILURE);
    }
}

MainWindow::~MainWindow()
{
    if(socket->isOpen())
        socket->close();
    delete ui;
}

void MainWindow::readSocket()
{
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
    socket->deleteLater();
    socket=nullptr;

    ui->statusBar->showMessage("Bağlantı Kesildi!");
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    switch (socketError) {
        case QAbstractSocket::RemoteHostClosedError:
        break;
        case QAbstractSocket::HostNotFoundError:
            QMessageBox::information(this, "QTCPClient", "Host bulunamadı. Lütfen host adını ve bağlantı noktası ayarlarını kontrol edin.");
        break;
        case QAbstractSocket::ConnectionRefusedError:
            QMessageBox::information(this, "QTCPClient", "Bağlantı reddedildi. QTCPServer'ın çalıştığından emin olun. Host adını ve bağlantı noktası ayarlarını kontrol edin. ");
        break;
        default:
            QMessageBox::information(this, "QTCPClient", QString("Bir hata oluştu: %1.").arg(socket->errorString()));
        break;
    }
}

void MainWindow::replyMessage(const QString& str)
{


    qDebug()<<str;
    if(str.contains(';'))
    {
        strServerPort=str.split(';');
        strList= strServerPort[1].split(',');
        if(strList[0]=="UserLogin")
        {
            if(strList[1]!="incorrect username or password")
            {
                ID=strList[1].toInt();
                name=strList[2];
                bank=strList[3];
                balance=strList[4].toDouble();

                ui->label_CustomerData_ID->setText(QString::number(ID));
                ui->label_CustomerData_name->setText(name);
                ui->label_CustomerData_nameOfTheBank->setText(bank);
                ui->label_CustomerData_balance->setText(QString::number(balance)+"₺");

                ui->tabWidget->setTabVisible(0,false);
                ui->tabWidget->setTabVisible(1,true);


            }
            else
            {
                QMessageBox::warning(this,"Müşteri Girişi","Kullanıcı adı veye şifre yanlış.");
            }
        }
        else if(strList[0]=="depositMoney")
        {
            if(strList[1]=="success")
            {
                balance=strList[2].toDouble();
                ui->label_CustomerData_balance->setText(QString::number(balance)+"₺");

                QMessageBox::information(this,"Para Yatırma","Para Yatırma başarılı.\nMevcut Bakiye: " +strList[2]+"₺");

                ui->tabWidget->setTabVisible(2,false);//depositMoney
                ui->tabWidget->setTabVisible(1,true);//menu
            }
        }
        else if(strList[0]=="withdrawMoney")
        {
            if(strList[1]=="success")
            {
                balance=strList[2].toDouble();
                ui->label_CustomerData_balance->setText(QString::number(balance)+"₺");

                QMessageBox::information(this,"Para Çekme","Para Çekildi.\nMevcut Bakiye: " +strList[2]+"₺");

                ui->tabWidget->setTabVisible(3,false);//withdrawMoney
                ui->tabWidget->setTabVisible(1,true);//menu

            }
            else
            {
                QMessageBox::information(this,"Para Çekme","Yetersiz Bakiye!\nMevcut Bakiye: " +strList[2]+"₺");

            }
        }
        else if(strList[0]=="transferMoney")
        {
            if(strList[1]=="success")
            {
                balance=strList[3].toDouble();
                ui->label_CustomerData_balance->setText(QString::number(balance)+"₺");

                QMessageBox::information(this,"Para Transferi","Para Transferi Gerçekleşti. \n"+strList[2]+"\nMevcut Bakiye: " +strList[3]+"₺");

                ui->tabWidget->setTabVisible(4,false);//transferMoney
                ui->tabWidget->setTabVisible(1,true);//menu

                ui->lineEdit_transferMoney_amount->clear();
                ui->lineEdit_transferMoney_customerID->clear();
                ui->lineEdit_transferMoney_customerName->clear();
            }
            else if(strList[1]=="receiverIsNotFound"){

                QMessageBox::warning(this,"Para Transferi","Alıcı Bulunamadı!\nLütfen alıcı müşteri numarasını kontrol ediniz. ");

            }
            else if(strList[1]=="receiverNameIsIncorrect"){

                QMessageBox::warning(this,"Para Transferi","Alıcı Adı Hatalı!\nLütfen alıcı adını kontrol ediniz. ");

            }
            else if(strList[1]=="invalidReceiver")
            {

                QMessageBox::warning(this,"Para Transferi","Geçersiz alıcı!\n Lütfen alıcı müşteri numarasını kontrol ediniz. ");

            }
            else
            {
                QMessageBox::warning(this,"Para Transferi","Yetersiz Bakiye! \nMevcut Bakiye: " +strList[2]+"₺");

            }
        }
        else if(strList[0]=="customerIDCheck")
        {
            if(strList[1]!='-')
            {
                recevier_name= strList[1].split(' ')[0];
                recevier_lastName=strList[1].split(' ')[1];
                ui->label_nameHint->setText("İpucu: "+recevier_name[0]+recevier_name[1]+"*** "+recevier_lastName[0]+recevier_lastName[1]+"***");

            }
            else
            {
                ui->label_nameHint->setText("Alıcı Bulunamadı");

            }

        }


    }

}

void MainWindow::sendStr()
{
    QDataStream socketStream(socket);
    socketStream.setVersion(QDataStream::Qt_5_15);

    QByteArray header;
    header.prepend(QString("fileType:message,fileName:null,fileSize:%1;").arg(str.size()).toUtf8());
    header.resize(128);

    QByteArray byteArray = str.toUtf8();
    byteArray.prepend(header);

    socketStream << byteArray;

}

void MainWindow::userLogin()
{
    if(socket)
    {
        if(socket->isOpen())
        {
            str = "UserLogin,"+ui->lineEdit_UserLogin_userName->text() + ","+ ui->lineEdit_UserLogin_pass->text();
            sendStr();

            ui->lineEdit_UserLogin_pass->clear();

        }
        else
            QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");

}



void MainWindow::on_pushButton_depositMoney_clicked()
{

    ui->tabWidget->setTabVisible(1,false);//menu
    ui->tabWidget->setTabVisible(2,true);//depositMoney

    ui->label_depositMoney_CustomerData->setText("Kullanılabilir Bakiye: "+QString::number(balance)+"₺");
}


void MainWindow::on_pushButton_withdrawMoney_clicked()
{

    ui->tabWidget->setTabVisible(1,false);//menu
    ui->tabWidget->setTabVisible(3,true);//withdrawMoney
    ui->label_withdrawMoney_CustomerData->setText("Kullanılabilir Bakiye: "+QString::number(balance)+"₺");

}


void MainWindow::on_pushButton_transferMoney_clicked()
{
    ui->tabWidget->setTabVisible(1,false);//menu
    ui->tabWidget->setTabVisible(4,true);//transferMoney
    ui->label_transferMoney_CustomerData->setText("Kullanılabilir Bakiye: "+QString::number(balance)+"₺");

}


void MainWindow::on_pushButton_exit_clicked()
{


    ui->tabWidget->setTabVisible(1,false);//menu
    ui->lineEdit_UserLogin_userName->clear();
    ui->tabWidget->setTabVisible(0,true);//User Login
    QMessageBox::information(this,"Oturum Kapatıldı","Sn. "+name+ " Oturumunuz sonlandırıldı.");

    ID=-1;
    bank="";
    name="";
    balance=-1;
}


void MainWindow::on_pushButton_UserLogin_login_clicked()
{
    userLogin();
}
void MainWindow::on_lineEdit_UserLogin_pass_returnPressed()
{
    userLogin();
}


void MainWindow::on_pushButton_depositMoney_cancel_clicked()
{
    ui->tabWidget->setTabVisible(2,false);//depositMoney
    ui->tabWidget->setTabVisible(1,true);//menu
    ui->lineEdit_depositMoney_amount->clear();

}


void MainWindow::on_pushButton_depositMoney_okay_clicked()
{
    if(socket)
    {
        if(socket->isOpen())
        {
            str = "depositMoney,"+QString::number(ID)+","+ QString::number(balance)+","+ui->lineEdit_depositMoney_amount->text();

            sendStr();

            ui->lineEdit_depositMoney_amount->clear();

        }
        else
            QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");

}


void MainWindow::on_pushButton_withdrawMoney_cancel_clicked()
{

    ui->tabWidget->setTabVisible(3,false);//withdrawMoney
    ui->tabWidget->setTabVisible(1,true);//menu
    ui->lineEdit_withdrawMoney_amount->clear();

}


void MainWindow::on_pushButton_withdrawMoney_okay_clicked()
{
    if(socket)
    {
        if(socket->isOpen())
        {
            str =  "withdrawMoney,"+
                    QString::number(ID)+","+
                    QString::number(balance)+","+
                    ui->lineEdit_withdrawMoney_amount->text();

            sendStr();

            ui->lineEdit_withdrawMoney_amount->clear();

        }
        else
            QMessageBox::critical(this,"QTCPClient","");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");

}


void MainWindow::on_pushButton_transferMoney_cancel_clicked()
{

    ui->tabWidget->setTabVisible(4,false);//transferMoney
    ui->tabWidget->setTabVisible(1,true);//menu
    ui->lineEdit_transferMoney_amount->clear();
    ui->lineEdit_transferMoney_customerID->clear();
    ui->lineEdit_transferMoney_customerName->clear();


}


void MainWindow::on_pushButton_transferMoney_okay_clicked()
{
    if(socket)
    {
        if(socket->isOpen())
        {                   //0
            str = "transferMoney,"+QString::number(ID)+","+             //1
                    ui->lineEdit_transferMoney_customerID->text()+","+  //2
                    QString::number(balance)+","+                       //3
                    ui->lineEdit_transferMoney_amount->text()+","+      //4
                    ui->lineEdit_transferMoney_customerName->text();    //5

            sendStr();

        }
        else
            QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
    }
    else
        QMessageBox::critical(this,"QTCPClient","Not connected");

}








void MainWindow::on_lineEdit_transferMoney_customerID_textChanged(const QString &arg1)
{
   //A bug occurred due to the socket delay.
    if (arg1.length()>0 && arg1.length()<2)//temporary solution
    {
        if(socket)
        {
            if(socket->isOpen())
            {
                str = "customerIDCheck,"+ //0
                              arg1;       //1

                sendStr();
            }
            else
                QMessageBox::critical(this,"QTCPClient","Socket doesn't seem to be opened");
        }
        else
            QMessageBox::critical(this,"QTCPClient","Not connected");

    }
    else
        ui->label_nameHint->setText("");
}



