#ifndef UPDATEDRIVER_H
#define UPDATEDRIVER_H

#include <QDialog>

namespace Ui {
class updateDriver;
}

class updateDriver : public QDialog
{
    Q_OBJECT

public:
    explicit updateDriver(QWidget *parent = nullptr);
    ~updateDriver();

signals:
    void sig_recv(QString name,QString sex,QString id,QString age,QString mobile);
    void sig_send(QString age,QString mobile);

public slots:
    void receivemsg(QString name,QString sex,QString id,QString age,QString mobile){
        emit sig_recv(name,sex,id,age,mobile);
    }
    void sendmsg(QString age,QString mobile){
        emit sig_send(age,mobile);
    }
private:
    Ui::updateDriver *ui;
};

#endif // UPDATEDRIVER_H
