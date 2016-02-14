#ifndef GAME
#define GAME
#include <QApplication>
#include <QBrush>
#include <QPainter>
#include <QFont>
#include <QMessageBox>
#include <QMenuBar>
#include <QMenu>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMainWindow>
#include <QAction>
#include <QThread>
#include <QLCDNumber>

#include "blockbutton.h"
#include "timer.h"

enum Difficulity
{
    EASY = 0,
    MEDIUM = 1,
    HARD = 2
};


class Game: public QMainWindow
{
    Q_OBJECT
private:
    int mines[30][16];
    BlockButton *blocks[30][16];
    QAction *re_act;
    QAction *playEasy;
    QAction *playMedium;
    QAction *playHard;
    QLCDNumber *remain;
    QLCDNumber *clock;
    Timer *timer;
    QThread *thread;
    Difficulity current_dif;
    Difficulity forward_dif;
    bool is_begin;
    int remain_mines;
    int flags;
    int window_h,window_w;

    void generateBlock(int i,int j);
    bool isWin();
    void generateMine();
    void generateNum();
    int calc_mines(int i,int j);
    void turnOver0(int i,int j);
    void win();
protected:
    void paintEvent(QPaintEvent *);

public:
    Game(QMainWindow *parent = 0);
    ~Game();


signals:
    void bomb();

public slots:
    void restart();
    void gameover();
    void turnOver();
    void flag();
    void setLevel();
    void ontimeOut();
};

#endif // GAME

