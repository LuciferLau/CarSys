#include "mndialog.h"
#include "ui_mndialog.h"

mnDialog::mnDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mnDialog)
{
    ui->setupUi(this);
    setWindowTitle("选择车辆");
    setWindowIcon(QIcon(":/Menu/IconRes/Search.ico"));
    setFixedSize(400,300);

    mod = new QSqlRelationalTableModel(this);
    ui->tableView->setModel(mod);
    mod->setTable("carinfo");
    //设置表格策略
    //ui->tableView->hideColumn(0);//隐藏cid行
    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//设置为选中行;
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    mod->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交
    //设置表格标题
    mod->setHeaderData(0,Qt::Horizontal,"车辆ID");//Cid
    mod->setHeaderData(1,Qt::Horizontal,"车辆名");//Cname
    mod->setHeaderData(2,Qt::Horizontal,"驾驶员姓名");//Did
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(100);
    //获取外键数据
    mod->setRelation(2,QSqlRelation("driverinfo","Did","Dname"));
    mod->setJoinMode(QSqlRelationalTableModel::LeftJoin);//左连接模式
    mod->select();

    //获取选中的行
    connect(ui->tableView,&QAbstractItemView::clicked,this,[=](){
        nowSelected = ui->tableView->currentIndex().row();
        qDebug()<<"当前选中行为："<< nowSelected;
    });

    connect(ui->but_ok,&QPushButton::clicked,this,[=](){
        if(-1 == nowSelected){
            QMessageBox::warning(this,"错误","请先选中行");
            return;
        }
        if(ui->time->text().isEmpty()){
            QMessageBox::warning(this,"错误","间隔不能为空");
            return;
        }
        //发送模拟间隔到主窗口
        int time = ui->time->text().toInt();
        setTime(time);
        //发送Cid和Did到主窗口
        int cid = mod->data(mod->index(nowSelected,0)).toInt();
        QString dname = mod->data(mod->index(nowSelected,2)).toString();
        int did = DnameToDid(dname);
        selectData(cid,did);

        close();
    });

    connect(ui->but_cancel,&QPushButton::clicked,this,[=](){
        close();
    });
}

mnDialog::~mnDialog()
{
    delete ui;
}
