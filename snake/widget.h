#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPushButton>
#include <QJsonArray>

#include "gameengine.h"
#include "mediamanager.h"
#include "LeaderboardClient.h"

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    ~Widget();

    void setCurrentUser(const QString &name, const QString &id);

private slots:
    // Menu button slots
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

    // Game engine event handlers
    void onGameWon();
    void onGameLost();

    // Online leaderboard handlers
    void onOnlineRankReceived(const QJsonArray &top10);
    void onOnlineSubmitFailed(const QString &err);

protected:
    void paintEvent(QPaintEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void showEvent(QShowEvent *event) override;

private:
    // Button visibility management
    void showAllMenuBtn();
    void hideAllMenuBtn();
    void showGameOverBtn();
    void hideGameOverBtn();
    void showOptionBtn();
    void hideOptionBtn();
    void showRecordBtn();
    void hideRecordBtn();



    void showOnlineLeaderboard(const QJsonArray &top10);

    // Core components
    GameEngine *m_engine;
    MediaManager *m_media;
    LeaderboardClient *m_onlineClient;

    // Menu buttons
    QPushButton *btnStart, *btnRecord, *btnOnlineRank, *btnDifficulty, *btnExit;
    QPushButton *btnReset, *btnRestart, *btnBackMenu;
    QPushButton *btnEasy, *btnMedium, *btnHard, *btnBack;

    // Image resources
    QPixmap loseImg, rollImg[2], TaffyHead, bgWindow, winImg;
    int rollIndex = 0;
    QTimer *rollTimer;

    // Current user info
    QString currentUserName;
    QString currentUserId;
};

#endif // WIDGET_H
