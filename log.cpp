#include "log.h"

Log::Log(QObject *parent) :
    QObject(parent)
{
}

void Log::append(QString str){
    emit appendLog(str);
}
