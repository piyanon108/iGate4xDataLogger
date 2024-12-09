#include "ChatClient.h"

ChatClient::ChatClient(quint16 port, QString ipaddress, QString initMessage, int index, QObject *parent) :
    QObject(parent)
{
    QString url = QString("ws://%1:%2").arg(ipaddress).arg(port);
    socketIndex = index;
    m_pWebSocketClient = new QWebSocket;
    m_pWebSocketClient->open(QUrl(url));
    init_message = initMessage;
    connect(m_pWebSocketClient,SIGNAL(connected()), this, SLOT(onConnected()));

}

void ChatClient::sendTextMessage(QString message)
{
    m_pWebSocketClient->sendTextMessage(message);
}

void ChatClient::broadcastSystemMessage(QString message)
{
    m_pWebSocketClient->sendTextMessage(message);
}

void ChatClient::onConnected()
{
    qDebug() << "WebSocket connected";
    m_pWebSocketClient->sendTextMessage(init_message);
    connect(m_pWebSocketClient, SIGNAL(textMessageReceived(QString)),
            this, SLOT(onTextMessageReceived(QString)));
    m_pWebSocketClient->close();
    m_pWebSocketClient->deleteLater();
    emit connected(socketIndex);
}

void ChatClient::onTextMessageReceived(QString message)
{
    emit textMessageReceived(message);
}
