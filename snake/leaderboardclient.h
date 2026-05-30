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
    static bool isNetworkError(QNetworkReply::NetworkError err) {
        return err == QNetworkReply::ConnectionRefusedError
            || err == QNetworkReply::HostNotFoundError
            || err == QNetworkReply::TimeoutError;
    }

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
    // 主服务器（Railway 云端）
    static inline QString serverUrl = "https://lively-balance-s.up.railway.app";
    // 备用服务器（ngrok 公网隧道 → 本地 MySQL）
    // 当 Railway 不可用时自动切换
    static inline QString fallbackUrl = "https://eclipse-bridged-credibly.ngrok-free.dev";
    // 当前是否使用备用服务器
    static inline bool useFallback = false;
};
#endif // LEADERBOARDCLIENT_H
