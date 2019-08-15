#ifndef DRIVERSMANAGER_H
#define DRIVERSMANAGER_H

#include <QWidget>
#include<QDebug>
#include<QSqlRelationalTableModel>
#include<QSqlQuery>
#include<QSqlRecord>
#include<QRadioButton>
#include"adddriver.h"
#include"updatedriver.h"

namespace Ui {
class DriversManager;
}

class DriversManager : public QWidget
{
    Q_OBJECT

signals:
    void sig_driverdata(QString name,QString sex,QString id,QString age,QString mobile);
    void sig_maychanged();
    void sig_recvchanged();

public slots:
    void senddata(QString name,QString sex,QString id,QString age,QString mobile){
        emit sig_driverdata(name,sex,id,age,mobile);
    }

    void maychanged(){
        emit sig_maychanged();
    }

    void recvchanged(){
        emit sig_recvchanged();
    }
public:
    explicit DriversManager(QWidget *parent = nullptr);
    ~DriversManager();   

private:
    Ui::DriversManager *ui;
    QSqlRelationalTableModel *dmod;
    QSqlQuery *sql;
    addDriver *add;
    updateDriver *update;
    int nowSelected = -1;
};

#endif // DRIVERSMANAGER_H
