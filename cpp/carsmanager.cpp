#include "carsmanager.h"
#include "ui_carsmanager.h"

CarsManager::CarsManager(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::CarsManager)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/Menu/IconRes/Gear.ico"));
    setWindowTitle("车辆管理");
    setFixedSize(600,400);
    cmod = new QSqlRelationalTableModel(this);
    ui->tableView->setModel(cmod);
    cmod->setTable("carinfo");
    //设置表格策略
    //ui->tableView->hideColumn(0);//隐藏cid行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置为选中行;
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    cmod->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交
    //设置表格标题
    cmod->setHeaderData(0,Qt::Horizontal,"车辆ID");//Cid
    cmod->setHeaderData(1,Qt::Horizontal,"车辆名");//Cname
    cmod->setHeaderData(2,Qt::Horizontal,"驾驶员姓名");//Did
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(70);
    //获取外键数据
    cmod->setRelation(2,QSqlRelation("driverinfo","Did","Dname"));
    cmod->setJoinMode(QSqlRelationalTableModel::LeftJoin);//左连接模式
    cmod->select();

    //获取选中的行
    connect(ui->tableView,&QAbstractItemView::clicked,this,[=](){
        nowSelected = ui->tableView->currentIndex().row();
        qDebug()<<"当前选中行为："<< nowSelected;
    });

    //筛选条件
    ui->check_all->click();
    connect(ui->check_all,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<cmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        cmod->select();
    });

    connect(ui->check_had,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<cmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        int row = cmod->rowCount();
        for(int i=0;i<row;++i){
            QString res = cmod->data(cmod->index(i,2)).toString();
            if(""==res)//如果这行数据没司机
                ui->tableView->hideRow(i);//隐藏这行
        }
        cmod->select();
    });

    connect(ui->check_not,&QRadioButton::clicked,this,[=](){
        for(int i=0;i<cmod->rowCount();++i)
            ui->tableView->showRow(i);//显示所有行
        int row = cmod->rowCount();
        for(int i=0;i<row;++i){
            QString res = cmod->data(cmod->index(i,2)).toString();
            if(""!=res)//如果这行数据有司机
                ui->tableView->hideRow(i);//隐藏这行
        }
        cmod->select();
    });

    connect(ui->but_add,&QPushButton::clicked,this,[=](){
        myadd = new addCar(this);
        connect(myadd,&addCar::sig_cardata,this,[=](QString Cname,QString Dname){
            //  qDebug()<<"收到信息："<<Cname<<" "<<Dname;
            QString name = Cname;
            int driver = DnametoDid(Dname);
            //  qDebug()<<"转化为："<<Cname<<" "<<driver;
            //向表carinfo添加数据
            sql->prepare("INSERT INTO carinfo (Cname, Did) VALUES (:Cname, :Did)");
            sql->bindValue(":Cname",name);
            sql->bindValue(":Did",driver);
            if(sql->exec()){
                qDebug()<<"添加完成！";
                cmod->submit();                
            }
            else {
                 qDebug()<<"添加失败！";
                cmod->revert();
            }

            //同时修改driverinfo里面的数据
            QString updateid = CnametoCid(name);
            // qDebug()<<"转化为："<<updateid<<" "<<Dname;
            sql->prepare("UPDATE driverinfo SET Cid = :Cid where Dname = :name");
            sql->bindValue(":Cid",updateid);
            sql->bindValue(":name",Dname);
            if(sql->exec()){
                qDebug()<<"执行完成！";
                cmod->submit();              
            }
            else {
                 qDebug()<<"修改失败！";
                cmod->revert();
            }
            cmod->select();
        });
        cmod->select();
        myadd->exec();
    });

    connect(ui->but_update,&QPushButton::clicked,this,[=](){
        if(-1 == nowSelected)
            QMessageBox::warning(this,"错误","请先选中行");
        else {
            myupdate = new updateCar(this);
            connect(this,&CarsManager::sig_selectedName,myupdate,[=](QString name){
                myupdate->recvmsg(name);//发回给update窗口
            });
            //获取选中行信息
            int Cid = cmod->data(cmod->index(nowSelected,0)).toInt();
            QString Dname = cmod->data(cmod->index(nowSelected,2)).toString();
            sendtoupdate(Dname);

            connect(myupdate,&updateCar::sig_drivername,this,[=](QString name){
                int Did = DnametoDid(name);
                //对carinfo进行修改
                sql->prepare("UPDATE carinfo SET Did = :Did where Cid = :Cid");
                sql->bindValue(":Did",Did);
                sql->bindValue(":Cid",Cid);
                sql->exec();

                //同时对driverinfo进行修改 name是新的车主名，Dname是旧的车主名
                sql->prepare("UPDATE driverinfo SET Cid = :Cid where Dname = :Dname");
                sql->bindValue(":Dname",name);
                sql->bindValue(":Cid",Cid);
                sql->exec();
                //新车主赋值新的Cid，旧车主设为null。
                sql->prepare("UPDATE driverinfo SET Cid = null where Dname = :Dname");
                sql->bindValue(":Dname",Dname);
                sql->exec();

            });
            cmod->select();
            myupdate->exec();
        }
    });

    connect(ui->but_delete,&QPushButton::clicked,this,[=](){
        if(-1 == nowSelected)
            QMessageBox::warning(this,"错误","请先选中行");
        else {
            cmod->removeRow(nowSelected);
            ui->tableView->hideRow(nowSelected);//先隐藏掉，模拟删除
        }
    });

    connect(ui->but_fin,&QPushButton::clicked,this,[=](){
        if(!cmod->isDirty())
            close();
        else if(QMessageBox::warning(this,"提交数据","确定修改吗？",QMessageBox::Ok,QMessageBox::No)==QMessageBox::Ok)
            cmod->submitAll();
        else cmod->revertAll();
        maychanged();
        close();
    });

    connect(ui->but_serach,&QPushButton::clicked,this,[=](){
        QString name = "Cname LIKE '%"+ui->lineEdit->text()+"%'";//模糊查询
        qDebug()<<name;
        ui->lineEdit->clear();
        cmod->setFilter(name);
        cmod->select();
    });

    connect(this,&CarsManager::sig_recvchanged,this,[=](){
        cmod->select();//收到DriversManager发来的消息就执行查询
    });

}

CarsManager::~CarsManager()
{
    delete ui;
}
