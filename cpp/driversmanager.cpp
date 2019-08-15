#include "driversmanager.h"
#include "ui_driversmanager.h"

DriversManager::DriversManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DriversManager)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Contact.ico"));
    setWindowTitle("驾驶员管理");
    dmod = new QSqlRelationalTableModel(this);
    ui->tableView->setModel(dmod);
    dmod->setTable("driverinfo");
    //设置表格策略
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置为选中行
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    dmod->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交
    //设置表格格式
    dmod->setHeaderData(0,Qt::Horizontal,"驾驶员ID");//Did
    dmod->setHeaderData(1,Qt::Horizontal,"姓名");//Dname
    dmod->setHeaderData(2,Qt::Horizontal,"性别");//Dsex
    dmod->setHeaderData(3,Qt::Horizontal,"身份证");//Didcard
    dmod->setHeaderData(4,Qt::Horizontal,"年龄");//Dage
    dmod->setHeaderData(5,Qt::Horizontal,"手机号码");//Dmobile
    dmod->setHeaderData(6,Qt::Horizontal,"状态");//Dstatus
    dmod->setHeaderData(7,Qt::Horizontal,"驾驶车辆名");//Cid
    //获取外键数据
    dmod->setRelation(7,QSqlRelation("carinfo","Cid","Cname"));
    //左连接模式，从左表（driverinfo）返回所有的行，即使右表（carinfo）没有一个匹配，也会显示该项
    dmod->setJoinMode(QSqlRelationalTableModel::LeftJoin);//左连接模式
    dmod->select();
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(70);

    sql = new QSqlQuery;//初始化query对象
    //筛选条件
    ui->check_all->click();
    connect(ui->check_all,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<dmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        dmod->select();
    });

    connect(ui->check_had,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<dmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        int row = dmod->rowCount();
        for(int i=0;i<row;++i){
            QString res = dmod->data(dmod->index(i,7)).toString();
            if(""==res)//如果这行数据没车
                ui->tableView->hideRow(i);//隐藏这行
        }
        dmod->select();
    });

    connect(ui->check_not,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<dmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        int row = dmod->rowCount();
        for(int i=0;i<row;++i){
            QString res = dmod->data(dmod->index(i,7)).toString();
            if(""!=res)//如果这行数据有车
                ui->tableView->hideRow(i);//隐藏这行
        }
        dmod->select();
    });

    //按钮实现
    //注意：Calling select() will revert any unsubmitted changes and remove any inserted columns.

    //添加功能
    connect(ui->but_add,&QPushButton::clicked,this,[=](){
        add = new addDriver(this);
        connect(add,&addDriver::sig_driverdata,this,[=](QString name,QString sex,QString id,QString age,QString mobile){
            //qDebug()<<"收到消息："<<name<<sex<<id<<age.toInt()<<mobile;
            //预处理
            sql->prepare("INSERT INTO driverinfo (Dname, Dsex, Didcard, Dage, Dmobile) VALUES (:name, :sex, :id, :age, :mobile)");
            sql->bindValue(":name",name);
            sql->bindValue(":sex",sex);
            sql->bindValue(":id",id);
            sql->bindValue(":age",age.toInt());
            sql->bindValue(":mobile",mobile);
            sql->exec();
            if(sql->isActive())
                qDebug()<<"执行完毕！";
            else qDebug()<<"执行失败！"<<dmod->lastError().text();
            dmod->select();
        });
        dmod->select();
        add->exec();
    });

    //获取选中的行
    connect(ui->tableView,&QAbstractItemView::clicked,this,[=](){
        nowSelected = ui->tableView->currentIndex().row();
        qDebug()<<"当前选中行为："<< nowSelected;
    });

    //修改功能
    connect(ui->but_update,&QPushButton::clicked,this,[=](){
        if(-1 == nowSelected)
            QMessageBox::warning(this,"错误","请先选中行");
        else{
            update = new updateDriver(this);
            connect(this,&DriversManager::sig_driverdata,update,[=](QString name,QString sex,QString id,QString age,QString mobile){
                update->receivemsg(name,sex,id,age,mobile);//发回update窗口
            });

            //获取选中行的信息，并发送回update窗口，带星号选项不可修改
            QString name = dmod->data(dmod->index(nowSelected,1)).toString();
            QString sex = dmod->data(dmod->index(nowSelected,2)).toString();
            QString id = dmod->data(dmod->index(nowSelected,3)).toString();
            QString age = dmod->data(dmod->index(nowSelected,4)).toString();
            QString mobile = dmod->data(dmod->index(nowSelected,5)).toString();
            senddata(name,sex,id,age,mobile);

            //从update窗口收到修改后的信息
            QString Did = dmod->data(dmod->index(nowSelected,0)).toString();//当前选中字段的PK
            connect(update,&updateDriver::sig_send,this,[=](QString age, QString mobile){
                //qDebug()<<"收到修改后的信息："<<id<<age<<mobile;
                sql->prepare("UPDATE driverinfo SET Dage = :age, Dmobile = :mobile where Did = :id");
                sql->bindValue(":age",age);
                sql->bindValue(":mobile",mobile);
                sql->bindValue(":id",Did);
                sql->exec();
                if(sql->isActive())
                    qDebug()<<"执行完毕！";
                else qDebug()<<"执行失败！"<<dmod->lastError().text();
                dmod->select();
            });
            update->exec();
        }
    });

    //删除功能
    connect(ui->but_delete,&QPushButton::clicked,this,[=](){
        if(-1 == nowSelected)
            QMessageBox::warning(this,"错误","请先选中行");
        else {
            dmod->removeRow(nowSelected);
            ui->tableView->hideRow(nowSelected);//先隐藏掉，模拟删除
        }
    });

    //提交功能
    connect(ui->but_fin,&QPushButton::clicked,this,[=](){
        if(!dmod->isDirty())
            close();
        else if(QMessageBox::warning(this,"提交数据","确定修改吗？",QMessageBox::Ok,QMessageBox::No)==QMessageBox::Ok)
            dmod->submitAll();
        else dmod->revertAll();
        maychanged();
        close();
    });

    //按名字查询
    connect(ui->but_search,&QPushButton::clicked,this,[=](){
        QString name = "Dname LIKE '%"+ui->name->text()+"%'";//模糊查询
        qDebug()<<name;
        ui->name->clear();
        dmod->setFilter(name);
        dmod->select();
    });

    connect(this,&DriversManager::sig_recvchanged,this,[=](){
        dmod->select();//收到CarsManager发来的消息就执行查询
    });
}

DriversManager::~DriversManager()
{
    delete ui;
}
