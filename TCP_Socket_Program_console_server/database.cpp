#include "database.h"

database::database()
{

}

void database::userLogin(QString username, QString password)
{
    db=QSqlDatabase::addDatabase("QMYSQL", "MyConnect");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1234");
    db.setDatabaseName("banka");
    message="";

    if(db.open()){
        qDebug()<<"Connection Login";

        QSqlQuery query(QSqlDatabase::database("MyConnect"));
        query.prepare(QString("SELECT * FROM customers WHERE username = :username AND password = :password"));
        query.bindValue(":username",username);
        query.bindValue(":password",password);

        if(!query.exec()){
            qDebug()<<"Failed";
        }
        else
        {
            while(query.next()){
                customerID =query.value(0).toInt();
                userName =query.value(1).toString();
                pass =query.value(2).toString();
                name =query.value(3).toString();
                bank =query.value(4).toString();
                balance =query.value(5).toDouble();

                if(username == userName && password == pass){
                    //qDebug()<<"Login Success";
                    message="true";

                }
                else
                {
//                    qDebug()<<"Login Failed";
                     message="false";
                }
            }
        }
    }
    else
        qDebug()<<"Not Connected";

    db.close();
}

void database::findCustomer(int ID)
{
    db=QSqlDatabase::addDatabase("QMYSQL","findCustomer");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1234");
    db.setDatabaseName("banka");

    if(db.open()){
        qDebug()<<"Connection";

        QSqlQuery query(QSqlDatabase::database("findCustomer"));
        query.prepare(QString("SELECT * FROM customers WHERE CustomerID = :ID"));
        query.bindValue(":ID", ID);


        if(!query.exec()){
            qDebug()<<"Musteri bulunamadı";

        }

        else
        {
            while(query.next()){
                customerID =query.value(0).toInt();
                userName =query.value(1).toString();
                pass =query.value(2).toString();
                name =query.value(3).toString();
                bank =query.value(4).toString();
                balance =query.value(5).toDouble();
           }


        }


    }
    else
        qDebug()<<"Not Connected";

    db.close();
}

void database::updateDB(int ID, double balance)
{
    db=QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("127.0.0.1");
    db.setUserName("root");
    db.setPassword("1234");
    db.setDatabaseName("banka");
    if(db.open()){
        qDebug()<<"Connection Update";
        QSqlQuery query;
        query.prepare("UPDATE customers SET balance=:balance WHERE CustomerID=:ID");
        query.bindValue(":ID", ID);
        query.bindValue(":balance",balance);
        if(query.exec()){
            qDebug()<<"updated";
        }
        else
            qDebug()<<"Not updated";
    }
    else
        qDebug()<<"Not Connected";
    db.close();

}

bool database::withdrawMoney(int ID, double availableBalance, double amount)
{
    if(availableBalance>amount || availableBalance==amount){
        balance=availableBalance-amount;
        updateDB(ID,balance);
        return true;

    }
    else
        return false;

}

void database::depositMoney(int ID, double availableBalance, double amount)
{
    balance=availableBalance+amount;
    updateDB(ID,balance);
}

bool database::transferMoney(int forwarder, int recipient, double amount)
{
    findCustomer(forwarder);
    m_bank=bank;
    m_balance=balance;

    if(m_balance>amount || m_balance==amount){

        m_balance=m_balance-amount;
        findCustomer(recipient);

        if(m_bank!=bank){
            m_balance=m_balance-processingFee;
            if(m_balance<0)
                return false;
            info=QString::number(processingFee)+" TL İŞLEM ÜCRETİ TAHSİL EDİLMİŞTİR.";      //TL iSLEM UCRETi TAHSiL EDiLMiSTiR.

        }
        else
            info="BANKANIZ BU İŞLEM İÇİN SİZDEN ÜCRET TAHSİL ETMEMİŞTİR.";                  //BANKANIZ BU iSLEM iCiN SiZDEN UCRET TAHSiL ETMEMiSTiR.
        balance=balance+amount;
        updateDB(forwarder,m_balance);
        updateDB(recipient,balance);
        return true;

    }
    else
        return false;

}
