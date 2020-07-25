#include "arcpaint.h"

ArcPaint::ArcPaint(QWidget *parent)
    : QWidget(parent)
{
    radius = 20;
    arcRangle = 180;
    this->resize(radius*2, radius*2);
    painter = new QPainter();
    arcPen = new QPen;
    arcPen->setColor(Qt::white);
    arcPen->setStyle(Qt::SolidLine);
    arcPen->setCapStyle(Qt::RoundCap);
    arcPen->setJoinStyle(Qt::RoundJoin);
    //arcPen->setWidth(10);
    arcPen->setWidthF(6);

    font = new QFont();
    font->setPixelSize(60);

    text = "hello";
}

ArcPaint::~ArcPaint()
{
}

void ArcPaint::setRadius(int val)
{
    radius = val;
    this->resize(radius*2, radius*2);
}
void ArcPaint::setCenterText(const QString &str)
{
    text = str;
}

void ArcPaint::setCenterText(int val, const QString &catStr)
{
    text = text.setNum(val) + catStr;
}

void ArcPaint::setCenterText(double val, const QString &catStr, int precision)
{
    text = text.setNum(val, 'f', precision) + catStr;
}

void ArcPaint::setArcAngle(int angle, Direction direction)
{
    arcRangle = angle * direction;
}

void ArcPaint::paintEvent(QPaintEvent *)
{
    painter->begin(this);
    painter->setRenderHint(QPainter::HighQualityAntialiasing, true);
    painter->setPen(*arcPen);
    painter->setFont(*font);

    QRect textRect(0,0,this->width(), this->height());
    painter->drawText(textRect, Qt::AlignCenter, text);

    int penW = arcPen->width();
    painter->drawArc((penW)/2,penW/2,radius*2-penW,radius*2-penW,90*16,arcRangle*16);

    painter->end();
    this->update();
}

QPen *ArcPaint::getPen()
{
    return arcPen;
}

QFont *ArcPaint::getFont()
{
    return font;
}
