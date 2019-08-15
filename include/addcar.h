#ifndef ADDCAR_H
#define ADDCAR_H

#include <QDialog>
#include<QMessageBox>
#include<QDebug>
#include<QSqlQuery>
#include<QSqlError>

namespace Ui {
class addCar;
}

class addCar : public QDialog
{
    Q_OBJECT

public:
    explicit addCar(QWidget *parent = nullptr);
    ~addCar();

    bool isEmpty(QString str){
        if(str=="")
            return true;
        else return false;
    }

signals:
    void sig_cardata(QString Cname,QString Dname);

public slots:
    void sendmsg(QString Cname,QString Dname){
        emit sig_cardata(Cname,Dname);
    }

private:
    Ui::addCar *ui;
};

#endif // ADDCAR_H
