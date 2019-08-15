#include "updatedriver.h"
#include "ui_updatedriver.h"

updateDriver::updateDriver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateDriver)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Wrench.ico"));
    setWindowTitle("修改驾驶员信息");
    setFixedSize(400,320);

    connect(this,&updateDriver::sig_recv,this,[=](QString name,QString sex,QString id,QString age,QString mobile){
        ui->name->setText(name);
        ui->name->setEnabled(false);
        ui->sex->setCurrentText(sex);
        ui->sex->setEnabled(false);
        ui->id->setText(id);
        ui->id->setEnabled(false);
        //前三项不可编辑
        ui->age->setText(age);
        ui->mobile->setText(mobile);
    });

    connect(ui->but_ok,&QPushButton::clicked,this,[=](){
        sendmsg(ui->age->text(),ui->mobile->text());
        close();
    });

    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
        close();
    });
}

updateDriver::~updateDriver()
{
    delete ui;
}
