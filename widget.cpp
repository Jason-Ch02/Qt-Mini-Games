#include "widget.h"
#include <QPainter>
#include <QFile>
#include <QTextStream>
#include <QFont>
#include <QKeyEvent>

Widget::Widget(QWidget*parent):QWidget(parent)
{
    // 缩小窗口 保持正方形 紧凑菜单
    //this->setFixedSize(800, 600);
    // 只设初始大小，不固定
    this->resize(1000,700);
    setFocusPolicy(Qt::StrongFocus);

    state=MENU;
    gameSpeed=100;
    gameOver=false;
    highScore=ReadHighScore("highscore.txt");

    timer=new QTimer(this);
    connect(timer,&QTimer::timeout,this,&Widget::gameLoop);
    timer->start(gameSpeed);

    btnStart=new QPushButton("开始游戏",this);
    btnRecord=new QPushButton("查看记录",this);
    btnDifficulty=new QPushButton("选择难度",this);
    btnExit=new QPushButton("退出游戏",this);
    btnReset=new QPushButton("重置分数",this);

    btnRestart=new QPushButton("重新开始",this);
    btnBackMenu=new QPushButton("返回菜单",this);
    btnEasy=new QPushButton("简单",this);
    btnMedium=new QPushButton("中等",this);
    btnHard=new QPushButton("困难",this);
    btnBack=new QPushButton("返回",this);

    // 紧凑菜单按钮位置
    btnStart->setGeometry(420,220,160,45);
    btnRecord->setGeometry(420,280,160,45);
    btnDifficulty->setGeometry(420,340,160,45);
    btnExit->setGeometry(420,400,160,45);
    btnReset->setGeometry(420,460,160,45);

    btnRestart->setGeometry(420,400,160,45);
    btnBackMenu->setGeometry(50,50,110,35);

    btnEasy->setGeometry(420,240,160,45);
    btnMedium->setGeometry(420,300,160,45);
    btnHard->setGeometry(420,360,160,45);
    btnBack->setGeometry(50,50,110,35);


    connect(btnStart,&QPushButton::clicked,this,&Widget::startGame);
    connect(btnRecord,&QPushButton::clicked,this,&Widget::showRecord);
    connect(btnDifficulty,&QPushButton::clicked,this,&Widget::showOption);
    connect(btnExit,&QPushButton::clicked,this,&Widget::exitGame);
    connect(btnReset,&QPushButton::clicked,this,&Widget::resetScore);

    connect(btnRestart,&QPushButton::clicked,this,&Widget::restartGame);
    connect(btnBackMenu,&QPushButton::clicked,this,&Widget::backToMenu);
    connect(btnEasy,&QPushButton::clicked,this,&Widget::setEasy);
    connect(btnMedium,&QPushButton::clicked,this,&Widget::setMedium);
    connect(btnHard,&QPushButton::clicked,this,&Widget::setHard);
    connect(btnBack,&QPushButton::clicked,this,&Widget::backToMenu);

    hideGameOverBtn();
    hideOptionBtn();
    hideRecordBtn();
    showAllMenuBtn();

    //背景
    bgWindow.load("D:/Qtcontent/back.jpg");
    //头像
    TaffyHead.load("D:/Qtcontent/head.png");
    //失败
    loseImg.load("D:/Qtcontent/lose.png");
    // 加载轮播图片
    rollImg[0].load("D:/Qtcontent/1.png");
    rollImg[1].load("D:/Qtcontent/2.png");
    rollIndex = 0;

    // 轮播定时器（500ms切换一次，达到动态效果）
    rollTimer=new QTimer(this);
    rollTimer->setInterval(500); // 0.5秒切换一张，看起来像动图
    connect(rollTimer,&QTimer::timeout,this,[=]()
            {
                rollIndex=(rollIndex+1)%2;
                update();
            });
    rollTimer->start();
}

void Widget::startGame()
{
    hideAllMenuBtn();
    state=PLAYING;
    initGame();
}

void Widget::showRecord()
{
    hideAllMenuBtn();
    state=RECORD;
    showRecordBtn();
}

void Widget::showOption()
{
    hideAllMenuBtn();
    state=OPTION;
    showOptionBtn();
}

void Widget::exitGame()
{
    close();
}

void Widget::resetScore()
{
    highScore=0;
    WriteHighScore(0,"highscore.txt");
}

void Widget::restartGame()
{
    hideGameOverBtn();
    state=PLAYING;
    initGame();
    gameOver=false;
}

void Widget::backToMenu()
{
    hideGameOverBtn();
    hideOptionBtn();
    hideRecordBtn();
    showAllMenuBtn();
    state=MENU;
    gameOver=false;
}

void Widget::setEasy()
{
    gameSpeed=100;
    timer->start(gameSpeed);
    backToMenu();
}

void Widget::setMedium()
{
    gameSpeed=60;
    timer->start(gameSpeed);
    backToMenu();
}

void Widget::setHard()
{
    gameSpeed=30;
    timer->start(gameSpeed);
    backToMenu();
}

void Widget::showAllMenuBtn()
{
    btnStart->show();
    btnRecord->show();
    btnDifficulty->show();
    btnExit->show();
    btnReset->show();
}

void Widget::hideAllMenuBtn()
{
    btnStart->hide();
    btnRecord->hide();
    btnDifficulty->hide();
    btnExit->hide();
    btnReset->hide();
}

void Widget::showGameOverBtn()
{
    btnRestart->show();
    btnBackMenu->show();
}

void Widget::hideGameOverBtn()
{
    btnRestart->hide();
    btnBackMenu->hide();
}

void Widget::showOptionBtn()
{
    btnEasy->show();
    btnMedium->show();
    btnHard->show();
    btnBack->show();
}

void Widget::hideOptionBtn()
{
    btnEasy->hide();
    btnMedium->hide();
    btnHard->hide();
    btnBack->hide();
}

void Widget::showRecordBtn()
{
    btnBack->show();
}

void Widget::hideRecordBtn()
{
    btnBack->hide();
}

void Widget::initGame()
{
    snake.clear();
    snake.push_back(Point(GAME_SIZE/2,GAME_SIZE/2));
    dir=RIGHT;
    ndir=RIGHT;
    gameOver=false;
    // 生成不重叠的食物
    food.x = rand()%GAME_SIZE;
    food.y = rand()%GAME_SIZE;
    while(true)
    {
        bool ok = true;
        for(auto &p : snake)
            if(p.x==food.x && p.y==food.y)
                ok=false;
        if(ok) break;
        food.x = rand()%GAME_SIZE;
        food.y = rand()%GAME_SIZE;
    }
}

void Widget::MoveSnake(vector<Point>&snake,Point&food,int dir,bool&gameOver,int size)
{
    Point newHead=snake[0];
    if(dir==UP)newHead.y--;
    if(dir==DOWN)newHead.y++;
    if(dir==LEFT)newHead.x--;
    if(dir==RIGHT)newHead.x++;

    if(newHead.x<0||newHead.x>=size||newHead.y<0||newHead.y>=size)
    {
        gameOver=true;
        return;
    }

    for(auto&p:snake)
    {
        if(p.x==newHead.x&&p.y==newHead.y)
        {
            gameOver=true;
            return;
        }
    }

    snake.insert(snake.begin(),newHead);
    if(newHead.x==food.x&&newHead.y==food.y)
    {
        // 生成不重叠的食物
        food.x = rand()%GAME_SIZE;
        food.y = rand()%GAME_SIZE;
        while(true)
        {
            bool ok = true;
            for(auto &p : snake)
                if(p.x==food.x && p.y==food.y)
                    ok=false;
            if(ok) break;
            food.x = rand()%GAME_SIZE;
            food.y = rand()%GAME_SIZE;
        }
    }
    else
    {
        snake.pop_back();
    }
}

void Widget::gameLoop()
{
    if(state==PLAYING&&!gameOver)
    {
        dir=ndir;
        MoveSnake(snake,food,dir,gameOver,GAME_SIZE);
    }

    if(gameOver&&state!=GAMEOVER)
    {
        int now=snake.size()-1;
        if(now>highScore)
        {
            highScore=now;
            WriteHighScore(now,"highscore.txt");
        }
        state=GAMEOVER;
        showGameOverBtn();
    }
    update();
}

void Widget::keyPressEvent(QKeyEvent*e)
{
    if(state==PLAYING&&!gameOver)
    {
        if(e->key()==Qt::Key_K)gameOver=true;
        if(e->key()==Qt::Key_W&&dir!=DOWN)ndir=UP;
        if(e->key()==Qt::Key_S&&dir!=UP)ndir=DOWN;
        if(e->key()==Qt::Key_A&&dir!=RIGHT)ndir=LEFT;
        if(e->key()==Qt::Key_D&&dir!=LEFT)ndir=RIGHT;
    }
}

void Widget::paintEvent(QPaintEvent*e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.fillRect(rect(),Qt::white);
    p.setFont(QFont("微软雅黑",12));
    p.drawPixmap(rect(), bgWindow);


    if(state==MENU)
    {
        p.setFont(QFont("微软雅黑",24));
        p.drawText(410,150,"贪吃蛇大作战");
    }

    if(state==RECORD)
    {
        p.setFont(QFont("微软雅黑",22));
        p.drawText(440,180,"最高分记录");
        p.drawText(440,230,"最高分:"+QString::number(highScore));
    }

    if(state==OPTION)
    {
        p.setFont(QFont("微软雅黑",22));
        p.drawText(450,180,"选择难度");
    }

    if(state==PLAYING)
    {
        int offset = 50;
        int gameTotalSize = GAME_SIZE * BLOCK_SIZE;
        p.drawRect(offset, offset, gameTotalSize, gameTotalSize);

        // 1. 塔菲蛇头：直接用 BLOCK_SIZE 大小，和格子一样大
        p.drawPixmap(
            snake[0].x * BLOCK_SIZE + offset,
            snake[0].y * BLOCK_SIZE + offset,
            BLOCK_SIZE,
            BLOCK_SIZE,
            TaffyHead
            );

        // 2. 蛇身：和蛇头一样大，严丝合缝对齐
        p.setBrush(Qt::red);
        for(int i=1;i<snake.size();i++)
        {
            p.drawRect(
                snake[i].x * BLOCK_SIZE + offset,
                snake[i].y * BLOCK_SIZE + offset,
                BLOCK_SIZE,
                BLOCK_SIZE
                );
        }

        // 3. 食物：和格子一样大，蛇头碰到就吃到
        p.setBrush(Qt::yellow);
        p.drawEllipse(
            food.x * BLOCK_SIZE + offset,
            food.y * BLOCK_SIZE + offset,
            BLOCK_SIZE,
            BLOCK_SIZE
            );

        // 文字和轮播图保持不变
        p.setFont(QFont("微软雅黑",22));
        p.drawText(50, 30, "塔菲主题贪吃蛇大作战");
        p.setFont(QFont("微软雅黑",14));
        p.drawText(50, 650, "分数:"+QString::number(snake.size()-1));
        p.drawPixmap(690,120,220,260, rollImg[rollIndex]);
    }

    if(state==GAMEOVER)
    {
        p.fillRect(0,0, 1000,700, QColor(0,0,0,180));
        p.setPen(Qt::white);
        p.setFont(QFont("微软雅黑", 26));

        // 正上方图片
        p.drawPixmap(450, 150, 120, 120, loseImg);

        p.drawText(440, 330, "游戏失败");
        p.drawText(410, 380, "最终得分:"+QString::number(snake.size()-1));
    }
}

int Widget::ReadHighScore(const QString&filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::ReadOnly))
        return 0;
    QTextStream in(&f);
    int s;
    in>>s;
    f.close();
    return s;
}

void Widget::WriteHighScore(int score,const QString&filename)
{
    QFile f(filename);
    if(!f.open(QIODevice::WriteOnly))
        return;
    QTextStream out(&f);
    out<<score;
    f.close();
}

Widget::~Widget()
{

}
