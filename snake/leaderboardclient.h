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
    static inline QString serverUrl = "http://localhost:3000";
};
#endif // LEADERBOARDCLIENT_H
