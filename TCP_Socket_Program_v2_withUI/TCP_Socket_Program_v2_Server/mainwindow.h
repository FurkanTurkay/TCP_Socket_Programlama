#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>

#include <QMessageBox>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>
#include <QStringList>

#include "database.h"

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
    void newConnection();
    void appendToSocketList(QTcpSocket* socket);

    void readSocket();
    void discardSocket();
    void displayError(QAbstractSocket::SocketError socketError);

    void replyMessage(const QString& str);
    void sendMessage(QTcpSocket* socket);


    void refreshLabel();
    void AutoSentMessage();
    void matchIDwithSocket(int clientID,bool operation);
    void deleteSocketID();
    void findSocketNumberLeavingServer();
    bool sameUserError(int clientID);




private:
    Ui::MainWindow *ui;

    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
    QStringList strList;
    QStringList strClientPort;
    QString strSocket;
    QStringList receiverClientList;
    QStringList m_receiverClientList;
    QStringList socketList;
    QStringList m_socketList;
    QString socketListStr;
    QString socketNumberLeavingServer;


    int ID;
    QString strPost;
    QString label;

    database d;


    int clientNumber;
    QString text_clientNumberwithID;

    bool boolValue;
};

#endif // MAINWINDOW_H
