#ifndef DATALOGGERMNG_H
#define DATALOGGERMNG_H

#include <QObject>
#include <Database.h>
#include <ChatServer.h>

class DataLoggerMNG : public QObject
{
    Q_OBJECT
public:
    explicit DataLoggerMNG(QObject *parent = nullptr);
    

signals:

private:
    Database *database;
    ChatServer *SocketServer;



private slots:
    void onNewMessage(QString message, QWebSocket *newClient);
    void onNewJsonMessage(QJsonObject jsonData, QWebSocket *newClient);
};

#endif // DATALOGGERMNG_H
