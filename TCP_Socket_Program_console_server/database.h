#ifndef DATABASE_H
#define DATABASE_H

#include<QtSql>
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>

#include <QDebug>

class database
{
public:
    database();
    void userLogin(QString username, QString password);
    void findCustomer(int ID);
    void updateDB(int ID,double balance);

    bool withdrawMoney(int ID, double availableBalance,double amount);
    void depositMoney(int ID, double availableBalance,double amount);
    bool transferMoney(int forwarder,int recipient, double amount);


    QString message;

    int customerID;
    QString userName;
    QString pass;
    QString name;
    QString m_bank;


    QString bank;

    QString info;

    double processingFee=5;
    double balance;
    double m_balance;

    double recipientBalance;

private:
    QSqlDatabase db;
};

#endif // DATABASE_H
