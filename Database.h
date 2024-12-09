#ifndef DATABASE_H
#define DATABASE_H

#include <QObject>
#include <QSqlDatabase>
#include <QtSql>
#include <QString>
#include <QStringList>
#include <QDebug>

class Database : public QObject
{
    Q_OBJECT
public:
    explicit Database(QString dbName, QString user, QString password, QString host, QObject *parent = nullptr);
    bool database_createConnection();
    bool CreateTableDataLogger();
signals:
    void mysqlError();

private:
    QSqlDatabase db;
    QString _dbName;
};

#endif // DATABASE_H
