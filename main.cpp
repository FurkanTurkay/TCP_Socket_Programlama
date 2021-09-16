#include <QCoreApplication>
#include"database.h"

#include <iostream>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    database d;




    std::string username;
    std::string pass;
    std::string c;

    int customerID;
    int ID;
    std::string customerName;

    double amount;
    while(1){
    std::cout<<"\n**************\nHOSGELDiNiZ \n**************\n"
               "\tKulanici Adi: ";
    std::cin >> username;
    std::cout<<"\t       Sifre: ";
    std::cin >> pass;


    d.userLogin(QString::fromStdString(username),QString::fromStdString(pass));
    if(d.message=="true"){
        ID=d.customerID;
        qDebug()<<"\tMusteri No:\t"<<d.customerID;
        qDebug()<<"\tisim:\t\t"<<d.name;
        qDebug()<<"\tBanka:\t\t"<<d.bank;
        qDebug()<<"\tBakiye:\t\t"<<d.balance;
        while(1){
            std::cout<<"\n\t (1) Para Yatir\n\t (2) Para Cek\n\t (3) Para Transferi\n\n\t (0) CIKIS YAP\n\t";
            std::cin >> c;
            if(c=="1")
            {
                std::cout<<"Yatirmak istediginiz tutari giriniz: ";
                std::cin >> amount;
                d.depositMoney(ID,d.balance,amount);
                std::cout<<amount<<" TL YATIRILDI. KULLANILABiLiR BAKiYE: "<<d.balance<<" TL";
                break;

            }
            else if(c=="2")
            {
                std::cout<<"Cekmek istediginiz tutari giriniz: ";
                std::cin >> amount;
                if(d.withdrawMoney(ID,d.balance,amount))
                    std::cout<<amount<<" TL PARA CEKiLDi. KULLANILABiLiR BAKiYE: "<<d.balance<<" TL";
                else
                    std::cout<<"YETERSiZ BAKiYE. KULLANILABiLiR BAKiYE: "<<d.balance<<" TL";

                break;
            }
            else if(c=="3")
            {
                std::cout<<"Para transferi yapilacak musterinin;\n\
                            \t Musteri no          : ";
                std::cin >> customerID;
                std::cout<<"\t Musteri ismi        : ";
                std::cin >> customerName;
                std::cout<<"\t Gonderilecek miktar : ";
                std::cin >> amount;
                if(d.findCustomer(customerID)){


                        if(d.transferMoney(ID,customerID,amount)){
                            d.findCustomer(ID);
                            std::cout<<"\tPARA TRANSFERi GERCEKLESTi.\n\tiSLEM UCRETi: "<<d.info.toStdString()<<"\n\tKULLANILABiLiR BAKiYE: "<<d.balance<<" TL";
                        }
                        else {
                            d.findCustomer(ID);
                            std::cout<<"YETERSiZ BAKiYE! KULLANILABiLiR BAKiYE: "<<d.m_balance<<" TL";
                        }

                }
                else
                    std::cout<<"ALICI BULUNAMADI ";

                break;


            }
            else if(c=="0")
            {
                break;
            }
            else{
                std::cout<<"Hatali secim yaptiniz, tekrar giris yapiniz !!! ";
            }
        }
    }

    else
        std::cout<<"KULLANICI BULUNAMADI!!!\n ";
    }

    return a.exec();
}
