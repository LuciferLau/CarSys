#include "theme.h"
#include "ui_theme.h"

Theme::Theme(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Theme)
{
    ui->setupUi(this);
    setWindowTitle("更换地图主题");
    setWindowIcon(QIcon(":/Menu/IconRes/Gear.ico"));
    setFixedSize(300,400);
    connect(ui->b1,&QPushButton::clicked,this,[=](){
        color = "normal";
    });
    connect(ui->b2,&QPushButton::clicked,this,[=](){
        color = "dark";
    });
    connect(ui->b3,&QPushButton::clicked,this,[=](){
        color = "light";
    });
    connect(ui->b4,&QPushButton::clicked,this,[=](){
        color = "whitsmoke";
    });
    connect(ui->b5,&QPushButton::clicked,this,[=](){
        color = "fresh";
    });
    connect(ui->b6,&QPushButton::clicked,this,[=](){
        color = "grey";
    });
    connect(ui->b7,&QPushButton::clicked,this,[=](){
        color = "graffiti";
    });
    connect(ui->b8,&QPushButton::clicked,this,[=](){
        color = "macaron";
    });
    connect(ui->b9,&QPushButton::clicked,this,[=](){
        color = "blue";
    });
    connect(ui->b10,&QPushButton::clicked,this,[=](){
        color = "darkblue";
    });

    connect(ui->pushButton,&QPushButton::clicked,this,[=](){
        sendcolor();//确认就发信号
        close();//然后关闭窗口
    });
    connect(ui->pushButton_2,&QPushButton::clicked,this,&Theme::close);//取消就关闭窗口
}

Theme::~Theme()
{
    delete ui;
}
