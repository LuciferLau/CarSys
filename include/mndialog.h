#ifndef MNDIALOG_H
#define MNDIALOG_H

#include <QDialog>
#include<QComboBox>
#include<QSqlQuery>
#include<QDebug>
#include<QSqlError>
#include<QSqlRelationalTableModel>
#include<QMessageBox>

namespace Ui {
class mnDialog;
}

class mnDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mnDialog(QWidget *parent = nullptr);
    ~mnDialog();

    int DnameToDid(QString name){
        sql->prepare("select Did from driverinfo where Dname = :name");
        sql->bindValue(":name",name);
        sql->exec();
        if(sql->next()){
            int res = sql->value("Did").toInt();
            return res;
        }
        else qDebug()<<sql->lastError().text();
        return -1;
    }

signals:
    void sig_selectData(int Cid,int Did);
    void sig_setTime(int time);

public slots:
    void selectData(int Cid,int Did){
        emit sig_selectData(Cid,Did);
    }
    void setTime(int time){
        emit sig_setTime(time);
    }

private:
    Ui::mnDialog *ui;
    QSqlQuery *sql = new QSqlQuery;
    QSqlRelationalTableModel *mod;
    int nowSelected = -1;
};

#endif // MNDIALOG_H
