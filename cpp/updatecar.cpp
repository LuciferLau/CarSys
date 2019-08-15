#include "updatecar.h"
#include "ui_updatecar.h"

updateCar::updateCar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::updateCar)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Wrench.ico"));
    setWindowTitle("更改司机");
    setFixedSize(400,200);

    //ComboBox选项获取
    QSqlQuery *sql = new QSqlQuery;
    sql->exec("select distinct Dname from driverinfo where Cid is null");
    while(sql->next())
        ui->Drivername->addItem(sql->value("Dname").toString());

    connect(this,&updateCar::sig_recvname,this,[=](QString name){
        int num = ui->Drivername->count();//里面的item个数
        for(int i = 0;i<num;++i){
            if(ui->Drivername->itemText(i)==name)//防止重复添加
                return;
        }
        ui->Drivername->addItem(name);
    });

    connect(ui->but_ok,&QPushButton::clicked,this,[=](){
        QString name = ui->Drivername->currentText();
        sendmsg(name);
        close();
    });

    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
        close();
    });
}

updateCar::~updateCar()
{
    delete ui;
}
