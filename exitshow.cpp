#include "exitshow.h"
#include "ui_exitshow.h"

ExitShow::ExitShow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ExitShow)
{
    ui->setupUi(this);
    setWindowFlag(Qt::FramelessWindowHint);
}

ExitShow::~ExitShow()
{
    delete ui;
}

void ExitShow::on_confirmBtn_clicked()
{
    reject(); // 确认
}

void ExitShow::on_cancelBtn_clicked()
{
    accept(); // 取消
}

void ExitShow::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    QFont font;
    QPixmap exitShowPic(QString(":/images/backGround5.png"));
    painter.drawPixmap(0,0, this->width(),this->height(),exitShowPic);
    painter.drawRect(0,0,this->width()-1,this->height()-1);
    font = ui->confirmBtn->font();
    painter.setFont(font);
    QRect textRect(0,0,this->width(), this->height()*2/3);
    painter.drawText(textRect, Qt::AlignCenter, "你确定要退出吗？");
}
