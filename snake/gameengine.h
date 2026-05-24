#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <QObject>
#include <QTimer>
#include <vector>

class GameEngine : public QObject
{
    Q_OBJECT

public:
    enum State { MENU, PLAYING, RECORD, OPTION, GAMEOVER, VICTORY };
    enum Direction { UP, DOWN, LEFT, RIGHT };

    struct Point {
        int x, y;
        Point(int x = 0, int y = 0) : x(x), y(y) {}
    };

    static constexpr int BLOCK_SIZE = 12;
    static constexpr int GAME_SIZE = 45;

    explicit GameEngine(QObject *parent = nullptr);

    void startGame();
    void restartGame();
    void forceGameOver();
    void setDirection(Direction d);
    void setSpeed(int ms);
    void resetHighScore();

    State state() const { return m_state; }
    void setState(State s) { m_state = s; }
    bool isGameOver() const { return m_gameOver; }
    int score() const { return static_cast<int>(m_snake.size()) - 1; }
    int highScore() const { return m_highScore; }
    int speed() const { return m_gameSpeed; }
    const std::vector<Point> &snake() const { return m_snake; }
    Point food() const { return m_food; }

signals:
    void foodEaten();
    void gameWon();
    void gameLost();
    void ticked();

private slots:
    void tick();

private:
    void initGame();
    void moveSnake();
    void generateFood();
    void updateHighScore();

    State m_state = MENU;
    bool m_gameOver = false;
    int m_gameSpeed = 100;
    int m_highScore = 0;
    Direction m_dir = RIGHT;
    Direction m_ndir = RIGHT;
    std::vector<Point> m_snake;
    Point m_food;
    QTimer *m_timer;
};

#endif // GAMEENGINE_H
