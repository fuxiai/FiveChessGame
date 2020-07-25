#ifndef CHESSBOARD_H
#define CHESSBOARD_H

#include <QWidget>
#include <QPainter>
#include <QPaintEvent>
#include <QPen>
#include <QDebug>

#define  CHEQUER_NUM   15

class ChessBoard : public QWidget
{
    Q_OBJECT
public:
enum ChessType {noChess, blackChess, whiteChess};
    explicit ChessBoard(QWidget *parent = nullptr);
    void init();
    int getChequerSide();
    int getchequerNumOfLine();
    ChessType getChessInfo(int x, int y);
    bool canDrop();
    void setDropFlag(bool enable);
    void setChessInfo(int x, int y, ChessType type);
    void setCurrentChess(ChessType chess);
    bool isGameOver(int x, int y);
protected:
    void paintEvent(QPaintEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void mousePressEvent(QMouseEvent *e);
signals:
    void boardGameOver();
    void chessDroped(int x, int y);
private:
    int chessNumcalPart(int polarityW, int polarityY, int x, int y);
    bool chessNumcalAll(int polarityW1, int polarityY1, int polarityW2, int polarityY2, int x, int y);
    int topW;
    int topH;
    int chequerSide;
    int chessSide;
    int chequerNumOfLine;   // 棋盘一行格子数
    int rangeCenter;        // 落棋点范围

    bool dropChessEnable;   // 落棋使能
    bool dropedFlag;
    int dropW;              // 落棋行
    int dropH;              // 落棋列

    ChessType currentChess;
    ChessType chessInfo[CHEQUER_NUM][CHEQUER_NUM];
};

#endif // CHESSBOARD_H
