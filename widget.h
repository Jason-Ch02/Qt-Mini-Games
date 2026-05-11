#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <vector>
using namespace std;

enum State{MENU,PLAYING,RECORD,OPTION,GAMEOVER};
enum Direction{UP,DOWN,LEFT,RIGHT};

struct Point
{
    int x,y;
    Point(int x=0,int y=0):x(x),y(y){}
};

#define BLOCK_SIZE 12
#define GAME_SIZE 45

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget*parent=nullptr);
    ~Widget();

private slots:
    void startGame();
    void showRecord();
    void showOption();
    void exitGame();
    void resetScore();
    void restartGame();
    void backToMenu();
    void setEasy();
    void setMedium();
    void setHard();

protected:
    void paintEvent(QPaintEvent*e)override;
    void keyPressEvent(QKeyEvent*e)override;

private:
    State state;
    bool gameOver;
    int gameSpeed;
    int highScore;
    Direction dir,ndir;
    vector<Point>snake;
    Point food;
    QTimer*timer;

    QPushButton*btnStart;
    QPushButton*btnRecord;
    QPushButton*btnDifficulty;
    QPushButton*btnExit;
    QPushButton*btnReset;

    QPushButton*btnRestart;
    QPushButton*btnBackMenu;
    QPushButton*btnEasy;
    QPushButton*btnMedium;
    QPushButton*btnHard;
    QPushButton*btnBack;

    void showAllMenuBtn();
    void hideAllMenuBtn();
    void showGameOverBtn();
    void hideGameOverBtn();
    void showOptionBtn();
    void hideOptionBtn();
    void showRecordBtn();
    void hideRecordBtn();

    void initGame();
    void gameLoop();
    void MoveSnake(vector<Point>&snake,Point&food,int dir,bool&gameOver,int size);
    int ReadHighScore(const QString&filename);
    void WriteHighScore(int score,const QString&filename);


    //实现背景的设置
    //失败界面
    QPixmap loseImg;
    // 轮播
    QPixmap rollImg[2];
    int rollIndex;
    QTimer* rollTimer;

    //塔菲q版头像
    QPixmap TaffyHead;

    //背景
    QPixmap bgWindow;
};

#endif
