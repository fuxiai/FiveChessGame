#ifndef ARCPAINT_H
#define ARCPAINT_H

#include <QWidget>
#include <QPen>
#include <QPainter>
#include <QDebug>
#include <QLabel>
#include <QLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QFont>
#include <QTimeLine>
#include <QMouseEvent>
#include <QTimerEvent>

class ArcPaint : public QWidget
{
    Q_OBJECT

public:
    enum Direction {clockwise = -1, anticlockwise = 1};
    ArcPaint(QWidget *parent = nullptr);
    ~ArcPaint();
    void setRadius(int val); //设置半径
    //设置文本
    void setCenterText(const QString &str); // 字符串
    void setCenterText(int val, const QString &catStr = NULL); // 数字
    void setCenterText(double val, const QString &catStr = NULL, int precision = 2);

    // 设置弧线
    void setArcAngle(int angle, Direction direction = anticlockwise);

    // 获取画家相关参数
    QPen * getPen();
    QFont * getFont();
protected:
    void paintEvent(QPaintEvent *);

private:
    double percent;    // 进度百分比，与current相互影响
    int radius;        // 半径大小
    int arcRangle;
    QPen * arcPen;
    QBrush * penBrush;
    QFont * font;
    QPainter * painter;
    QLabel * centerTextLabel;
    QTimeLine * timeLine;
    QString text;
};
#endif // ARCPAINT_H
