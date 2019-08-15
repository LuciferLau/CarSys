#include "addcar.h"
#include "ui_addcar.h"

addCar::addCar(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addCar)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Wrench.ico"));
    setWindowTitle("添加车辆信息");
    setFixedSize(400,320);

    //ComboBox选项获取
    QSqlQuery *sql = new QSqlQuery;
    sql->exec("select distinct Dname from driverinfo where cid is null");
    while(sql->next())
        ui->driver->addItem(sql->value("Dname").toString());
    //按钮实现
    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
        close();
    });

    connect(ui->but_ok,&QPushButton::clicked,this,[=](){
        if(isEmpty(ui->name->text())){
            QMessageBox::warning(this,"非法输入","带*号的项不能为空！");
            return ;
        }
        QString Cname = ui->name->text();
        QString Dname = ui->driver->currentText();
        sendmsg(Cname,Dname); //提交数据到父窗口
        close();
    });

    if(0==ui->driver->count()){
        QMessageBox::warning(this,"添加失败","无空闲司机！");
        return;
    }
}

addCar::~addCar()
{
    delete ui;
}
