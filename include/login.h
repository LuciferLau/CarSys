#ifndef LOGIN_H
#define LOGIN_H

#include<QDialog>
#include<QMessageBox>
#include<QIcon>

namespace Ui {
class login;
}

class login : public QDialog
{
    Q_OBJECT

public:
    explicit login(QWidget *parent = nullptr);
    ~login();

signals:
    void sig_success();

public slots:
    void success(){
        emit sig_success();
    }

private:
    Ui::login *ui;

};


#endif // LOGIN_H
