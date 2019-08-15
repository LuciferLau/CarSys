#ifndef ADDDRIVER_H
#define ADDDRIVER_H

#include <QDialog>
#include<QDebug>
#include<QSqlError>
#include<QMessageBox>
namespace Ui {
class addDriver;
}

class addDriver : public QDialog
{
    Q_OBJECT

public:
    explicit addDriver(QWidget *parent = nullptr);
    ~addDriver();

    bool isEmpty(QString str){
        if(str=="")
            return true;
        else return false;
    }

signals:
    void sig_driverdata(QString name,QString sex,QString id,QString age,QString mobile);

public slots:
    void sendmsg(QString name,QString sex,QString id,QString age="",QString mobile=""){//两个非必填选项可以为空
        //qDebug()<<"发送的数据为："<<name<<sex<<id<<age<<mobile;
        emit sig_driverdata(name, sex, id, age, mobile);
    }

private:
    Ui::addDriver *ui;
};

#endif // ADDDRIVER_H
