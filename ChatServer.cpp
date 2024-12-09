/****************************************************************************
  **
  ** Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
  ** Contact: https://www.qt.io/licensing/
  **
  ** This file is part of the QtWebSockets module of the Qt Toolkit.
  **
  ** $QT_BEGIN_LICENSE:BSD$
  ** Commercial License Usage
  ** Licensees holding valid commercial Qt licenses may use this file in
  ** accordance with the commercial license agreement provided with the
  ** Software or, alternatively, in accordance with the terms contained in
  ** a written agreement between you and The Qt Company. For licensing terms
  ** and conditions see https://www.qt.io/terms-conditions. For further
  ** information use the contact form at https://www.qt.io/contact-us.
  **
  ** BSD License Usage
  ** Alternatively, you may use this file under the terms of the BSD license
  ** as follows:
  **
  ** "Redistribution and use in source and binary forms, with or without
  ** modification, are permitted provided that the following conditions are
  ** met:
  **   * Redistributions of source code must retain the above copyright
  **     notice, this list of conditions and the following disclaimer.
  **   * Redistributions in binary form must reproduce the above copyright
  **     notice, this list of conditions and the following disclaimer in
  **     the documentation and/or other materials provided with the
  **     distribution.
  **   * Neither the name of The Qt Company Ltd nor the names of its
  **     contributors may be used to endorse or promote products derived
  **     from this software without specific prior written permission.
  **
  **
  ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
  ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
  ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
  ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
  ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
  ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
  **
  ** $QT_END_LICENSE$
  **
  ****************************************************************************/
#include "ChatServer.h"
#include <QDateTime>

#define EMER            "EMR"
#define RPTON           "RPT"
#define PTTON           "Tx"
#define RXON            "Rx"
#define PTTRX           "Tx&Rx"
#define STANDBY         "--"
#define WAIT            "***"


QT_USE_NAMESPACE

ChatServer::ChatServer(quint16 port, QObject *parent) :
    QObject(parent),
    m_pWebSocketServer(Q_NULLPTR),
    m_clients()
{
    m_pWebSocketServer = new QWebSocketServer(QStringLiteral("Chat Server"),
                                              QWebSocketServer::NonSecureMode,
                                              this);
    if (m_pWebSocketServer->listen(QHostAddress::Any, port))
    {
        qDebug() << "Chat Server listening on port" << port;
        connect(m_pWebSocketServer, &QWebSocketServer::newConnection,
                this, &ChatServer::onNewConnection);
    }
}

ChatServer::~ChatServer()
{
    m_pWebSocketServer->close();
    qDeleteAll(m_clients.begin(), m_clients.end());
}

void ChatServer::onNewConnection()
{
    QWebSocket *pSocket = m_pWebSocketServer->nextPendingConnection();
    connect(pSocket, &QWebSocket::textMessageReceived, this, &ChatServer::processMessage);
    connect(pSocket, &QWebSocket::disconnected, this, &ChatServer::socketDisconnected);

    connect(pSocket, &QWebSocket::binaryMessageReceived, [=](const QByteArray &message){
        // Handle incoming binary messages
        QJsonDocument jsonDoc = QJsonDocument::fromJson(message);
        QJsonObject jsonData = jsonDoc.object();
        emit onNewJsonMessage(jsonData, pSocket);
        // Process the JSON data
    });

    m_clients << pSocket;
    qDebug() << "On New Connection from address : " << pSocket->peerName();
    if (clientNum <= 0)
    {
        clientNum = m_clients.length();
        emit onNumClientChanged(clientNum);
    }
    else {
        clientNum = m_clients.length();
    }
}

void ChatServer::broadcastMessage(QString message){
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        pClient->sendTextMessage(message);
    }
    Q_FOREACH (QWebSocket *pClient, m_WebSocketClients)
    {
        pClient->sendTextMessage(message);
    }
}
void ChatServer::sendMessage(QWebSocket *SoftPhoneClient,QString message)
{
    Q_FOREACH (QWebSocket *pClient, m_clients)
    {
        if (SoftPhoneClient == pClient)
            pClient->sendTextMessage(message);
    }
}
void ChatServer::sendToWebMessageClient(QString message)
{
    Q_FOREACH (QWebSocket *webClient, m_WebSocketClients)
    {
        webClient->sendTextMessage(message);
    }
}
void ChatServer::commandProcess(QString message, QWebSocket *pSender){
//    qDebug() << message;
//    return;
//    message = message.replace(" ","");
    QJsonDocument d = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject command = d.object();
    QString getCommand =  QJsonValue(command["menuID"]).toString();

//    qDebug() << "getCommand" << getCommand ;//<< message;
    if (getCommand == "") {
        qDebug()<< "commandProcess not found" << message;
        return;
    }

    if (getCommand == ("startSoftPhone"))
    {
        int softPhoneID = QJsonValue(command["softPhoneID"]).toInt();
        bool found = false;
        Q_FOREACH (SoftPhoneSocketClient *sClient, softPhoneSocketClient)
        {
            if (sClient->softPhoneID == softPhoneID)
            {
                sClient->SocketClients = pSender;
                found = true;
            }
        }
        if (found == false)
        {
            SoftPhoneSocketClient *sClient = new SoftPhoneSocketClient;
            sClient->softPhoneID = softPhoneID;
            sClient->SocketClients = pSender;
            softPhoneSocketClient.append(sClient);
        }
    }
    else if (getCommand == ("getMonitorPage"))
    {
        m_WebSocketClients << pSender;
        // emit getMonitorPage(pSender);
    }
    else if (getCommand == ("channelMessage"))
    {
//        Q_FOREACH (QWebSocket *webClient, m_iGateGroupMngWebSocketClients)
//        {
//            webClient->sendTextMessage(message);
//            if(QJsonValue(command["pttInput_count"]).toInt() == 1)
//                qDebug() << "channelMessage" << message;
//        }
        int softPhoneID = QJsonValue(command["softPhoneID"]).toInt();
        int trxMode = QJsonValue(command["trxMode"]).toInt();
        bool mainTxPTTOn = QJsonValue(command["mainTxPTTOn"]).toInt();
        bool mainRxSQLOn = QJsonValue(command["mainRxSQLOn"]).toInt();
        bool standbyTxPTTOn = QJsonValue(command["standbyTxPTTOn"]).toInt();
        bool standbySQLOn = QJsonValue(command["standbyRxSQLOn"]).toInt();
        int numConn = QJsonValue(command["numConn"]).toInt();
        int numTxRx = QJsonValue(command["numTxRx"]).toInt();
        QString trxType = QJsonValue(command["mainRadioTRxType"]).toString();
        QString mainRadio01URI = QJsonValue(command["mainRadio01URI"]).toString();
        QString mainRadio02URI = QJsonValue(command["mainRadio02URI"]).toString();
        QString standbyRadio01URI = QJsonValue(command["standbyRadio01URI"]).toString();
        QString standbyRadio02URI = QJsonValue(command["standbyRadio02URI"]).toString();
        QString mainRadio01IPAddr = QJsonValue(command["mainRadio01IPAddr"]).toString();
        QString mainRadio02IPAddr = QJsonValue(command["mainRadio02IPAddr"]).toString();
        QString standbyRadio01IPAddr = QJsonValue(command["standbyRadio01IPAddr"]).toString();
        QString standbyRadio02IPAddr = QJsonValue(command["standbyRadio02IPAddr"]).toString();


        bool main01_is_mainTx = QJsonValue(command["main01_is_mainTx"]).toInt();
        bool main01_is_mainRx = QJsonValue(command["main01_is_mainRx"]).toInt();
        bool main02_is_mainRx = QJsonValue(command["main02_is_mainRx"]).toInt();
        bool standby01_is_mainTx = QJsonValue(command["standby01_is_mainTx"]).toInt();
        bool standby01_is_mainRx = QJsonValue(command["standby01_is_mainRx"]).toInt();
        bool standby02_is_mainRx = QJsonValue(command["standby02_is_mainRx"]).toInt();

        bool pttOn = ((main01_is_mainTx & mainTxPTTOn) || (standby01_is_mainTx & standbyTxPTTOn));
        bool sqlOn = ((main01_is_mainRx & mainRxSQLOn) || (standby01_is_mainRx & standbySQLOn) ||
                      (main02_is_mainRx & mainRxSQLOn) || (standby02_is_mainRx & standbySQLOn));

        // emit appendChannelData(softPhoneID, trxMode, pttOn, sqlOn, numConn, numTxRx, trxType, mainRadio01IPAddr, mainRadio02IPAddr, standbyRadio01IPAddr, standbyRadio02IPAddr);
        Q_FOREACH (QWebSocket *webClient, m_WebSocketClients)
        {
            webClient->sendTextMessage(message);
        }
    }
    else if (getCommand == ("connStatus"))
    {
        int softPhoneID = QJsonValue(command["softPhoneID"]).toInt();
        QString lastPttOn = QJsonValue(command["pttURI"]).toString();
        QString trxStatus = QJsonValue(command["TxRx"]).toString();
        QString pttURI = QJsonValue(command["pttURI"]).toString();
        bool mainTxPTTOn = (trxStatus == PTTON) ||  (trxStatus == PTTRX);
        bool mainRxSQLOn = (trxStatus == RXON)  ||  (trxStatus == PTTRX);
        int numConn = QJsonValue(command["connNum"]).toInt();
        // emit appendClientConnStatus(softPhoneID, lastPttOn, trxStatus, pttURI, mainTxPTTOn, mainRxSQLOn, numConn);
    }
    else
    {
        qDebug() << "getCommand" << getCommand ;//<< message;
    }
}

void ChatServer::processMessage(QString message)
{
//    qDebug() << message;
    QWebSocket *pSender = qobject_cast<QWebSocket *>(sender());
    commandProcess(message, pSender);
    emit onNewMessage(message, pSender);
}

void ChatServer::socketDisconnected()
{
    QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());
    if (pClient)
    {
        m_clients.removeAll(pClient);
        m_WebSocketClients.removeAll(pClient);
        m_WebSocketVUClients.removeAll(pClient);

        Q_FOREACH (SoftPhoneSocketClient *sClient, softPhoneSocketClient)
        {
            if (sClient->SocketClients == pClient)
            {
                softPhoneSocketClient.removeAll(sClient);
                break;
            }
        }

        pClient->deleteLater();
        qDebug() << pClient->localAddress().toString() << "has disconect";
    }
    clientNum = m_clients.length();
    if (clientNum <= 0)
    {
          onNumClientChanged(clientNum);
    }
}
