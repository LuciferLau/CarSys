#include "login.h"
#include "ui_login.h"

login::login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::login)
{
    ui->setupUi(this);
    //页面设计
    setWindowIcon(QIcon(":/Menu/IconRes/Power.ico"));
    setWindowTitle("Login");
    ui->password->setEchoMode(QLineEdit::Password);//隐藏密码
    connect(ui->but_login,&QPushButton::clicked,this,[=](){
        QString user = ui->userid->text();
        QString pw = ui->password->text();

        if(user.isEmpty()){
            QMessageBox::warning(this,"Error","用户名不为空！");
            return;
        }
        else if(pw.isEmpty()){
            QMessageBox::warning(this,"Error","密码不为空！");
            return;
        }
        else {
            if(user != "admin"){
                QMessageBox::warning(this,"Error","用户名不存在！");
                return;
            }
            else if(pw != "admin"){
                QMessageBox::warning(this,"Error","密码错误！");
                return;
            }
            else{
                QDialog::accept();
            }
        }

    });

    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
       QDialog::reject();
    });

}

login::~login()
{
    delete ui;
}

