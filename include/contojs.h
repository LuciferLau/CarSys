#ifndef CONTOJS_H
#define CONTOJS_H

#include <QObject>
#include<QDebug>
#include<QJsonObject>
#include<QJsonDocument>
#include<QJsonArray>
#include<QJsonObject>

class ConToJS : public QObject
{
    Q_OBJECT
public:
    explicit ConToJS(QObject *parent = nullptr);
    virtual ~ConToJS(){}

signals:
    //发给主窗口的
    void sig_centtoqt(QString lng,QString lat);
    void sig_addMarker(QString marker);
    void sig_updateLabel(QString lnglat);
    void sig_sendOrientation(QJsonArray orientation);//发送方向数组
    void sig_sendDistance(QJsonArray distance);//发送距离数组
    void sig_sendStart(QJsonArray start);//发送路径点起点数组
    void sig_sendEnd(QJsonArray end);//发送路径点终点数组
    void sig_sendTime(QJsonArray time);
    void sig_sendLocation(QJsonArray location);//发送该经纬度的地址
    void sig_initDone();
    void sig_error();

    //发给JS端的
    void sig_locate(double jd,double wd);//定位信号
    void sig_getcent();//获取地图中心点信号
    void sig_mapswitchtraffic();//交通图层信号
    void sig_mapswitchsatellite();//卫星图层信号
    void sig_mapswitchroadnet();//路网图层信号
    void sig_mapswitchtheme(QString color);//切换主题信号
    void sig_drawline();//划线信号，未使用
    void sig_checkdis(QString start_lng,QString start_lat,QString end_lng,QString end_lat);//测距信号
    void sig_erase();//清楚覆盖物信号
    void sig_getroute(QString start_lng,QString start_lat,QString end_lng,QString end_lat);//绘制行车轨迹信号
    void sig_simulate(QString start_lng,QString start_lat,QString end_lng,QString end_lat);//模拟行车轨迹信号
    void sig_regeo(QString lng,QString lat);//逆地理编码信号
    void sig_insertDone();//数据插入完成，准备获取address数组
    void sig_clean();

public slots:

    //Q_INVOKABLE宏声明函数将函数暴露给JS端，使其可以通过webchannel调用
    Q_INVOKABLE void ok(){
        qDebug()<<"C++调用JS成功啦";
    }

    Q_INVOKABLE void debug(QString str){//用于DEBUG在JS端程序的输出
        qDebug()<<str;
    }

    Q_INVOKABLE void showplus(QString lng,QString lat){
        qDebug()<<"地图中心点经度："<<lng<<" 纬度："<<lat;
        emit sig_centtoqt(lng,lat);
    }

    Q_INVOKABLE void mapdebug(QString lng,QString lat){
        QString marker = lng + "," +lat;
        emit sig_updateLabel(marker);
        emit sig_addMarker(marker);
        qDebug()<<"点击坐标为"<<lng<<" "<<lat;
    }

    Q_INVOKABLE void sendStart(QJsonArray arr){
        qDebug()<<"起点数组已接收";
        emit sig_sendStart(arr);
    }

    Q_INVOKABLE void sendEnd(QJsonArray arr){
        qDebug()<<"终点数组已接收";
        emit sig_sendEnd(arr);
    }

    Q_INVOKABLE void sendDistance(QJsonArray arr){
        qDebug()<<"距离数组已接收";
        emit sig_sendDistance(arr);
    }

    Q_INVOKABLE void sendOrientation(QJsonArray arr){
        qDebug()<<"方向数组已接收";
        emit sig_sendOrientation(arr);
    }

    Q_INVOKABLE void sendTime(QJsonArray arr){
        qDebug()<<"时间数组已接收";
        emit sig_sendTime(arr);
    }

    Q_INVOKABLE void sendLocation(QJsonArray arr){
        //qDebug()<<arr;
        emit sig_sendLocation(arr);
    }

    Q_INVOKABLE void initDone(){
        emit sig_initDone();
    }

    Q_INVOKABLE void error(){
        emit sig_error();
    }

    //    Q_INVOKABLE void show(QJsonObject lnglat){   旧函数，新函数调用自带方法进行在JS端转换了数据
    //        QString res = QString(QJsonDocument(lnglat).toJson());//QJSON->QString
    //        //qDebug()<<res;
    //        QString y="lat\":" , x="lng\":";
    //        //qDebug()<<res.lastIndexOf(x)<<"  "<<res.lastIndexOf(y);
    //        QString tmp_lng = res.mid(res.lastIndexOf(x),15);
    //        QString lng = tmp_lng.mid(6);
    //        QString tmp_lat = res.mid(res.lastIndexOf(y),15);
    //        QString lat = tmp_lat.mid(6);
    //        qDebug()<<"地图中心点经度："<<lng<<" 纬度："<<lat;
    //        emit sig_centtoqt(lng,lat);
    //    }

    //QT调用JS中函数
    void locate(double jd,double wd){
        emit sig_locate(jd,wd);
    }

    void getcent(){
        emit sig_getcent();
    }

    void mapswitchtraffic(){
        emit sig_mapswitchtraffic();
    }

    void mapswitchsatellite(){
        emit sig_mapswitchsatellite();
    }

    void mapswitchroadnet(){
        emit sig_mapswitchroadnet();
    }

    void mapswitchtheme(QString color){
        emit sig_mapswitchtheme(color);
    }

    void drawline(){
        emit sig_drawline();
    }

    void checkdis(QString start_lng,QString start_lat,QString end_lng,QString end_lat){
        emit sig_checkdis(start_lng,start_lat,end_lng,end_lat);
    }

    void erase(){
        emit sig_erase();
    }

    void getroute(QString start_lng,QString start_lat,QString end_lng,QString end_lat){
        emit sig_getroute(start_lng,start_lat,end_lng,end_lat);
    }

    void simulate(QString start_lng,QString start_lat,QString end_lng,QString end_lat){
        emit sig_simulate(start_lng,start_lat,end_lng,end_lat);
    }

    void regeo(QString lng,QString lat){
        emit sig_regeo(lng,lat);
    }

    void insertDone(){
        emit sig_insertDone();
    }

    void cleanJS(){
        emit sig_clean();
    }

};

#endif // CONTOJS_H
