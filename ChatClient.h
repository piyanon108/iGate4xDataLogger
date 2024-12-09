#ifndef CHATCLIENT_H
#define CHATCLIENT_H

#include <QObject>
#include <QList>
#include <QByteArray>
#include "QWebSocketServer"
#include "QWebSocket"
#include <QDebug>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonDocument>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class ChatClient : public QObject
{
    Q_OBJECT
public:
    explicit ChatClient(quint16 port, QString ipaddress, QString initMessage,int index, QObject *parent = Q_NULLPTR);

    void broadcastSystemMessage(QString message);

public slots:
   void sendTextMessage(QString message);
signals :

    void connected(int index);
    void textMessageReceived(QString message);

private:
    QWebSocket *m_pWebSocketClient;
    QList<QWebSocket *> m_clients;
    QWebSocket *newSocket;
    QString init_message;
    int socketIndex = 0;

private slots:
    void onConnected();
    void onTextMessageReceived(QString message);
};

#endif // CHATCLIENT_H
