#ifndef LOG_H
#define LOG_H

#include <QObject>

class Log : public QObject
{
    Q_OBJECT
public:
    explicit Log(QObject *parent = 0);

    void append(QString str);
    void clear();
    
signals:
    void appendLog(QString str);
    
public slots:
    
};

#endif // LOG_H
