#ifndef SERVER_H
#define SERVER_H

#include <QObject>
#include <QTcpSocket>
#include <QTcpServer>

class server : public QObject
{
    Q_OBJECT
public:
    explicit server(QObject *parent = nullptr);
    void serverStuff();
    void send2client();
    void close();

signals:
private:
    QTcpSocket *tcpSocket;
    QTcpServer *tcpServer;
    QString getStr;
    QString postStr;

    std::string entry;
};

#endif // SERVER_H
