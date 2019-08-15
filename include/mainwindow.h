#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

//其他
#include<QDebug>
#include<QAction>
#include<QPalette>
#include<QColor>
#include<QPainter>
#include<QVector>
#include<QDateTime>
#include<QTimer>

//控件类
#include<QAxWidget>
#include<QPushButton>
#include<QMessageBox>
#include<QTreeWidgetItem>
#include<QLineEdit>
#include<QLabel>
#include<QToolBox>
#include<QProgressBar>
#include<QCheckBox>
#include<QMovie>
#include<QSpacerItem>

//自定义窗口控件
#include"about.h"
#include"theme.h"
#include"carsmanager.h"
#include"driversmanager.h"
#include"login.h"
#include"mndialog.h"

//QT和JS通信
#include"websocketclientwrapper.h"
#include"websockettransport.h"
#include"contojs.h"
#include<QWebSocket>
#include<QWebChannel>
#include<QWebSocketServer>
#include<QAxObject>

//连接MYSQL
#include<QSqlDatabase>
#include<QSqlError>
#include<QSqlQuery>
#include<QSqlRelationalTableModel>


namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void initwindow(){
        hide();
        begin = new login;
        if(begin->exec()==QDialog::Accepted){
            begin->success();
            show();
        }
        else exit(0);
    }

    void initconn(){
        //  它仿照QTcpServer建模，并且行为相同。所以，如果你知道如何使用QTcpServer，
        //  你就知道如何使用QWebSocketServer。这个类可以接受传入的WebSocket连接。
        //  您可以指定端口或让QWebSocketServer自动选择一个端口。
        //  您可以收听特定地址或所有机器的地址。调用listen（）让服务器侦听传入的连接。
        server = new QWebSocketServer(QStringLiteral("QWebChannel Server"),QWebSocketServer::NonSecureMode,this);
        if(!server->listen(QHostAddress::LocalHost, 12345)){
            qDebug()<<"listen failed!";
        }
        wrapper = new WebSocketClientWrapper(server,this);

        //建立数据通道
        channel = new QWebChannel(this);
        connect(wrapper, &WebSocketClientWrapper::clientConnected,channel, &QWebChannel::connectTo);
        //注册元数据信息，让JS端可以访问到元数据，第一参数是id(HTML)，第二参数是对象名，
        conn = new ConToJS(this);
        channel->registerObject(QStringLiteral("qtserver") , conn);//注册通道名，在JS端名为qtserver
    }

    void initdb(){
        //数据库mysql装载
        QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
        db.setHostName("127.0.0.1");//localhost
        db.setUserName("root");//数据库登录名
        db.setPassword("root");//密码
        db.setDatabaseName("carsys");//数据库名
        if(false == db.open()){
            QMessageBox::warning(this,"数据库错误",db.lastError().text());
        }
    }

    //信息获取函数
    QString getCarName(int Cid){
        sql->prepare("SELECT DISTINCT Cname FROM carinfo WHERE Cid = :id");
        sql->bindValue(":id",Cid);
        sql->exec();
        if(sql->next())
            return sql->value("Cname").toString();
        else qDebug()<<sql->lastError().text();
        return "";
    }

    QString getDriverName(int Did){
        sql->prepare("SELECT DISTINCT Dname FROM driverinfo WHERE Did = :id");
        sql->bindValue(":id",Did);
        sql->exec();
        if(sql->next()){
            return sql->value("Dname").toString();
        }
        else qDebug()<<sql->lastError();
        return "";
    }

    void getCarinfo(QString Cname){
        sql->prepare("SELECT Cid FROM carinfo WHERE Cname = :name");
        sql->bindValue(":name",Cname);
        sql->exec();
        if(sql->isValid())
            qDebug()<<"查询到结果为："<<sql->value("Cid").toInt();
        else qDebug()<<sql->lastError();
    }

    void clearAll(){
        distance.clear();
        times.clear();
        orientation.clear();
        start_lnglat.clear();
        end_lnglat.clear();
        location.clear();
    }

    bool insert(int Cid,int Did,QString speed,QString direction,QString lng,QString lat,QString address){
        sql->prepare("INSERT INTO roadinfo (Cid,Did,Cname,Dname,Dstatus,Rspeed,Rdirection,Rlng,Rlat,Raddress,Rtime) "
                     "VALUES (:Cid , :Did ,Cid,Did,Did, :speed , :direction , :lng , :lat , :address ,NOW())");
        sql->bindValue(":Cid",Cid);
        sql->bindValue(":Did",Did);
        sql->bindValue(":speed",speed);
        sql->bindValue(":direction",direction);
        sql->bindValue(":lng",lng);
        sql->bindValue(":lat",lat);
        sql->bindValue(":address",address);
        if(sql->exec()){
            updaterecv();//发送sig_updaterecv信号
            return true;
        }
        else {
            qDebug()<<sql->lastError().text();
            return false;
        }
    }

    bool update(QString lng,QString lat,QString address){
        sql->prepare("UPDATE roadinfo SET Raddress = :address where Rlng = :lng and Rlat = :lat");
        sql->bindValue(":lng",lng);
        sql->bindValue(":lat",lat);
        sql->bindValue(":address",address);
        if(sql->exec())
            return true;    
        else {
            qDebug()<<sql->lastError().text();
            return false;
        }
    }

    void updateInfo(){
        int size = start_lnglat.size();
        //qDebug()<<"location数组存在："<<size<<"条数据！";
        for(int i = 0;i<size;++i){
            //插入了除了终点的其他数据
            auto lnglat = start_lnglat[i].split(",");
            auto lng = lnglat[0];
            auto lat = lnglat[1];
            if(i<location.size())
                update(lng,lat,location[i]);
            else
                update(lng,lat,location.back());
            rmod->select();
        }
        //更新终点数据
        auto last = end_lnglat[size-1].split(",");
        auto lng = last[0];
        auto lat = last[1];
        update(lng,lat,location.back());
        rmod->select();
        //清除两端的数组
        clearAll();
        conn->cleanJS();
        qDebug()<<"数据更新完毕：共"<<size+1<<"条数据。所有数组已清空";
    }

signals:
    void sig_locationInit();
    void sig_updaterecv();
    void sig_success();

public slots:
    void sltNavigateComplete2(IDispatch*,QVariant&){
        for(int i=0;i <=100;++i)//一眨眼的动态进度条
            pbar->setValue(i);
    }

    void locationOk(){
        emit sig_locationInit();
    }

    void updaterecv(){
        emit sig_updaterecv();
    }

    void success(){
        emit sig_success();
    }

protected:


private:
    Ui::MainWindow *ui;
    //控件对象
    login *begin;
    QProgressBar *pbar;
    QSqlRelationalTableModel *rmod;
    CarsManager *carm;
    DriversManager *drim;
    QAction *act_draw;
    QAction *act_checkdis;
    QAction *act_eraser;
    mnDialog *moni;
    //连接对象
    QWebSocketServer *server;
    WebSocketClientWrapper *wrapper;
    QWebChannel *channel;
    ConToJS *conn;
    //数据对象
    QSqlQuery *sql;
    QString Markers[1000];
    QVector<int>distance;//存储每条路径的距离
    QVector<int>times;//存储每条路径的预计时间
    QVector<QString>orientation;//存储每条路径的行驶方向
    QVector<QString>start_lnglat;//存储每条路径的起点经纬度
    QVector<QString>end_lnglat;//存储每条路径的终点经纬度
    QVector<QString>location;//存储每条路径的地点，需要二次获取
    QTimer *timer = new QTimer;
    int CarID;//车辆ID
    int DriverID;//驾驶员ID
    int msec;//插入间隔
    int curIndex;//数组当前下标
    //lng1，lat1为广州大学附近经纬度
    QString lng1 = "113.375864";
    QString lat1 = "23.046155";
    //lng2，lat2为体育中心附近经纬度
    QString lng2 = "113.328065";
    QString lat2 = "23.133293";
};

#endif // MAINWINDOW_H
