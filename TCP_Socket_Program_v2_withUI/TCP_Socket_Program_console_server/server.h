#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QDebug>
#include <QMetaType>
#include <QSet>
#include <QStandardPaths>
#include <QTcpServer>
#include <QTcpSocket>

#include "database.h"

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    ~server();
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

    void infoDebug();
    void AutoSentMessage();

private:
    QTcpServer* m_server;
    QSet<QTcpSocket*> connection_set;
    QStringList strList;
    QStringList strClientPort;
    int ID;
    QString strPost;
    QString infoText;
    database d;


};

#endif // SERVER_H
