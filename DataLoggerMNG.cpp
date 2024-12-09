#include "DataLoggerMNG.h"

DataLoggerMNG::DataLoggerMNG(QObject *parent) : QObject(parent)
{
    database = new Database("iGatePlus","userData","Ifz8zean6868**","127.0.0.1",this);
    SocketServer = new ChatServer(8088,this);
    database->CreateTableDataLogger();


    connect(SocketServer,SIGNAL(onNewMessage(QString , QWebSocket *)),
                      this,SLOT(onNewMessage(QString , QWebSocket *)));
    connect(SocketServer,SIGNAL(onNewJsonMessage(QJsonObject , QWebSocket *)),
                      this,SLOT(onNewJsonMessage(QJsonObject , QWebSocket *)));
}

void DataLoggerMNG::onNewMessage(QString message, QWebSocket *newClient)
{
    QJsonDocument d = QJsonDocument::fromJson(message.toUtf8());
    QJsonObject jsonData = d.object();
    QString getCommand =  QJsonValue(jsonData["menuID"]).toString();

    qDebug() << "onNewMessage getCommand" << getCommand << message;
    if (getCommand == "") {
        qDebug()<< "commandProcess not found" << message;
        return;
    }
    else
    {
        onNewJsonMessage(jsonData, newClient);
    }
}
void DataLoggerMNG::onNewJsonMessage(QJsonObject jsonData, QWebSocket *newClient)
{
    QString getCommand =  QJsonValue(jsonData["menuID"]).toString();

    qDebug() << "onNewJsonMessage getCommand" << getCommand << jsonData;
    if (getCommand == "") {
        qDebug()<< "commandProcess not found" << jsonData;
        return;
    }
    else if(getCommand == "logInsert")
    {
        QString timestamp = QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss");
        QString deviceName = QJsonValue(jsonData["deviceName"]).toString();
        QString deviceAddress = QJsonValue(jsonData["deviceAddress"]).toString();
        QString dataLog = QJsonValue(jsonData["dataLog"]).toString();
        QString state =  QJsonValue(jsonData["state"]).toString();

        qDebug() << "logInsert" << timestamp << deviceName << deviceAddress << dataLog << state;
    }
}


