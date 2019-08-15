#include "adddriver.h"
#include "ui_adddriver.h"

addDriver::addDriver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addDriver)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Wrench.ico"));
    setWindowTitle("添加驾驶员信息");
    setFixedSize(400,320);
    //按钮实现
    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
        close();
    });

    connect(ui->but_ok,&QPushButton::clicked,this,[=](){

        if(isEmpty(ui->name->text()) || isEmpty(ui->id->text())  ){
            QMessageBox::warning(this,"非法输入","带*号的项不能为空！");
            return ;
        }

        QString name, sex, id, age, mobile;
        name=ui->name->text();
        sex=ui->sex->currentText();
        id=ui->id->text();
        age=ui->age->text();
        mobile=ui->mobile->text();
        sendmsg(name,sex,id,age,mobile); //提交数据到父窗口

        close();
    });
}

addDriver::~addDriver()
{
    delete ui;
}
