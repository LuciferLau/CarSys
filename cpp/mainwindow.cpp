#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    initwindow();//登陆窗口
    initconn();//初始化qt和js的连接
    initdb();//初始化数据库
    sql = new QSqlQuery;//初始化数据查询语句

    //设置窗口默认大小；标题文字；图标。
    resize(1280,720);
    setWindowTitle("车辆信息管理系统");
    setWindowIcon(QIcon(":/IconRes/Earth.ico"));

    //右边时间控件
    ui->timewidget->dynamicCall("Navigate(const QString&)","E:/CarSys/time.html");
    //高德地图
    ui->mapWidget->dynamicCall("Navigate(const QString&)","E:/CarSys/gdmap.html");

    //menubar车辆管理
    carm = new CarsManager;
    connect(ui->actionManageCar,&QAction::triggered,this,[=](){
        carm->show();
    });

    //menubar驾驶员管理
    drim = new DriversManager;
    connect(ui->actionManageDriver,&QAction::triggered,this,[=](){
        drim->show();
    });

    connect(drim,&DriversManager::sig_maychanged,carm,[=](){
        carm->recvchanged();//收到数据表变化提示
    });

    connect(carm,&CarsManager::sig_maychanegd,drim,[=](){
        drim->recvchanged();//收到数据表变化提示
    });

    //menubar图层变化
    connect(ui->actiontraffic,&QAction::triggered,this,[=](){
        conn->mapswitchtraffic();
    });
    connect(ui->actionsatellite,&QAction::triggered,this,[=](){
        conn->mapswitchsatellite();
    });
    connect(ui->actionroadnet,&QAction::triggered,this,[=](){
        conn->mapswitchroadnet();
    });
    connect(ui->actiontheme,&QAction::triggered,this,[=](){
        Theme *tmp = new Theme;
        tmp->show();
        connect(tmp,&Theme::sigsendcolor,this,[=](QString color){
            conn->mapswitchtheme(color);
        });
    });
    //menubar帮助
    connect(ui->actionabout,&QAction::triggered,this,[=](){
        about msg(this);
        msg.exec();
    });

    //toolbar经纬度定位
    QAction *act_locate=new QAction(this);
    act_locate->setIcon(QIcon(":/Menu/IconRes/GPS.ico"));
    act_locate->setText("定位");
    QLabel *x=new QLabel(this);
    QLabel *y=new QLabel(this);
    QLineEdit *xe=new QLineEdit(this);
    QLineEdit *ye=new QLineEdit(this);
    x->setText(" 经度：");    y->setText(" 纬度：");
    xe->setMaximumSize(80,20);     ye->setMaximumSize(80,20);
    ui->mainToolBar->addWidget(x);
    ui->mainToolBar->addWidget(xe);
    ui->mainToolBar->addWidget(y);
    ui->mainToolBar->addWidget(ye);
    ui->mainToolBar->addAction(act_locate);
    xe->setText("113.000000");//初始化
    ye->setText("23.000000");

    //toolbar登陆&退出
    connect(ui->actionlogin,&QAction::triggered,this,[=](){});//登陆按钮未定义功能
    ui->actionlogin->setDisabled(true);

    connect(ui->actionexit,&QAction::triggered,this,[=](){
        hide();
        begin = new login;
        begin->show();
        if(begin->exec()==QDialog::Accepted){
            begin->success();
            show();
        }
        else exit(0);
    });

    //toolbar定位按钮
    static double tbx,tby;
    connect(act_locate,&QAction::triggered,this,[=]( ){
        tbx = xe->text().toDouble();
        tby = ye->text().toDouble();
        if(tbx<-180||tbx>180||tby>90||tby<-90){
            QMessageBox::warning(this,"ERROR","经度范围（-180,180）纬度范围（-90,90）！");
            return;
        }
        conn->locate(tbx,tby);//调用JS定位
    });
    ui->mainToolBar->addSeparator();//添加分隔符

    //toolbar快速定位按钮
    QAction *act_spd = new QAction("快速定位",this);
    act_spd->setIcon(QIcon(":/Menu/IconRes/locate.ico"));
    ui->mainToolBar->addAction(act_spd);
    connect(act_spd,&QAction::triggered,this,[=](){
        conn->getcent();//从JS端获取地图中心点经纬度
        connect(conn,&ConToJS::sig_centtoqt,this,[=](QString lng,QString lat){
            xe->setText(lng);//将经纬度设置回LineEdit
            ye->setText(lat);
        });
    });

    //地图标点获取坐标实现
    const int maxsize = 1000;
    static int cur = 0;
    connect(conn,&ConToJS::sig_addMarker,this,[=](QString marker){
        qDebug()<<"收到坐标点信号："<<marker;
        Markers[cur++] = marker;
        if(cur==2){//若标点数大于等于2，则使功能可用
            act_draw->setDisabled(false);
            act_checkdis->setDisabled(false);
        }
        if(cur==maxsize){
            QMessageBox::warning(this,"警告","地图上最多标注1000个点！");
            conn->erase();
        }
    });

    //toolbar划线按钮
    act_draw = new QAction("划线",this);
    act_draw->setIcon(QIcon(":/Menu/IconRes/Pen.ico"));
    ui->mainToolBar->addAction(act_draw);
    act_draw->setDisabled(true);//默认状态不可用
    connect(act_draw,&QAction::triggered,this,[=](){
        QString start_lng = Markers[0].mid(0,10);
        QString start_lat = Markers[0].mid(11,10);
        //        QString end_lng = Markers[1].mid(0,10);
        //        QString end_lat = Markers[1].mid(11,10);
        const int lastindex = cur-1;
        QString end_lng = Markers[lastindex].mid(0,10);
        QString end_lat = Markers[lastindex].mid(11,10);
        conn->getroute(start_lng,start_lat,end_lng,end_lat);//点击地图速度过快，数据可能传输失败导致程序崩溃
        //数组归位
        act_draw->setDisabled(true);
        act_checkdis->setDisabled(true);
        Markers->clear();
        cur = 0;
    });

    //toolbar测距按钮
    act_checkdis = new QAction("测距",this);
    act_checkdis->setIcon(QIcon(":/Menu/IconRes/ruler.ico"));
    ui->mainToolBar->addAction(act_checkdis);
    act_checkdis->setDisabled(true);//默认状态不可用
    connect(act_checkdis,&QAction::triggered,this,[=](){
        QString start_lng = Markers[0].mid(0,10);
        QString start_lat = Markers[0].mid(11,10);
        //        QString end_lng = Markers[1].mid(0,10);
        //        QString end_lat = Markers[1].mid(11,10);
        const int lastindex = cur-1;
        QString end_lng = Markers[lastindex].mid(0,10);
        QString end_lat = Markers[lastindex].mid(11,10);
        conn->checkdis(start_lng,start_lat,end_lng,end_lat);
        //数组归位
        act_draw->setDisabled(true);
        act_checkdis->setDisabled(true);
        Markers->clear();
        cur = 0;
    });

    //toolbar擦除按钮
    act_eraser = new QAction("擦除",this);
    act_eraser->setIcon(QIcon(":/Menu/IconRes/erase.ico"));
    ui->mainToolBar->addAction(act_eraser);
    connect(act_eraser,&QAction::triggered,this,[=](){
        act_draw->setDisabled(true);
        act_checkdis->setDisabled(true);
        conn->erase();
        //数组归位
        Markers->clear();
        cur = 0;
    });

    //tableview表格区
    rmod = new QSqlRelationalTableModel(this);//初始化model
    ui->tableView->setModel(rmod);//把model对象赋给tableview
    rmod->setTable("roadinfo");//表名

    ui->tableView->setSelectionBehavior(QAbstractItemView::SelectRows);//选中行
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);//不可编辑
    rmod->setEditStrategy(QSqlTableModel::OnManualSubmit);//手动提交
    //设计表格形式

    rmod->setHeaderData(3,Qt::Horizontal,"车辆名");
    rmod->setHeaderData(4,Qt::Horizontal,"驾驶员姓名");
    rmod->setHeaderData(5,Qt::Horizontal,"驾驶员状态");
    rmod->setHeaderData(6,Qt::Horizontal,"车辆速度");
    rmod->setHeaderData(7,Qt::Horizontal,"行驶方向");
    rmod->setHeaderData(8,Qt::Horizontal,"经度");
    rmod->setHeaderData(9,Qt::Horizontal,"纬度");
    rmod->setHeaderData(10,Qt::Horizontal,"地理位置");
    rmod->setHeaderData(11,Qt::Horizontal,"数据录入时间");
    rmod->select();//查询数据
    ui->tableView->resizeColumnsToContents();
    ui->tableView->horizontalHeader()->setMinimumSectionSize(85);
    //从外键获取数据
    rmod->setRelation(3,QSqlRelation("carinfo","Cid","Cname"));//通过Cname获取Cname字段数据
    rmod->setRelation(4,QSqlRelation("driverinfo","Did","Dname"));//通过Did获取Dname字段数据
    rmod->setRelation(5,QSqlRelation("driverinfo","Did","Dstatus"));//通过Dstatus获取Dstatus字段数据
    rmod->select();//查询数据
    ui->tableView->hideColumn(0);//隐藏Rid行
    ui->tableView->hideColumn(1);//隐藏Cid行
    ui->tableView->hideColumn(2);//隐藏Did行

    //statusbar里面加入进度条
    pbar = new QProgressBar(ui->statusBar);
    pbar->setRange(0,100);
    pbar->setFixedSize(160,20);
    ui->statusBar->addPermanentWidget(pbar);
    connect(ui->mapWidget,SIGNAL(DocumentComplete(IDispatch*,QVariant&)),this, //浏览器读取完毕后发出信号
            SLOT(sltNavigateComplete2(IDispatch*,QVariant&)));                                      //槽函数执行操作
    connect(pbar,&QProgressBar::valueChanged,pbar,[=](){
        if(pbar->value()==100){
            pbar->reset();//进度条满了就清零
        }
    });


    //模拟数据获取，从JS端获取数据
    connect(conn,&ConToJS::sig_sendDistance,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i)
            distance.push_back(arr.at(i).toInt());
    });

    connect(conn,&ConToJS::sig_sendOrientation,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i)
            orientation.push_back(arr.at(i).toString());
    });

    connect(conn,&ConToJS::sig_sendStart,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i){
            QString lng,lat,res;
            lng = QString::number(arr.at(i).toObject().value("lng").toDouble(),10,6);
            lat = QString::number(arr.at(i).toObject().value("lat").toDouble(),10,6);
            res=lng+","+lat;//构造一个“lng，lat”样式的字符串
            start_lnglat.push_back(res);
        }
    });

    connect(conn,&ConToJS::sig_sendEnd,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i){
            QString lng,lat,res;
            lng = QString::number(arr.at(i).toObject().value("lng").toDouble(),10,6);
            lat = QString::number(arr.at(i).toObject().value("lat").toDouble(),10,6);
            res=lng+","+lat;//构造一个“lng，lat”样式的字符串
            end_lnglat.push_back(res);
        }
    });

    connect(conn,&ConToJS::sig_sendTime,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i)
            times.push_back(arr.at(i).toInt());
    });

    connect(conn,&ConToJS::sig_sendLocation,this,[=](QJsonArray arr){
        for(int i = 0 ; i<arr.size(); ++i){
            location.push_back(arr.at(i).toString());
        }
        locationOk();//发送sig_locationInit信号
    });

    connect(conn,&ConToJS::sig_initDone,this,[=](){//收到数组初始化成功的信号
        int size = start_lnglat.size();
        timer->start(msec);//小车前进速度，单位msec
        curIndex = 0;
        success();//修改UIlabel的内容
        //qDebug()<<"数组存在："<<size<<"条数据！";
        connect(timer,&QTimer::timeout,this,[=](){
            //插入了除了终点的其他数据
            if(curIndex!=size){
                //获取经纬度和方向数据
                //qDebug()<<orientation[i]<<"  "<<start_lnglat[i]<<"  "<<end_lnglat[i];
                auto lnglat = start_lnglat[curIndex].split(",");
                auto lng = lnglat[0];
                auto lat = lnglat[1];

                //获取速度数据，并转换为字符串
                double tmp = distance[curIndex]/times[curIndex]*3.6;//转化为km/h
                QString speed = QString::number(tmp,10,1);//转换为字符串，保留1位小数
                speed += " km/h";//加上单位描述

                //获取地理位置数据，暂时保存在JS端
                conn->regeo(lng,lat);

                //插入了除了终点的其他数据
                insert(CarID , DriverID, speed, orientation[curIndex], lng, lat,"");
                rmod->select();
                ++curIndex;
            }
            //插入终点数据
            if(curIndex==size){
                auto last = end_lnglat[size-1].split(",");
                auto lng = last[0];
                auto lat = last[1];
                //获取地理位置数据，暂时保存在JS端
                conn->regeo(lng,lat);

                //获取速度数据，并转换为字符串
                double tmp = distance.back()/times.back()*3.6;//转化为km/h
                QString speed = QString::number(tmp,10,1);//转换为字符串，保留1位小数
                speed += " km/h";//加上单位描述

                //插入终点数据
                insert(CarID,DriverID,speed,orientation.back(),lng,lat,"");
                timer->stop();//停止计时，防止重复插入
                rmod->select();
                qDebug()<<"数据插入完毕，一共插入"<<size+1<<"条数据。";
                conn->insertDone();//发送插入数据完毕信号，准备更新地理位置
                curIndex = 0;//下标归零
            }
        });
    });

    connect(this,&MainWindow::sig_locationInit,this,[=](){
        updateInfo();
    });

    //按钮实现
    //选择车辆获得模拟数据
    connect(ui->but_start,&QPushButton::clicked,this,[=](){
        moni = new mnDialog;
        connect(moni,&mnDialog::sig_selectData,this,[=](int Cid,int Did){
            //qDebug()<<"收到Cid: "<<Cid<<"收到Did:"<<Did;
            CarID = Cid;//给全部变量赋值
            DriverID = Did;
            qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));//设置随机数种子
            //返回a~b间的值,则为：a + qrand() % (b - a)
            double a = qrand()%99;//返回0-100随机数，除以1000，获得0.000~0.100的随机值
            double b = qrand()%99;
            double c = qrand()%99;
            double d = qrand()%99;
            a/=1000; b/=1000; c/=1000; d/=1000;
            //qDebug()<<a<<" "<<b<<" "<<c<<" "<<d;
            //为经纬度加上随机偏移量
            QString start_lng = QString::number((lng1.toDouble()+a),10,6);
            QString start_lat = QString::number((lat1.toDouble()+b),10,6);
            QString end_lng = QString::number((lng2.toDouble()-c),10,6);
            QString end_lat = QString::number((lat2.toDouble()-d),10,6);
            qDebug()<<"本次模拟随机起点经纬度是："<<start_lng<<","<<start_lat<<"终点经纬度是："<<end_lng<<","<<end_lat;
            conn->simulate(start_lng,start_lat,end_lng,end_lat);//向JS端请求获取5个数组的数据
            //正常情况下，这时候5个数组都非空了
            rmod->select();
        });
        connect(moni,&mnDialog::sig_setTime,this,[=](int time){
            msec = time;
        });
        moni->exec();
    });

    connect(conn,&ConToJS::sig_error,this,[=](){
        ui->status->setText("当前状态：JS端路径规划错误");
    });

    connect(this,&MainWindow::sig_success,this,[=](){
        ui->status->setText("当前状态：准备获取数据");
    });

    connect(conn,&ConToJS::sig_updateLabel,this,[=](QString lnglat){
        ui->status->setText("点击坐标为："+lnglat);
    });

    //与工具栏的擦除按钮相同功能
    connect(ui->but_clean,&QPushButton::clicked,this,[=](){
        act_draw->setDisabled(true);
        act_checkdis->setDisabled(true);
        conn->erase();
        //数组归位
        Markers->clear();
        cur = 0;
        ui->status->setText("当前状态：无");
        rmod->select();
    });

    //toolbar动态图
    QMovie *gif = new QMovie(":/Menu/IconRes/CarGIF.gif");
    QLabel *cargif = new QLabel;
    gif->start();
    cargif->setMovie(gif);
    cargif->setFixedSize(300,40);
    ui->mainToolBar->addWidget(cargif);

    //listwidget列表区
    connect(this,&MainWindow::sig_updaterecv,this,[=](){
        if(20==ui->listWidget->count())
            ui->listWidget->clear();//每20条数据自动刷新
        QDateTime curtime;
        ui->listWidget->addItem(QString("监控数据更新："+curtime.currentDateTime().toString()));
    });
}

MainWindow::~MainWindow()
{
    delete ui;
}

