#ifndef CARSMANAGER_H
#define CARSMANAGER_H

#include <QWidget>
#include<QIcon>
#include<QDebug>
#include<QSqlRelationalTableModel>
#include<QSqlQuery>
#include<QSqlError>
#include<QMessageBox>
#include"addcar.h"
#include"updatecar.h"

namespace Ui {
class CarsManager;
}

class CarsManager : public QWidget
{
    Q_OBJECT

public:
    explicit CarsManager(QWidget *parent = nullptr);
    ~CarsManager();
    int DnametoDid(const QString& name){
        sql->prepare("SELECT DISTINCT Did FROM driverinfo WHERE Dname = :name");
        sql->bindValue(":name",name);
        sql->exec();
        sql->next();
        int res = sql->value("Did").toInt();
        //qDebug()<<"转化结果值为："<<res;
        return res;
    }

    QString CnametoCid(const QString& name){
        sql->prepare("SELECT DISTINCT Cid FROM carinfo WHERE Cname = :name");
        sql->bindValue(":name",name);
        sql->exec();
        sql->next();
        QString res = sql->value("Cid").toString();
        //qDebug()<<"转化结果值为："<<res;
        return res;
    }

    int CnametointCid(const QString& name){
        sql->prepare("SELECT DISTINCT Cid FROM carinfo WHERE Cname = :name");
        sql->bindValue(":name",name);
        sql->exec();
        sql->next();
        int res = sql->value("Cid").toInt();
        //qDebug()<<"转化结果值为："<<res;
        return res;
    }

signals:
    void sig_selectedName(QString name);
    void sig_maychanegd();
    void sig_recvchanged();

public slots:
    void sendtoupdate(QString name){
        emit sig_selectedName(name);
    }

    void maychanged(){
        emit sig_maychanegd();
    }

    void recvchanged(){
        emit sig_recvchanged();
    }

private:
    Ui::CarsManager *ui;
    QSqlRelationalTableModel *cmod;
    QSqlQuery *sql = new QSqlQuery;
    addCar *myadd;
    updateCar *myupdate;
    int nowSelected = -1;
};

#endif // CARSMANAGER_H
