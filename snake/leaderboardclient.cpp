#include "LeaderboardClient.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

void LeaderboardClient::setServerUrl(const QString &url)
{
    serverUrl = url;
}

LeaderboardClient::LeaderboardClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void LeaderboardClient::submitScore(const QString &playerName, int score)
{
    QUrl url(serverUrl + "/submit");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject data;
    data["name"] = playerName;
    data["score"] = score;

    QNetworkReply *reply = manager->post(request, QJsonDocument(data).toJson());
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onSubmitReply(reply); });
}

void LeaderboardClient::fetchLeaderboard()
{
    QUrl url(serverUrl + "/leaderboard");
    QNetworkRequest request(url);
    QNetworkReply *reply = manager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply]() { onLeaderboardReply(reply); });
}

void LeaderboardClient::onSubmitReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        QJsonObject obj = doc.object();
        if (obj.contains("rank"))
            emit scoreSubmitted(obj["rank"].toInt());
        else
            emit scoreSubmitFailed("服务器返回异常");
    } else {
        emit scoreSubmitFailed(reply->errorString());
    }
    reply->deleteLater();
}

void LeaderboardClient::onLeaderboardReply(QNetworkReply *reply)
{
    if (reply->error() == QNetworkReply::NoError) {
        QJsonDocument doc = QJsonDocument::fromJson(reply->readAll());
        if (doc.isArray())
            emit leaderboardReceived(doc.array());
        else
            emit leaderboardFetchFailed("格式错误");
    } else {
        emit leaderboardFetchFailed(reply->errorString());
    }
    reply->deleteLater();
}
