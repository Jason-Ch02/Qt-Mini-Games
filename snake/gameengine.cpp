#include "gameengine.h"
#include <cstdlib>

GameEngine::GameEngine(QObject *parent)
    : QObject(parent)
{
    m_highScore = 0;

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &GameEngine::tick);
    m_timer->start(m_gameSpeed);
}

void GameEngine::startGame()
{
    m_state = PLAYING;
    initGame();
}

void GameEngine::restartGame()
{
    m_state = PLAYING;
    initGame();
    m_gameOver = false;
}

void GameEngine::forceGameOver()
{
    if (m_state == PLAYING && !m_gameOver)
        m_gameOver = true;
}

void GameEngine::setDirection(Direction d)
{
    if (m_state != PLAYING || m_gameOver)
        return;

    if (d == UP && m_dir == DOWN) return;
    if (d == DOWN && m_dir == UP) return;
    if (d == LEFT && m_dir == RIGHT) return;
    if (d == RIGHT && m_dir == LEFT) return;

    m_ndir = d;
}

void GameEngine::setSpeed(int ms)
{
    m_gameSpeed = ms;
    m_timer->start(m_gameSpeed);
}

void GameEngine::resetHighScore()
{
    m_highScore = 0;
}

void GameEngine::initGame()
{
    m_snake.clear();
    m_snake.push_back(Point(GAME_SIZE / 2, GAME_SIZE / 2));
    m_dir = RIGHT;
    m_ndir = RIGHT;
    m_gameOver = false;
    generateFood();
}

void GameEngine::generateFood()
{
    m_food.x = std::rand() % GAME_SIZE;
    m_food.y = std::rand() % GAME_SIZE;
    while (true) {
        bool ok = true;
        for (auto &p : m_snake)
            if (p.x == m_food.x && p.y == m_food.y)
                ok = false;
        if (ok) break;
        m_food.x = std::rand() % GAME_SIZE;
        m_food.y = std::rand() % GAME_SIZE;
    }
}

void GameEngine::tick()
{
    if (m_state == PLAYING && !m_gameOver) {
        m_dir = m_ndir;
        moveSnake();

        if (static_cast<int>(m_snake.size()) == GAME_SIZE * GAME_SIZE) {
            updateHighScore();
            m_state = VICTORY;
            emit gameWon();
            emit ticked();
            return;
        }
    }

    if (m_gameOver && m_state == PLAYING) {
        updateHighScore();
        m_state = GAMEOVER;
        emit gameLost();
    }

    emit ticked();
}

void GameEngine::moveSnake()
{
    Point newHead = m_snake[0];
    if (m_dir == UP) newHead.y--;
    if (m_dir == DOWN) newHead.y++;
    if (m_dir == LEFT) newHead.x--;
    if (m_dir == RIGHT) newHead.x++;

    if (newHead.x < 0 || newHead.x >= GAME_SIZE || newHead.y < 0 || newHead.y >= GAME_SIZE) {
        m_gameOver = true;
        return;
    }

    for (auto &p : m_snake) {
        if (p.x == newHead.x && p.y == newHead.y) {
            m_gameOver = true;
            return;
        }
    }

    m_snake.insert(m_snake.begin(), newHead);

    if (newHead.x == m_food.x && newHead.y == m_food.y) {
        emit foodEaten();
        if (m_snake.size() != GAME_SIZE * GAME_SIZE)
            generateFood();
    } else {
        m_snake.pop_back();
    }
}

void GameEngine::updateHighScore()
{
    int now = score();
    if (now > m_highScore) {
        m_highScore = now;
    }
}
