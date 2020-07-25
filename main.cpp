#include "fivechess.h"
#include "chessboard.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    FiveChess *w = new FiveChess();
   // w->running();
    w->init();
    w->show();
    //w->startGame();

    return a.exec();
}
