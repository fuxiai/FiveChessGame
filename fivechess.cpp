#include "fivechess.h"
#include "ui_fivechess.h"

FiveChess::FiveChess(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::FiveChess)
{
    ui->setupUi(this);

    chessboard = new ChessBoard();
    resultLabel = new QLabel(this);
    progressStepPrompt = new ArcPaint(this);
    timeLine = new QTimeLine(15000,this);
    music = new QMediaPlayer;
    soundList = new QMediaPlaylist;
    soundList->addMedia(QMediaContent(QUrl("qrc:/sound/press.mp3")));
    soundList->addMedia(QMediaContent(QUrl("qrc:/sound/start.mp3")));
    soundList->addMedia(QMediaContent(QUrl("qrc:/sound/clock.mp3")));
    soundList->addMedia(QMediaContent(QUrl("qrc:/sound/win.mp3")));
    soundList->setPlaybackMode(QMediaPlaylist::CurrentItemOnce);
    music->setPlaylist(soundList);

    setWindowFlags( Qt::FramelessWindowHint);

    state = NO_RUNNING;
}

FiveChess::~FiveChess()
{
    delete ui;
    delete soundList;
    delete music;
    delete timeLine;
    delete progressStepPrompt;
    delete resultLabel;
    delete chessboard;
}

void FiveChess::startGame()
{
    while (state != EXIT)
    {
        if (state == START) {
            state = RUNNING;
            running();
        }
        if (state == RESTART) {
            state = RUNNING;
            reStartGame();
        }
        myMSleep(50);
    }
    qDebug() << "game running over!";
}

void FiveChess::running()
{
    qDebug() << "start runing……";
    soundList->setCurrentIndex(1);
    music->play();
    ui->startBtn->setEnabled(false);
    chessboard->setEnabled(true);
    disconnect(ui->startBtn, SIGNAL(clicked()), this, SLOT(on_startBtn_clicked()));

    if (currentRound == player) {
        chessboard->setDropFlag(true);
    }

    while(state == RUNNING) {
        if (gameOverFlag == true) {
            return;
        }
        if (currentRound == computer) {
            pregressPrompt();
            ui->reDropBtn->setEnabled(true);
            chessboard->setCurrentChess(computerChess);
            chessboard->setDropFlag(false);
            myMSleep(qrand()%30 * 100); // 模拟思考时间

            if(reDropenable == true) {
                ui->msgLabel->setText("电脑：同意！！！");
                currentRound = player;
                chessboard->setCurrentChess(playerChess);
                chessboard->setDropFlag(true);
                chessboard->update();
                continue;
            }
            if(computerDropAI2()) {
                GameOver();
                return;
            } else {
                currentRound = player;
            }
            soundList->setCurrentIndex(0);
            music->play();
            computerStep++;
            emit updateComputerStep(computerStep);
            chessboard->setDropFlag(true);
        } else {
            chessboard->setCurrentChess(playerChess);
            ui->reDropBtn->setEnabled(false);
            pregressPrompt();
            while(chessboard->canDrop() == true && state == RUNNING) {
                myMSleep(50);
            }
            if (state != GAME_OVER) {
                soundList->setCurrentIndex(0);
                music->play();
            }

           // progressStepPrompt->setVisible(false);
            reDropenable = false;
            playerStep++;
            emit updatePlayerStep(playerStep);
            chessboard->setDropFlag(false);
            currentRound = computer;
        }
    }
}

void FiveChess::reStartGame()
{
    gameOverPromptFlag = false;
    resultLabel->setVisible(false);
    chessboard->init();
    this->init();
    this->update();
    qDebug() << "restart……";
    running();
}

void FiveChess::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPixmap backGroundPic(QString(":/images/backGround5.png"));
    QPen pen;
    pen.setWidth(3);
    painter.setPen(pen);
    painter.drawPixmap(0,0,this->width(),this->height(),backGroundPic);
    painter.drawRect(0,0,this->width()-1,this->height()-1);
    QPixmap playerPhoto(QString(":/images/player.png"));
    QPixmap computerPhoto(QString(":/images/computer.png"));
    QPixmap titlePhoto(QString(":/images/title.jpg"));
    QPixmap winPic(QString(":/images/win.png"));
    QPixmap failPic(QString(":/images/fail.png"));
    ui->gameTitleLabel->setScaledContents(true);
    ui->gameTitleLabel->setPixmap(titlePhoto);

    ui->playerLabel->setScaledContents(true);
    ui->playerLabel->setPixmap(playerPhoto);
    ui->playerNameLabel->setText("小懒虫");

    ui->computerLabel->setScaledContents(true);
    ui->computerLabel->setPixmap(computerPhoto);
    ui->computerNameLabel->setText("电脑玩家");

    if (gameOverPromptFlag == true && resultLabel != NULL)
    {        
        if(chessboard->isEnabled() == true)
            resultLabel->setVisible(true);
        resultLabel->resize(winPic.width(), winPic.height());
        resultLabel->move(this->width() /2 - resultLabel->width() /2,
                          this->height()/2 - resultLabel->height()/2);
        if(winner == player)resultLabel->setPixmap(winPic);
        else resultLabel->setPixmap(failPic);
        chessboard->setEnabled(false);
    }
    //this->update();
}

void FiveChess::mousePressEvent(QMouseEvent *e)
{
    lastPos = e->globalPos();
    if (e->button() == Qt::LeftButton) {
        if (resultLabel->isVisible() == true) {
            resultLabel->setVisible(false);
        }
    }
}

void FiveChess::mouseMoveEvent(QMouseEvent *e)
{
    int xLen = e->globalX() - lastPos.x();
    int yLen = e->globalY() - lastPos.y();
    lastPos = e->globalPos();
    move(x()+xLen, y()+yLen);
}

void FiveChess::GameOver()
{
    gameOverFlag = true;
    qDebug() << "game is over!!!";
    resultLabel->setVisible(true);
    timeLine->stop();

    if (currentRound == computer) {
        GameOverPrompt(computer);
        ui->msgLabel->setText("电脑获胜！");
        if (computerChess == ChessBoard::blackChess) {
           qDebug() << "win is computer by black chess!!";
        } else {
           qDebug() << "win is computer by white chess!!";
        }
    } else {
        GameOverPrompt(player);
        ui->msgLabel->setText("获得胜利！");
        if (playerChess == ChessBoard::blackChess) {
           qDebug() << "win is player by black chess!!";
        } else {
           qDebug() << "win is player by white chess!!";
        }
    }

    state = GAME_OVER;
    soundList->setCurrentIndex(3);
    music->play();
}

void FiveChess::GameOverPrompt(FiveChess::CurrentRound win)
{
    winner = win;

    gameOverPromptFlag = true;
    progressStepPrompt->setVisible(false);
    this->update();
}

void FiveChess::playerDropPos(int x, int y)
{
    lastChessX = x;
    lastChessY = y;
}

void FiveChess::updateFrame(int frame)
{
    static int i = 0;
    if (frame <= 5)
    {
        soundList->setCurrentIndex(2);
        music->play();
    }
    progressStepPrompt->setCenterText(frame);
   // qDebug() << "fram change!" << i++;
}

void FiveChess::timeValueChanged(qreal value)
{
    progressStepPrompt->setArcAngle(360 * value);
}

void FiveChess::timefinished()
{
    CurrentRound cur = currentRound == player ? computer: player;
    GameOverPrompt(cur);
    progressStepPrompt->setVisible(false);
    chessboard->setDropFlag(false);
    gameOverFlag = true;
    if(currentRound==player) {
        ui->msgLabel->setText("超时失败！");
    } else {
        ui->msgLabel->setText("对方超时！");
    }
}

void FiveChess::init()
{
    qsrand(time(0));
    this->setWindowTitle("人机五子棋");
    if (qrand() % 2 == 1) {
        computerChess = ChessBoard::blackChess;
        playerChess = ChessBoard::whiteChess;
        currentRound = computer;
        chessboard->setDropFlag(false);
        chessboard->setCurrentChess(computerChess);
    } else {
        computerChess = ChessBoard::whiteChess;
        playerChess = ChessBoard::blackChess;
        currentRound = player;
        //chessboard->setDropFlag(true);
        chessboard->setCurrentChess(playerChess);
    }
    gameOverFlag = false;
    gameOverPromptFlag = false;
    computerStep = 0;
    playerStep = 0;
    lastChessX = 0;
    lastChessY = 0;
    reDropenable = false;
    startLoop = true;
    runningFlag = true;
    memset(dropPointInfo, 0, sizeof(struct DropPointInfo));
    emit updateComputerStep(computerStep);
    emit updatePlayerStep(playerStep);
    resultLabel->setVisible(false);
    ui->chessBoardLayout->addWidget(chessboard);

    {
    connect(chessboard, SIGNAL(boardGameOver()), this, SLOT(GameOver()));
    connect(this, SIGNAL(updateComputerStep(int)), ui->computerStepLabel, SLOT(setNum(int)));
    connect(this, SIGNAL(updatePlayerStep(int)), ui->playerStepLabel, SLOT(setNum(int)));
    // timeLine
    connect(timeLine, SIGNAL(frameChanged(int)), this, SLOT(updateFrame(int)));
    connect(timeLine, SIGNAL(valueChanged(qreal)), this, SLOT(timeValueChanged(qreal)));
    connect(timeLine, SIGNAL(finished()), this, SLOT(timefinished()));
    // 获取落子位域坐标
    connect(chessboard, SIGNAL(chessDroped(int, int)),this, SLOT(playerDropPos(int, int)));
    }//connect
    progressStepPrompt->setRadius(ui->computerLabel->width()/2 + 2);
    progressStepPrompt->setVisible(false);

    timeLine->setFrameRange(0, 15);
    timeLine->setDirection(QTimeLine::Backward);
    timeLine->setCurveShape(QTimeLine::LinearCurve);

    ui->reDropBtn->setEnabled(false);
}

bool FiveChess::computerDropAI()
{
    int i, j;
    int range = 3;
    int count = 0;

    while(1) {
        i = qrand() % chessboard->getchequerNumOfLine();
        j = qrand() % chessboard->getchequerNumOfLine();

        if (chessboard->getChessInfo(i, j) == ChessBoard::noChess) {
            if (i < range || i >  chessboard->getchequerNumOfLine() - range) {
                count++;
                if (count > 50){
                    break;
                }
                continue;
            }
            break;
        }
    }
    chessboard->setChessInfo(i, j, computerChess);
    lastChessX = i;
    lastChessY = j;
    chessboard->update();
    if (chessboard->isGameOver(i, j)) {
        return true;
    }
    return false;
}

/*
 * 功能：获取一个方向上的半个部分，返回其个数值与空闲标志(表示断开位置是否可以落子，bit6表示)
 * 参数：polarityX：棋盘x轴极性
 *      polarityY：棋盘y轴极性
 *      x,y : 棋盘坐标值
*/
char FiveChess::dropInfoOneLinePartValue(ChessBoard::ChessType type,
                                         int polarityX, int polarityY, int x, int y)
{
    char val = 0;
    int tmpX = 0;
    int tmpY = 0;
    ChessBoard::ChessType chessType;
    // 当前坐标 dropW dropH
    val |= 0x20;   // 先置非空闲
    for (int i = 1; i < 5; i++) {
        tmpX = x + i * polarityX;
        tmpY = y + i * polarityY;
        if (tmpX < 0 || tmpY < 0 || tmpX >= 15 || tmpY >= 15) {
            val = 0;
            break;
        }
        chessType = chessboard->getChessInfo(tmpX, tmpY);
        if(chessType == type) {
            val++;
        } else if (chessType == ChessBoard::noChess) {
            val &= ~0x20;
            break;
        } else {
            break;
        }
    }
    return val;
}

/*
* 功能:计算每个方向上的值，获取信息
*/
short FiveChess::dropInfoOneLinePart(ChessBoard::ChessType type,
                          int polarityX1, int polarityY1,
                          int polarityX2, int polarityY2,int x, int y)
{
    short info = 0;
    char val = 0;
    char val1 = 0;
    char val2 = 0;
    val1 = dropInfoOneLinePartValue(type, polarityX1, polarityY1, x, y);
    val2 = dropInfoOneLinePartValue(type, polarityX2, polarityY2, x, y);
    val = val1 + val2 + 1;

    if ((val & 0x0f) >= 5) {
        info = 0xf000;
        return info;
    }
    if ((val1&0x20)&&(val2&0x20)) {
        return info;
    }
    switch(val) {
        case 0x4: info = 0x1000; break;
        case 0x3: info = 0x0100; break;
        case 0x24:info = 0x0010; break;
        case 0x23:info = 0x0001; break;
    }
    return info;
}
FiveChess::DropPointInfo FiveChess::dropOneInfo(int x, int y)
{
    DropPointInfo line = {0,0};
    short info = 0;
    info += dropInfoOneLinePart(ChessBoard::blackChess, -1,  1, 1, -1, x, y);
    info += dropInfoOneLinePart(ChessBoard::blackChess, -1, -1, 1,  1, x, y);
    info += dropInfoOneLinePart(ChessBoard::blackChess, -1,  0, 1,  0, x, y);
    info += dropInfoOneLinePart(ChessBoard::blackChess,  0,  1, 0, -1, x, y);
    line.blackFlags = info;
    //qDebug() << "line.blackFlags"<< info;
    info = 0;
    info += dropInfoOneLinePart(ChessBoard::whiteChess, -1,  1, 1, -1, x, y);
    info += dropInfoOneLinePart(ChessBoard::whiteChess, -1, -1, 1,  1, x, y);
    info += dropInfoOneLinePart(ChessBoard::whiteChess, -1,  0, 1,  0, x, y);
    info += dropInfoOneLinePart(ChessBoard::whiteChess,  0,  1, 0, -1, x, y);
    line.whiteFlags = info;
    return line;
}
bool FiveChess::isDropAIOver(int x, int y)
{
    qDebug() << "chessInfo" << x << y;
    chessboard->setChessInfo(x, y, computerChess);
    lastChessX = x;
    lastChessY = y;
    chessboard->update();
    if (chessboard->isGameOver(x, y)) {
        return true;
    }
    return false;
}
/*
*/
bool FiveChess::checkPointInfo(ChessBoard::ChessType type, char info1, int part1, char info2, int part2)
{
    if (computerdropedFlag == true) {
        return false;
    }
    char val1 = 0;
    char val2 = 0;
    for (int i = 0; i < CHEQUER_NUM; i++) {
        for (int j = 0; j < CHEQUER_NUM; j++) {
            if (type == ChessBoard::blackChess) {
                val1 = (dropPointInfo[i][j].blackFlags >> part1*4) & 0x0f;
                val2 = (dropPointInfo[i][j].blackFlags >> part2*4) & 0x0f;

            } else if (type == ChessBoard::whiteChess) {
                val1 = (dropPointInfo[i][j].whiteFlags >> part1*4) & 0x0f;
                val2 = (dropPointInfo[i][j].whiteFlags >> part2*4) & 0x0f;
            }
            if (val1 > info1 && val2 > info2) {
                computerDropingX = i;
                computerDropingY = j;
                computerdropedFlag = true;
                if(isDropAIOver(i,j) == true)
                    return true;
                else
                    return false;
            }
        }
    }

    return false;
}

bool FiveChess::computerDropAI2()
{

    int noChessNum = 0;
    //memset(dropPointInfo, 0, sizeof(struct DropPointInfo));
    for (int i = 0; i < CHEQUER_NUM; i++) {
        for (int j= 0; j < CHEQUER_NUM; j++) {
            dropPointInfo[i][j].blackFlags = 0;
            dropPointInfo[i][j].whiteFlags = 0;
            if (chessboard->getChessInfo(i, j) == ChessBoard::noChess) {
                noChessNum++;
                dropPointInfo[i][j] = dropOneInfo(i, j);
            }
        }
    }
    if (noChessNum == 0) {
        return false;
    }
    computerdropedFlag = false;
    // 五 电脑
    if (checkPointInfo(computerChess, 0x7, 3, 0x7, 3))return true;
    // 五 玩家
    if (checkPointInfo(playerChess,   0x7, 3, 0x7, 3))return true;
    // 四 电脑
    if (checkPointInfo(computerChess, 0x0, 3, 0x0, 3))return true;
    // 四 玩家
    if (checkPointInfo(playerChess,   0x0, 3, 0x0, 3))return true;
    // 四半 + 三 电脑
    if (checkPointInfo(computerChess, 0x0, 1, 0x0, 2))return true;
    // 四半 + 三 玩家
    if (checkPointInfo(playerChess,   0x0, 1, 0x0, 2))return true;
    // 双三 电脑
    if (checkPointInfo(computerChess, 0x0, 2, 0x0, 2))return true;
    // 双三 玩家
    if (checkPointInfo(playerChess,   0x0, 2, 0x0, 2))return true;

    // 未下棋
    if(computerdropedFlag == false) {
        if (computerDropAI() == true) return true;
        return false;
    }

    return false;
}

void FiveChess::pregressPrompt()
{
    progressStepPrompt->setVisible(true);
    if (currentRound == computer) {
        timeLine->stop();
        progressStepPrompt->move(ui->computerLabel->pos()+QPoint(-5,6));
        timeLine->start();
    } else {
        timeLine->stop();
        progressStepPrompt->move(ui->playerLabel->pos()+QPoint(-5,6));
        timeLine->start();
    }
}

void FiveChess::myMSleep(unsigned int msec)
{
    QTime _Timer = QTime::currentTime().addMSecs(msec);
    while( QTime::currentTime() < _Timer )
        QCoreApplication::processEvents(QEventLoop::AllEvents, 100);
}

void FiveChess::on_startBtn_clicked()
{
    //qDebug() << "start……";
    state = START;
    startGame();
}

void FiveChess::on_reStartBtn_clicked()
{
     state = RESTART;
}

void FiveChess::on_reDropBtn_clicked()
{
    soundList->setCurrentIndex(3);
    music->play();
    if (resultLabel->isVisible() == true) {
        return;
    }
    if(currentRound == player) {
        return;
    }
    if (chessboard->getChessInfo(lastChessX,lastChessY) == playerChess) {
        chessboard->setChessInfo(lastChessX,lastChessY, ChessBoard::ChessType::noChess);
        reDropenable = true;
        ui->msgLabel->setText("我：请求悔棋！！！");
    }
}

void FiveChess::on_exitBtn_clicked()
{
    ExitShow d(this);
    d.setModal(true);
    d.show();
    if (d.exec() == QDialog::Rejected) {
        state = EXIT;
        qApp->quit();
        qDebug() << "btn exit!";
    }
}
