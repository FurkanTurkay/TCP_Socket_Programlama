#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractSocket>
#include <QDebug>
#include <QFile>
#include <QFileDialog>
#include <QHostAddress>
#include <QMessageBox>
#include <QMetaType>
#include <QString>
#include <QStandardPaths>
#include <QTcpSocket>

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
    void displayError(QAbstractSocket::SocketError socketError);
    void replyMessage(const QString& str);
    void sendStr();

    void userLogin();

    void on_pushButton_depositMoney_clicked();

    void on_pushButton_withdrawMoney_clicked();

    void on_pushButton_transferMoney_clicked();

    void on_pushButton_exit_clicked();

    void on_pushButton_UserLogin_login_clicked();

    void on_lineEdit_UserLogin_pass_returnPressed();

    void on_pushButton_depositMoney_cancel_clicked();

    void on_pushButton_depositMoney_okay_clicked();

    void on_pushButton_withdrawMoney_cancel_clicked();

    void on_pushButton_withdrawMoney_okay_clicked();

    void on_pushButton_transferMoney_cancel_clicked();

    void on_pushButton_transferMoney_okay_clicked();

    void on_lineEdit_transferMoney_customerID_textChanged(const QString &arg1);



private:
    Ui::MainWindow *ui;

    QTcpSocket* socket;
    QStringList strServerPort;
    QStringList strList;

    QString str;
    QString strPost;
    QString strGet;

    int ID;
    QString name;
    QString bank;
    double balance;

    QString recevier_name;
    QString recevier_lastName;

};

#endif // MAINWINDOW_H
