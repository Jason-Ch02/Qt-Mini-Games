#include "widget.h"
#include <QPainter>
#include <QFont>
#include <QKeyEvent>
#include <QDebug>
#include <QPainterPath>
#include <QMessageBox>
#include <QDialog>
#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QJsonObject>
#include <QJsonArray>
#include <QMap>
#include <QCoreApplication>
#include <QFile>
#include <algorithm>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    resize(1000, 700);
    setFocusPolicy(Qt::StrongFocus);

    // ---- Core components ----
    m_engine = new GameEngine(this);
    m_media = new MediaManager(this);
    m_onlineClient = new LeaderboardClient(this);

    // ---- Menu buttons ----
    btnStart = new QPushButton("🎮 开始游戏", this);
    btnRecord = new QPushButton("🏆查看记录", this);
    btnOnlineRank = new QPushButton("🌐在线排行榜", this);
    btnDifficulty = new QPushButton("⚙选择难度", this);
    btnExit = new QPushButton("❌退出游戏", this);
    btnReset = new QPushButton("🔄重置分数", this);
    btnRestart = new QPushButton("🔃重新开始", this);
    btnBackMenu = new QPushButton("🔙返回", this);
    btnEasy = new QPushButton("😊 简单", this);
    btnMedium = new QPushButton("🤔 中等", this);
    btnHard = new QPushButton("😈 困难", this);
    btnBack = new QPushButton("🔙 返回", this);

    // ---- Button style ----
    QString pinkStyle = R"(
    QPushButton {
        background-color: #FFB6C1;
        border: none;
        border-radius: 18px;
        color: #4A2C2C;
        font: bold 16px '微软雅黑';
        padding: 6px 20px;
    }
    QPushButton:hover {
        background-color: #FFC0CB;
        border: 2px solid white;
        color: #5C3A3A;
    }
    QPushButton:pressed {
        background-color: #FF8A9A;
        padding: 8px 18px;
    }
)";
    btnStart->setStyleSheet(pinkStyle);
    btnRecord->setStyleSheet(pinkStyle);
    btnOnlineRank->setStyleSheet(pinkStyle);
    btnDifficulty->setStyleSheet(pinkStyle);
    btnExit->setStyleSheet(pinkStyle);
    btnReset->setStyleSheet(pinkStyle);
    btnRestart->setStyleSheet(pinkStyle);
    btnBackMenu->setStyleSheet(pinkStyle);
    btnEasy->setStyleSheet(pinkStyle);
    btnMedium->setStyleSheet(pinkStyle);
    btnHard->setStyleSheet(pinkStyle);
    btnBack->setStyleSheet(pinkStyle);

    // ---- Button geometry ----
    btnStart->setGeometry(420, 220, 160, 45);
    btnRecord->setGeometry(420, 280, 160, 45);
    btnOnlineRank->setGeometry(420, 340, 160, 45);
    btnDifficulty->setGeometry(420, 400, 160, 45);
    btnExit->setGeometry(420, 460, 160, 45);
    btnReset->setGeometry(420, 520, 160, 45);
    btnRestart->setGeometry(420, 400, 160, 45);
    btnBackMenu->setGeometry(50, 50, 110, 35);
    btnEasy->setGeometry(420, 240, 160, 45);
    btnMedium->setGeometry(420, 300, 160, 45);
    btnHard->setGeometry(420, 360, 160, 45);
    btnBack->setGeometry(50, 50, 110, 35);

    // ---- Button connections ----
    connect(btnStart, &QPushButton::clicked, this, &Widget::startGame);
    connect(btnRecord, &QPushButton::clicked, this, &Widget::showRecord);
    connect(btnOnlineRank, &QPushButton::clicked, this, [this]() { m_onlineClient->fetchLeaderboard(); });
    connect(btnDifficulty, &QPushButton::clicked, this, &Widget::showOption);
    connect(btnExit, &QPushButton::clicked, this, &Widget::exitGame);
    connect(btnReset, &QPushButton::clicked, this, &Widget::resetScore);
    connect(btnRestart, &QPushButton::clicked, this, &Widget::restartGame);
    connect(btnBackMenu, &QPushButton::clicked, this, &Widget::backToMenu);
    connect(btnEasy, &QPushButton::clicked, this, &Widget::setEasy);
    connect(btnMedium, &QPushButton::clicked, this, &Widget::setMedium);
    connect(btnHard, &QPushButton::clicked, this, &Widget::setHard);
    connect(btnBack, &QPushButton::clicked, this, &Widget::backToMenu);

    // ---- Initial visibility ----
    hideGameOverBtn();
    hideOptionBtn();
    hideRecordBtn();
    showAllMenuBtn();

    // ---- Image resources (with fallback) ----
    QString appDir = QCoreApplication::applicationDirPath();

    auto loadOrFallback = [&](QPixmap &pm, const QString &path, const QSize &size, const QColor &color) {
        // 查找图片：exe目录 → 源码目录（开发时）
        QString fullPath = appDir + "/" + path;
        if (!QFile::exists(fullPath)) fullPath = appDir + "/../../" + path;
        if (!QFile::exists(fullPath)) fullPath = appDir + "/../" + path;
        if (!pm.load(fullPath) || pm.isNull()) {
            pm = QPixmap(size);
            pm.fill(color);
        }
    };

    loadOrFallback(bgWindow, "white back.png", QSize(1000, 700), QColor(255, 245, 245));
    loadOrFallback(TaffyHead, "head.png", QSize(48, 48), Qt::red);
    loadOrFallback(loseImg, "lose.png", QSize(120, 120), QColor(200, 50, 50));
    loadOrFallback(rollImg[0], "3.png", QSize(120, 120), QColor(255, 215, 0));
    loadOrFallback(rollImg[1], "4.png", QSize(120, 120), QColor(255, 200, 0));
    winImg = rollImg[0];
    rollIndex = 0;

    rollTimer = new QTimer(this);
    rollTimer->setInterval(100);
    connect(rollTimer, &QTimer::timeout, this, [this]() {
        rollIndex = (rollIndex + 1) % 2;
        winImg = rollImg[rollIndex];
        update();
    });
    rollTimer->start();

    // ---- GameEngine → MediaManager (sound effects) ----
    connect(m_engine, &GameEngine::foodEaten, m_media, &MediaManager::playEatSound);

    // ---- GameEngine → Widget (game events) ----
    connect(m_engine, &GameEngine::gameWon, this, &Widget::onGameWon);
    connect(m_engine, &GameEngine::gameLost, this, &Widget::onGameLost);

    // ---- GameEngine → Widget (redraw on every tick) ----
    connect(m_engine, &GameEngine::ticked, this, [this]() { update(); });

    // ---- LeaderboardClient → Widget ----
    connect(m_onlineClient, &LeaderboardClient::scoreSubmitted, this, [this](int rank) {
        QMessageBox::information(this, "在线排行榜",
                                 QString("您已成功上传分数！\n当前在线排名: %1").arg(rank));
    });
    connect(m_onlineClient, &LeaderboardClient::scoreSubmitFailed, this, [this](const QString &err) {
        qDebug() << "在线提交失败:" << err;
    });
    connect(m_onlineClient, &LeaderboardClient::leaderboardReceived,
            this, &Widget::onOnlineRankReceived);
    connect(m_onlineClient, &LeaderboardClient::leaderboardFetchFailed, this, [this](const QString &err) {
        QMessageBox::warning(this, "获取在线榜失败", err);
    });
}

// ========== Menu button slots ==========

void Widget::startGame()
{
    hideAllMenuBtn();
    m_engine->startGame();
    m_media->stopMenuMusic();
    m_media->startGameMusic();
    m_media->stopAllEffects();
    m_media->showVideo();
    m_media->startVideo();
}

void Widget::showRecord()
{
    hideAllMenuBtn();
    m_engine->setState(GameEngine::RECORD);
    showRecordBtn();
    m_media->hideVideo();
}

void Widget::showOption()
{
    hideAllMenuBtn();
    m_engine->setState(GameEngine::OPTION);
    showOptionBtn();
}

void Widget::exitGame()
{
    close();
}

void Widget::resetScore()
{
    m_engine->resetHighScore();
}

void Widget::restartGame()
{
    hideGameOverBtn();
    m_engine->restartGame();
    m_media->stopAllEffects();
    m_media->showVideo();
    m_media->startVideo();
    m_media->startGameMusic();
}

void Widget::backToMenu()
{
    hideGameOverBtn();
    hideOptionBtn();
    hideRecordBtn();
    showAllMenuBtn();
    m_engine->setState(GameEngine::MENU);
    m_media->showVideo();
    m_media->startVideo();
    m_media->stopAllEffects();
    m_media->stopGameMusic();
    if (m_media->isMusicOn())
        m_media->startMenuMusic();
}

void Widget::setEasy()
{
    m_engine->setSpeed(100);
    backToMenu();
}

void Widget::setMedium()
{
    m_engine->setSpeed(60);
    backToMenu();
}

void Widget::setHard()
{
    m_engine->setSpeed(30);
    backToMenu();
}

// ========== Game event handlers ==========

void Widget::onGameWon()
{
    int score = m_engine->score();
    if (score > 0)
        m_onlineClient->submitScore(currentUserName, score);
    m_media->stopVideo();
    m_media->hideVideo();
    m_media->stopGameMusic();
    m_media->playWinSound();
    showGameOverBtn();
}

void Widget::onGameLost()
{
    int score = m_engine->score();
    if (score > 0)
        m_onlineClient->submitScore(currentUserName, score);
    m_media->stopVideo();
    m_media->hideVideo();
    m_media->stopGameMusic();
    m_media->playGameOverSound();
    showGameOverBtn();
}

// ========== Button visibility ==========

void Widget::showAllMenuBtn()
{
    btnStart->show();
    btnRecord->show();
    btnDifficulty->show();
    btnExit->show();
    btnReset->show();
    btnOnlineRank->show();
}

void Widget::hideAllMenuBtn()
{
    btnStart->hide();
    btnRecord->hide();
    btnDifficulty->hide();
    btnExit->hide();
    btnReset->hide();
    btnOnlineRank->hide();
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

// ========== Online leaderboard ==========

void Widget::onOnlineRankReceived(const QJsonArray &top10)
{
    showOnlineLeaderboard(top10);
}

void Widget::onOnlineSubmitFailed(const QString &err)
{
    QMessageBox::warning(this, "提交失败", "在线分数提交失败: " + err);
}

void Widget::showOnlineLeaderboard(const QJsonArray &top10)
{
    // Deduplicate: keep only the highest score per player name
    QMap<QString, int> bestByPlayer;
    for (int i = 0; i < top10.size(); ++i) {
        QJsonObject entry = top10[i].toObject();
        QString name = entry["name"].toString();
        int score = entry["score"].toInt();
        if (!bestByPlayer.contains(name) || score > bestByPlayer[name])
            bestByPlayer[name] = score;
    }
    // Sort names by score descending
    QList<QString> names = bestByPlayer.keys();
    std::sort(names.begin(), names.end(), [&](const QString &a, const QString &b) {
        return bestByPlayer[a] > bestByPlayer[b];
    });

    QDialog *dlg = new QDialog(this);
    dlg->setWindowTitle("在线排行榜 Top10");
    dlg->setFixedSize(450, 350);
    QVBoxLayout *layout = new QVBoxLayout(dlg);

    QTableWidget *table = new QTableWidget(dlg);
    table->setColumnCount(3);
    table->setHorizontalHeaderLabels({"排名", "玩家", "分数"});
    table->horizontalHeader()->setStretchLastSection(true);
    table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table->verticalHeader()->hide();

    int count = qMin(names.size(), 10);
    table->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        table->setItem(i, 0, new QTableWidgetItem(QString::number(i + 1)));
        table->setItem(i, 1, new QTableWidgetItem(names[i]));
        table->setItem(i, 2, new QTableWidgetItem(QString::number(bestByPlayer[names[i]])));
    }
    layout->addWidget(table);

    QPushButton *closeBtn = new QPushButton("关闭", dlg);
    connect(closeBtn, &QPushButton::clicked, dlg, &QDialog::accept);
    layout->addWidget(closeBtn);
    dlg->setLayout(layout);
    dlg->exec();
    dlg->deleteLater();
}

// ========== User info ==========

void Widget::setCurrentUser(const QString &name, const QString &id)
{
    currentUserName = name;
    currentUserId = id;
    setWindowTitle("塔菲主题贪吃蛇 - 当前用户: " + name);
}

// ========== Qt events ==========

void Widget::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (m_media->isMusicOn() && m_engine->state() == GameEngine::MENU)
        m_media->startMenuMusic();
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (m_engine->state() == GameEngine::PLAYING && !m_engine->isGameOver()) {
        if (e->key() == Qt::Key_K)
            m_engine->forceGameOver();
        if (e->key() == Qt::Key_W)
            m_engine->setDirection(GameEngine::UP);
        if (e->key() == Qt::Key_S)
            m_engine->setDirection(GameEngine::DOWN);
        if (e->key() == Qt::Key_A)
            m_engine->setDirection(GameEngine::LEFT);
        if (e->key() == Qt::Key_D)
            m_engine->setDirection(GameEngine::RIGHT);
    }
}

void Widget::paintEvent(QPaintEvent *e)
{
    Q_UNUSED(e);
    QPainter p(this);
    p.fillRect(rect(), Qt::white);
    p.setFont(QFont("微软雅黑", 12));
    p.drawPixmap(rect(), bgWindow);

    auto state = m_engine->state();

    // ---- MENU ----
    if (state == GameEngine::MENU) {
        QFont font("Comic Sans MS", 30, QFont::Bold);
        p.setFont(font);
        QRect titleRect(50, 80, width() - 100, 80);
        QPen pen(QColor(255, 255, 255, 120), 8);
        p.setPen(pen);
        p.drawText(titleRect, Qt::AlignCenter, "Taffy贪吃蛇大作战");
        p.save();
        QLinearGradient gradient(titleRect.topLeft(), titleRect.bottomRight());
        gradient.setColorAt(0.0, "#FF9A9E");
        gradient.setColorAt(0.5, "#FAD0C4");
        gradient.setColorAt(1.0, "#FBC2EB");
        p.setPen(QPen(QBrush(gradient), 0));
        p.drawText(titleRect, Qt::AlignCenter, "Taffy贪吃蛇大作战");
        p.restore();
    }

    // ---- RECORD ----
    if (state == GameEngine::RECORD) {
        p.setFont(QFont("微软雅黑", 22));
        p.drawText(440, 180, "最高分记录");
        p.drawText(440, 230, "最高分:" + QString::number(m_engine->highScore()));
    }

    // ---- OPTION ----
    if (state == GameEngine::OPTION) {
        p.setFont(QFont("微软雅黑", 22));
        p.drawText(450, 180, "选择难度");
    }

    // ---- PLAYING ----
    if (state == GameEngine::PLAYING) {
        int offset = 50;
        int gameTotalSize = GameEngine::GAME_SIZE * GameEngine::BLOCK_SIZE;
        p.drawRect(offset, offset, gameTotalSize, gameTotalSize);

        const auto &snake = m_engine->snake();
        auto food = m_engine->food();

        p.drawPixmap(
            snake[0].x * GameEngine::BLOCK_SIZE + offset,
            snake[0].y * GameEngine::BLOCK_SIZE + offset,
            GameEngine::BLOCK_SIZE, GameEngine::BLOCK_SIZE,
            TaffyHead);

        p.setBrush(Qt::red);
        for (size_t i = 1; i < snake.size(); i++) {
            p.drawRect(
                snake[i].x * GameEngine::BLOCK_SIZE + offset,
                snake[i].y * GameEngine::BLOCK_SIZE + offset,
                GameEngine::BLOCK_SIZE, GameEngine::BLOCK_SIZE);
        }

        p.setBrush(Qt::yellow);
        p.drawEllipse(
            food.x * GameEngine::BLOCK_SIZE + offset,
            food.y * GameEngine::BLOCK_SIZE + offset,
            GameEngine::BLOCK_SIZE, GameEngine::BLOCK_SIZE);

        p.setFont(QFont("微软雅黑", 22));
        p.drawText(50, 30, "塔菲主题贪吃蛇大作战");
        p.setFont(QFont("微软雅黑", 14));
        p.drawText(50, 650, "分数:" + QString::number(m_engine->score()));
    }

    // ---- GAMEOVER ----
    if (state == GameEngine::GAMEOVER) {
        p.fillRect(0, 0, 1000, 700, QColor(0, 0, 0, 180));
        p.setPen(Qt::white);
        p.setFont(QFont("微软雅黑", 26));
        p.drawPixmap(450, 150, 120, 120, loseImg);
        p.drawText(440, 330, "游戏失败");
        p.drawText(410, 380, "最终得分:" + QString::number(m_engine->score()));
    }

    // ---- VICTORY ----
    if (state == GameEngine::VICTORY) {
        p.fillRect(0, 0, 1000, 700, QColor(0, 0, 0, 180));
        p.setPen(Qt::white);
        p.setFont(QFont("微软雅黑", 26));
        p.drawPixmap(450, 150, 120, 120, winImg);
        p.drawText(440, 330, "游戏胜利");
        p.drawText(410, 380, "最终得分:" + QString::number(m_engine->score()));
    }
}

Widget::~Widget() {}
