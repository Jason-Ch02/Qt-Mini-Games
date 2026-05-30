#ifndef LEADERBOARDCLIENT_H
#define LEADERBOARDCLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonArray>

class LeaderboardClient : public QObject
{
    Q_OBJECT
public:
    explicit LeaderboardClient(QObject *parent = nullptr);
    void submitScore(const QString &playerName, int score);
    void fetchLeaderboard();
    static void setServerUrl(const QString &url);
    static void setFallbackUrl(const QString &url);
    static QString currentServerUrl();
    static void trySwitchToFallback();

signals:
    void scoreSubmitted(int rank);
    void scoreSubmitFailed(const QString &error);
    void leaderboardReceived(const QJsonArray &top10);
    void leaderboardFetchFailed(const QString &error);

private slots:
    void onSubmitReply(QNetworkReply *reply);
    void onLeaderboardReply(QNetworkReply *reply);

private:
    QNetworkAccessManager *manager;
    // 主服务器（Railway 云端）- 试用期结束后可能不可用
    static inline QString serverUrl = "https://lively-balance-s.up.railway.app";
    // 备用服务器（本地 MySQL）- 主服务器不可用时自动切换
    static inline QString fallbackUrl = "http://localhost:3000";
    // 当前是否使用备用服务器
    static inline bool useFallback = false;
};
#endif // LEADERBOARDCLIENT_H
