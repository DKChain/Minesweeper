#include "game.h"

int mines_num[] = {10, 40, 99};

int board[][2] = {{8, 8}, {16, 16}, {30, 16}};

int window_size[][2] = {{240, 340}, {490, 590}, {900, 590}};

const int block_w = 30, block_h = 30;
const int widgets_w = 100;

Game::Game(QMainWindow *parent):
    QMainWindow(parent)
{
    is_begin = false;
    flags = 0;
    current_dif = EASY;
    forward_dif = current_dif;
    remain_mines = mines_num[current_dif];
    window_w = window_size[current_dif][0];
    window_h = window_size[current_dif][1];

    //set menu
    QMenu *menu = menuBar()->addMenu(tr("Level"));

    playEasy = new QAction(tr("Easy"),this);
    connect(playEasy,SIGNAL(triggered()),this,SLOT(setLevel()));
    menu -> addAction(playEasy);

    playMedium = new QAction(tr("Medium"),this);
    connect(playMedium,SIGNAL(triggered()),this,SLOT(setLevel()));
    menu -> addAction(playMedium);

    playHard = new QAction(tr("Hard"),this);
    connect(playHard,SIGNAL(triggered()),this,SLOT(setLevel()));
    menu -> addAction(playHard);


    QMenu *others = menuBar()->addMenu(tr("Others"));
    re_act = new QAction(tr("Restart"),this);
    connect(re_act,SIGNAL(triggered()),this,SLOT(restart()));
    others->addAction(re_act);

    //set LCD to display remain mines counts
    remain = new QLCDNumber(this);
    remain->setGeometry(50,50,60,30);
    remain->setDigitCount(3);
    remain->setSegmentStyle(QLCDNumber::Flat);
    remain->display(mines_num[current_dif] - flags);

    //set LCD to display playtime
    clock = new QLCDNumber(this);
    clock->setGeometry(window_size[current_dif][0]-100,50,60,30);
    clock->setDigitCount(3);
    clock->setSegmentStyle(QLCDNumber::Flat);
    clock->display(0);

    //create a thread,but shouldn't start immediately
    thread = new QThread();

    //timer
    timer = new Timer();
    timer->setInterval(1000);
    timer->moveToThread(thread);
    connect(timer,SIGNAL(timeout()),this,SLOT(ontimeOut()),Qt::DirectConnection);
    connect(thread,SIGNAL(started()),timer,SLOT(start()));
    connect(thread,SIGNAL(finished()),timer,SLOT(stop()));

    //thread->start();

    //initialize array
    for(int i = 0; i < 30; i++)
        for(int j = 0; j < 16; j++)
            mines[i][j] = 0, blocks[i][j] = NULL;

    generateMine();
    generateNum();
    setFixedSize(window_w,window_h);
}

Game::~Game()
{
    for(int i = 0;i < board[current_dif][0]; i++)
        for(int j = 0; j < board[current_dif][1]; j++)
            if(blocks[i][j])
                delete(blocks[i][j]);

    delete(playEasy);
    delete(playMedium);
    delete(playHard);
    delete(re_act);
    delete(remain);
    delete(clock);
    delete(timer);
    delete(thread);
}

//Generate mines and put in array
void Game::generateMine()
{
    int num = mines_num[current_dif];

    while(num)
    {
        int i = rand() % board[current_dif][0];
        int j = rand() % board[current_dif][1];
        if(mines[i][j] == -1)
            continue;
        num--;
        mines[i][j] = -1;
        generateBlock(i,j);
    }
}

//Judge a block
int Game::calc_mines(int i, int j)
{
    if(i < 0 || j < 0)
        return 0;
    if(i >= board[current_dif][0] || j >= board[current_dif][1])
        return 0;

    if(mines[i][j] == -1)
        return 1;
    return 0;
}

//Generate number and put in array
void Game::generateNum()
{
    for(int i = 0;i < board[current_dif][0];i++)
        for(int j = 0; j < board[current_dif][1]; j++)
            if(mines[i][j] == -1)       //if it's a mine block
                continue;
            else                        //count mines around the number block
            {
                mines[i][j] = calc_mines(i-1,j-1) + calc_mines(i-1,j) + calc_mines(i-1,j+1)
                            + calc_mines(i  ,j-1)                     + calc_mines(i  ,j+1)
                            + calc_mines(i+1,j-1) + calc_mines(i+1,j) + calc_mines(i+1,j+1);
                generateBlock(i,j);
            }
}


//Generate all block and display
void Game::generateBlock(int i, int j)
{
    BlockButton *btn = new BlockButton(this);
    connect(btn,SIGNAL(clicked()),this,SLOT(turnOver()));
    connect(btn,SIGNAL(rightClicked()),this,SLOT(flag()));

    btn->setGeometry(block_w*i,block_h*j+widgets_w,block_w,block_h);
    btn->show();

    blocks[i][j] = btn;
}


void Game::restart()
{
    if(thread->isRunning())
        thread->quit();

    is_begin = false;
    clock->display(0);

    for(int i = 0;i < board[forward_dif][0]; i++)
        for(int j = 0; j < board[forward_dif][1]; j++)
            if(blocks[i][j])
                delete(blocks[i][j]);

    for(int i = 0; i < board[current_dif][0]; i++)
        for(int j = 0; j < board[current_dif][1]; j++)
            mines[i][j] = 0, blocks[i][j] = NULL;

    forward_dif = current_dif;

    generateMine();
    generateNum();
    remain_mines = mines_num[current_dif];
    flags = 0;

    remain->setGeometry(50,50,60,30);
    clock->setGeometry(window_size[current_dif][0]-100,50,60,30);

    setFixedSize(window_size[current_dif][0],window_size[current_dif][1]);
    remain->display(mines_num[current_dif] - flags);
}


void Game::gameover()
{
    thread->quit();
    //timer->stop();
    QMessageBox::information(this,"Game Over","BOMB!!!",QMessageBox::Yes,QMessageBox::Yes);
    for(int i = 0;i < board[current_dif][0]; i++)
        for(int j = 0; j < board[current_dif][1]; j++)
            if(blocks[i][j])
            {
                disconnect(blocks[i][j],SIGNAL(clicked(bool)),this,SLOT(turnOver()));
                disconnect(blocks[i][j],SIGNAL(rightClicked()),this,SLOT(flag()));
            }
}

//Turn over a block
void Game::turnOver()
{
    if(!is_begin)
        thread->start();
        //timer->start();

    //Get block which clicked
    BlockButton *btn = qobject_cast<BlockButton*>(sender());

    //if block has flaged,do nothing
    if(!btn->icon().isNull())
        return;

    //get coordinate of block
    QPoint global(btn->mapToParent(QPoint(0,0)));
    int i = global.x() / block_w;
    int j = (global.y() - widgets_w) / block_h;

    //turn over the block
    turnOver0(i,j);

    if(mines[i][j] == -1)
    {
        delete blocks[i][j];
        blocks[i][j] = NULL;
        gameover();
    }

    //if win
    if(!remain_mines)
        win();
}

void Game::paintEvent(QPaintEvent *)
{
    QPainter painter(this);

    for(int i = 0;i < board[current_dif][0];i++)
        for(int j = 0;j < board[current_dif][1];j++)
            if(mines[i][j] > 0)
            {
                painter.setPen(QColor::fromRgb(0,0,0));
                painter.drawText(QRectF(block_w*i,block_h*j+widgets_w,block_w,block_h),Qt::AlignCenter,
                             QString::number(mines[i][j]));
            }
            else if(mines[i][j] == -1)
                painter.drawImage(QRectF(block_w*i,block_h*j+widgets_w,block_w,block_h),QImage("../image/mine.png"));
}

//Turn over block,if block is blank,recur turn over block around it.
void Game::turnOver0(int i, int j)
{
    if(!blocks[i][j])
        return ;
    if(i < 0 || i >= board[current_dif][0])
        return;
    if(j < 0 || j >= board[current_dif][1])
        return;

    //touch a mine
    if(mines[i][j] == -1)
        return;

    if(mines[i][j] == 0)
    {
        delete blocks[i][j];
        blocks[i][j] = NULL;
        turnOver0(i-1,j-1);turnOver0(i-1,j);turnOver0(i-1,j+1);
        turnOver0(i  ,j-1);                 turnOver0(i  ,j+1);
        turnOver0(i+1,j-1);turnOver0(i+1,j);turnOver0(i+1,j+1);
    }

    else
    {
        delete blocks[i][j];
        blocks[i][j] = NULL;
    }

}

//Mark a block
void Game::flag()
{
    BlockButton *btn = qobject_cast<BlockButton*>(sender());
    QPoint global(btn->mapToParent(QPoint(0,0)));
    int i = global.x() / block_w;
    int j = (global.y()-widgets_w) / block_h;

    if(!btn->icon().isNull())
    {
        btn->setIcon(QIcon());
        btn->setText(tr("?"));
        flags--;
        remain->display(mines_num[current_dif] - flags);
        if(mines[i][j] == -1)
            remain_mines++;
    }
    else if(!btn->text().compare(tr("?")))
        btn->setText("");
    else
    {
        btn->setIcon(QIcon("../image/flag.png"));
        flags++;
        remain->display(mines_num[current_dif] - flags);
        if(mines[i][j] == -1)
            remain_mines--;
        if(!remain_mines)
            win();
    }
}

//Change game level
void Game::setLevel()
{
    QAction *act = qobject_cast<QAction*>(sender());

    forward_dif = current_dif;

    if(act == playEasy)
        current_dif = EASY;
    if(act == playMedium)
        current_dif = MEDIUM;
    if(act == playHard)
        current_dif = HARD;

    restart();
}

void Game::ontimeOut()
{
    clock->display(clock->value()+1);
}

void Game::win()
{
    thread->start();
    //timer->stop();
    QMessageBox::information(this,"SUCCESS","Congratulation!!!",QMessageBox::Ok,QMessageBox::Ok);
    for(int i = 0;i < board[current_dif][0]; i++)
        for(int j = 0; j < board[current_dif][1]; j++)
            if(blocks[i][j])
            {
                disconnect(blocks[i][j],SIGNAL(clicked(bool)),this,SLOT(turnOver()));
                disconnect(blocks[i][j],SIGNAL(rightClicked()),this,SLOT(flag()));
            }
}
