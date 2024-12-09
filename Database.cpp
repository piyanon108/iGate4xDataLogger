#include "Database.h"

Database::Database(QString dbName, QString user, QString password, QString host,QObject *parent) :
    QObject(parent)
{
    _dbName = dbName;
    QString mysqlcheck = QString("mysqlcheck -u%1 -p%2 %3").arg(user).arg(password).arg(dbName);
    system(mysqlcheck.toStdString().c_str());
    QThread::sleep(0.5);
    QThread::sleep(0.5);
    db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName(host);
    db.setDatabaseName(dbName);
    db.setUserName(user);
    db.setPassword(password);
}

bool Database::database_createConnection()
{
    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        return false;
    }
    db.close();
    qDebug() << "Database connected";
    return true;
}

bool Database::CreateTableDataLogger()
{
    bool notExist = false;
    QString query = "SELECT id FROM iGateDataLogger LIMIT 1";

    if (!db.open()) {
        qDebug() << "database error! database can not open.";
        emit mysqlError();
        return false;
    }
    QSqlQuery qry;
    qry.prepare(query);

    if (!qry.exec()){
        qDebug() << "notExist" << qry.lastError();
        notExist = true;
    }
    else
    {
        qDebug() << "DataLogger Exist";
    }

    if (notExist)
    {
        query = QString("CREATE TABLE `iGateDataLogger` ("
                        "`id` int unsigned NOT NULL AUTO_INCREMENT,"
                        "`deviceName` varchar(50) DEFAULT NULL,"
                        "`deviceAddress` varchar(50) DEFAULT NULL,"
                        "`timestamp` timestamp NULL DEFAULT NULL,"
                        "`dataLog` varchar(200) DEFAULT NULL,"
                        "`state` varchar(20) DEFAULT NULL,"
                        "PRIMARY KEY (`id`)"
                        ") ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_0900_ai_ci");
        qDebug() <<"query"<<query;
        qry.prepare(query);
        if (!qry.exec()){
            qDebug() << qry.lastError();
        }
    }
    db.close();
    return true;
}
