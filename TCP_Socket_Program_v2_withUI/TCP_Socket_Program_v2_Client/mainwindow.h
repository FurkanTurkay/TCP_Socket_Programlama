#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QDebug>
#include <QHostAddress>
#include <QMessageBox>
#include <QString>
#include <QTcpSocket>
#include <QThread>
#include <QValidator>
#include <QDataStream>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


signals:
    void newMessage(QString);
private slots:
    void readSocket();
    void discardSocket();
    void connectServer();
    void displayError(QAbstractSocket::SocketError socketError);
    void readMessage(const QString& str);
    void sendStr();

    void userLogin();
    void depositMoney();
    void withdrawMoney();
    void transferMoney();
    bool invalidEntry(QString text);

    void on_pushButton_depositMoney_clicked();

    void on_pushButton_withdrawMoney_clicked();

    void on_pushButton_transferMoney_clicked();

    void on_pushButton_exit_clicked();

    void on_pushButton_UserLogin_login_clicked();

    void on_lineEdit_UserLogin_pass_returnPressed();

    void on_pushButton_depositMoney_cancel_clicked();

    void on_pushButton_depositMoney_okay_clicked();

    void on_lineEdit_depositMoney_amount_returnPressed();

    void on_pushButton_withdrawMoney_cancel_clicked();

    void on_pushButton_withdrawMoney_okay_clicked();

    void on_lineEdit_withdrawMoney_amount_returnPressed();

    void on_pushButton_transferMoney_cancel_clicked();

    void on_pushButton_transferMoney_okay_clicked();

    void on_lineEdit_transferMoney_amount_returnPressed();

    void on_lineEdit_transferMoney_customerID_editingFinished();

    void on_lineEdit_transferMoney_customerID_textEdited(const QString &arg1);

    void on_pushButton_UserLogin_cancel_clicked();

    void on_pushButton_reconnect_clicked();

private:
    Ui::MainWindow *ui;

    QTcpSocket* socket;
    QStringList strServerPort;
    QStringList strList;

    QString str;
    QString strPost;
    QString strGet;


    int ID=-1;
    QString name;
    QString bank;
    double balance;

    QString recevier_name;
    QString recevier_lastName;

    bool serverStatus; // true: Server is running.
    bool IsProgramWorking=true;

};

#endif // MAINWINDOW_H
