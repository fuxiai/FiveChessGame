#include "chessboard.h"

ChessBoard::ChessBoard(QWidget *parent) : QWidget(parent)
{
    init();
}

int ChessBoard::getChequerSide()
{
    return chequerSide;
}

int ChessBoard::getchequerNumOfLine()
{
    return chequerNumOfLine;
}

ChessBoard::ChessType ChessBoard::getChessInfo(int x, int y)
{
    return chessInfo[x][y];
}

bool ChessBoard::canDrop()
{
    return dropedFlag;
}

void ChessBoard::setDropFlag(bool enable)
{
    dropedFlag = enable;
}

void ChessBoard::setChessInfo(int x, int y, ChessType type)
{
    chessInfo[x][y] = type;
}

void ChessBoard::setCurrentChess(ChessBoard::ChessType chess)
{
    currentChess = chess;
}

void ChessBoard::paintEvent(QPaintEvent *e)
{
    QPixmap chessBoardPic(QString(":/images/chessBoard.jpg"));
    QPixmap blackChessPic(QString(":/images/black.png"));
    QPixmap whiteChessPic(QString(":/images/white.png"));
    QPainter painter(this);
    QPen pen;
    pen.setColor(Qt::black);
    pen.setWidth(2);
    pen.setStyle(Qt::SolidLine);
    painter.setPen(pen);
    painter.drawPixmap(0, 0, this->width(), this->height(), chessBoardPic);

    for(int i = 0; i < chequerNumOfLine; i++) {
        for(int j = 0; j < chequerNumOfLine; j++) {
            painter.drawLine(topW, topH + j * chequerSide,
                             topW + (chequerNumOfLine-1) * chequerSide, topH + j * chequerSide);
            painter.drawLine(topW + j * chequerSide, topH,
                             topW + j * chequerSide, topH + (chequerNumOfLine-1) * chequerSide);
        }
    }
    for(int i = 0; i < chequerNumOfLine; i++) {
        for(int j = 0; j < chequerNumOfLine; j++) {
            if (chessInfo[i][j] == blackChess) {
                painter.drawPixmap(topW + i * chequerSide - chessSide/2, topH + j * chequerSide - chessSide/2,
                                   chessSide, chessSide, blackChessPic);
            }
            if (chessInfo[i][j] == whiteChess) {
                painter.drawPixmap(topW + i * chequerSide - chessSide/2, topH + j * chequerSide - chessSide/2,
                                   chessSide, chessSide, whiteChessPic);
            }
        }
    }
    //this->update();
}

void ChessBoard::mouseMoveEvent(QMouseEvent *e)
{
    // 去除边界无效跟踪
    if ((e->x() < (topW - rangeCenter) || e->x() > (topW * 2 + chequerSide * (chequerNumOfLine-1) - rangeCenter)) ||
         e->y() < (topH - rangeCenter) || e->y() > (topH * 2 + chequerSide * (chequerNumOfLine-1) - rangeCenter)  ) {
        this->setCursor(Qt::ArrowCursor);
        dropChessEnable = false;
        return;
    }
    // 根据每个落棋点改变光标格式
    int inChequerSideX = (e->x() - topW) % chequerSide;
    int inChequerSideY = (e->y() - topH) % chequerSide;
    dropW = (e->x() - topW) / chequerSide;
    dropH = (e->y() - topH) / chequerSide;
    if ((inChequerSideX < rangeCenter || inChequerSideX > (chequerSide - rangeCenter)) &&
        (inChequerSideY < rangeCenter || inChequerSideY > (chequerSide - rangeCenter))) {
        this->setCursor(Qt::PointingHandCursor);
        dropChessEnable = true;
        // 落点位置进一格
        if (inChequerSideX > (chequerSide - rangeCenter)) {
            dropW++;
        }
        if (inChequerSideY > (chequerSide - rangeCenter)) {
            dropH++;
        }
    } else {
        this->setCursor(Qt::ArrowCursor);
        dropChessEnable = false;
    }
}

void ChessBoard::mousePressEvent(QMouseEvent *e)
{
    if (dropChessEnable == false || dropedFlag == false) {
        return;
    }
    if(e->button() == Qt::LeftButton) {
        if (chessInfo[dropW][dropH] == noChess) {
            chessInfo[dropW][dropH] = currentChess;
            emit chessDroped(dropW, dropH);
            dropedFlag = false;
            if(isGameOver(dropW, dropH)) {
                emit boardGameOver();
            }
            this->update();
        }
    }
}

void ChessBoard::init()
{
    topW = 30;
    topH = 30;
    chequerSide = 40;
    chessSide = 40;
    chequerNumOfLine = CHEQUER_NUM;
    rangeCenter = 10;
    dropChessEnable = false;
    dropedFlag = false;
    currentChess = blackChess;
    memset(chessInfo, noChess, sizeof(chessInfo));

    this->setMouseTracking(true);
    this->resize(topW * 2 + (chequerNumOfLine-1) * chequerSide,
                 topH * 2 + (chequerNumOfLine-1) * chequerSide);
    this->setMinimumSize(topW * 2 + (chequerNumOfLine-1) * chequerSide,
                         topH * 2 + (chequerNumOfLine-1) * chequerSide);
    this->setMaximumSize(topW * 2 + (chequerNumOfLine-1) * chequerSide,
                         topH * 2 + (chequerNumOfLine-1) * chequerSide);
}

bool ChessBoard::isGameOver(int x, int y)
{
    bool result = false;
    // 判断行, 参数为判断方向上变化的极性
    result |= chessNumcalAll(-1, 0, 1, 0, x, y);
    // 判断列
    result |= chessNumcalAll(0, -1, 0, 1, x, y);
    // 判断反斜线
    result |= chessNumcalAll(-1, 1, 1, -1, x, y);
    // 判断正斜线
    result |= chessNumcalAll(-1, -1, 1, 1, x, y);
    return result;
}

int ChessBoard::chessNumcalPart(int polarityW, int polarityY, int x, int y)
{
    int val = 0;
    int W;
    int Y;
    dropW = x;
    dropH = y;
    // 当前坐标 dropW dropH
    for (int i = 1; i < 5; i++) {
        W = dropW + i * polarityW;
        Y = dropH + i * polarityY;
        if (W < 0 || Y < 0 || W >= 15 || Y >= 15) {
            break;
        }
        if(chessInfo[W][Y] == currentChess) {
             val++;
        } else {
            break;
        }
    }
    return val;
}

bool ChessBoard::chessNumcalAll(int polarityW1, int polarityY1, int polarityW2, int polarityY2, int x, int y)
{
    int val = 1;
    val += chessNumcalPart(polarityW1, polarityY1, x, y);
    val += chessNumcalPart(polarityW2, polarityY2, x, y);
    if(val >= 5) {
        return true;
    }
    return false;
}
