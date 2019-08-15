#ifndef THEME_H
#define THEME_H

#include <QWidget>
#include<QString>
#include<QIcon>
namespace Ui {
class Theme;
}

class Theme : public QWidget
{
    Q_OBJECT

public:
    explicit Theme(QWidget *parent = nullptr);
    ~Theme();

signals:
    void sigsendcolor(QString color);

public slots:
    void sendcolor(){
        emit sigsendcolor(color);
    }

private:
    Ui::Theme *ui;
    QString color;
};

#endif // THEME_H
