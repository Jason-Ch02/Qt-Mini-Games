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

void LeaderboardClient::setFallbackUrl(const QString &url)
{
    fallbackUrl = url;
}

QString LeaderboardClient::currentServerUrl()
{
    return useFallback ? fallbackUrl : serverUrl;
}

void LeaderboardClient::trySwitchToFallback()
{
    if (!useFallback && !fallbackUrl.isEmpty()) {
        useFallback = true;
        qDebug() << "⚠ 主服务器不可用，已切换到备用服务器:" << fallbackUrl;
    }
}

LeaderboardClient::LeaderboardClient(QObject *parent) : QObject(parent)
{
    manager = new QNetworkAccessManager(this);
}

void LeaderboardClient::submitScore(const QString &playerName, int score)
{
    QJsonObject data;
    data["name"] = playerName;
    data["score"] = score;
    QByteArray jsonData = QJsonDocument(data).toJson();

    // 同时向主服务器提交
    QUrl primaryUrl(serverUrl + "/submit");
    QNetworkRequest primaryReq(primaryUrl);
    primaryReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    primaryReq.setRawHeader("ngrok-skip-browser-warning", "true");
    QNetworkReply *primaryReply = manager->post(primaryReq, jsonData);
    connect(primaryReply, &QNetworkReply::finished, this, [this, primaryReply]() { onSubmitReply(primaryReply); });

    // 同时向备用服务器提交（双写，保证两边数据一致）
    if (!fallbackUrl.isEmpty() && fallbackUrl != serverUrl) {
        QUrl fallbackU(fallbackUrl + "/submit");
        QNetworkRequest fallbackReq(fallbackU);
        fallbackReq.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
        fallbackReq.setRawHeader("ngrok-skip-browser-warning", "true");
        QNetworkReply *fallbackReply = manager->post(fallbackReq, jsonData);
        connect(fallbackReply, &QNetworkReply::finished, this, [this, fallbackReply]() {
            // 备用服务器静默处理，不影响主流程
            if (fallbackReply->error() != QNetworkReply::NoError) {
                qDebug() << "备用服务器提交失败(可忽略):" << fallbackReply->errorString();
            }
            fallbackReply->deleteLater();
        });
    }
}

void LeaderboardClient::fetchLeaderboard()
{
    QUrl url(currentServerUrl() + "/leaderboard");
    QNetworkRequest request(url);
    // 跳过 ngrok 免费版警告页
    request.setRawHeader("ngrok-skip-browser-warning", "true");
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
        // 任意网络错误自动切换到备用服务器（ngrok）
        if (!useFallback && isNetworkError(reply->error())) {
            trySwitchToFallback();
        }
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
        // 任意网络错误自动切换到备用服务器（ngrok）
        if (!useFallback && isNetworkError(reply->error())) {
            trySwitchToFallback();
        }
        emit leaderboardFetchFailed(reply->errorString());
    }
    reply->deleteLater();
}
