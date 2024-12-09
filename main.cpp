#include <QCoreApplication>
#include <DataLoggerMNG.h>
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    DataLoggerMNG dataLoggerMng;
    return a.exec();
}
