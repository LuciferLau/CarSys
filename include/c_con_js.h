#ifndef C_CON_JS_H
#define C_CON_JS_H

#include <QObject>
#include<QDebug>
//这个是连接C++和JS的关键类

class C_con_JS : public QObject
{
    Q_OBJECT
public:
    explicit C_con_JS(QObject *parent = nullptr);

signals:
    Q_INVOKABLE void sig_test(){
        qDebug()<<"i am test";
    }

public slots:
    void test();
};

#endif // C_CON_JS_H
