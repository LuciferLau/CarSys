#ifndef UPDATECAR_H
#define UPDATECAR_H

#include <QDialog>
#include<QSqlQuery>
namespace Ui {
class updateCar;
}

class updateCar : public QDialog
{
    Q_OBJECT

public:
    explicit updateCar(QWidget *parent = nullptr);
    ~updateCar();

signals:
    void sig_drivername(QString name);
    void sig_recvname(QString name);

public slots:
    void sendmsg(QString name){
        emit sig_drivername(name);
    }
    void recvmsg(QString name){
        emit sig_recvname(name);
    }

private:
    Ui::updateCar *ui;
};

#endif // UPDATECAR_H
