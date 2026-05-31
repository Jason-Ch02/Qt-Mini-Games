#include "mediamanager.h"
#include <QUrl>
#include <QCoreApplication>
#include <QDir>
#include <QFile>

MediaManager::MediaManager(QWidget *parent)
    : QObject(parent)
{
    QString appDir = QCoreApplication::applicationDirPath();

    // 查找媒体文件：优先 exe 同目录，再查源码目录（开发时 build 目录没有媒体文件）
    auto mediaPath = [&](const QString &filename) -> QString {
        // 1) exe 所在目录（发布时用）
        QString path = appDir + "/" + filename;
        if (QFile::exists(path)) return path;
        // 2) 源码目录（开发时 build/xxx 往上两级）
        path = appDir + "/../../" + filename;
        if (QFile::exists(path)) return path;
        // 3) 源码目录平级（部分构建配置）
        path = appDir + "/../" + filename;
        if (QFile::exists(path)) return path;
        return QString();
    };

    // ---- Video Player ----
    m_videoWidget = new QVideoWidget(parent);
    m_videoWidget->setGeometry(680, 60, static_cast<int>(141.4 * 2.5), static_cast<int>(100 * 2.5));
    m_videoWidget->setAspectRatioMode(Qt::KeepAspectRatio);
    m_videoWidget->hide();

    m_videoAudio = new QAudioOutput(this);
    m_videoAudio->setVolume(0.5);

    m_videoPlayer = new QMediaPlayer(this);
    m_videoPlayer->setAudioOutput(m_videoAudio);
    m_videoPlayer->setVideoOutput(m_videoWidget);

    QString videoPath = mediaPath("xf_20260524142953.mp4");
    if (!videoPath.isEmpty())
        m_videoPlayer->setSource(QUrl::fromLocalFile(videoPath));

    connect(m_videoPlayer, &QMediaPlayer::mediaStatusChanged,
            this, [this](QMediaPlayer::MediaStatus status) {
                if (status == QMediaPlayer::EndOfMedia)
                    m_videoPlayer->play();
            });

    // ---- Menu Music ----
    m_menuMusic = new QMediaPlayer(this);
    m_menuAudio = new QAudioOutput(this);
    m_menuAudio->setVolume(0.05);
    m_menuMusic->setAudioOutput(m_menuAudio);

    QString menuMusicPath = mediaPath("taffy music.mp3");
    if (!menuMusicPath.isEmpty())
        m_menuMusic->setSource(QUrl::fromLocalFile(menuMusicPath));

    connect(m_menuMusic, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && m_musicOn)
            m_menuMusic->play();
    });

    // ---- Game Music ----
    m_gameMusic = new QMediaPlayer(this);
    m_gameAudio = new QAudioOutput(this);
    m_gameAudio->setVolume(0.01);
    m_gameMusic->setAudioOutput(m_gameAudio);

    QString gameMusicPath = mediaPath("taffy music.mp3");
    if (!gameMusicPath.isEmpty())
        m_gameMusic->setSource(QUrl::fromLocalFile(gameMusicPath));

    connect(m_gameMusic, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia && m_musicOn)
            m_gameMusic->play();
    });

    // ---- Eat Sound ----
    m_eatSound = new QMediaPlayer(this);
    m_eatAudio = new QAudioOutput(this);
    m_eatAudio->setVolume(0.5);
    m_eatSound->setAudioOutput(m_eatAudio);

    QString eatPath = mediaPath("food sound.mp3");
    if (!eatPath.isEmpty())
        m_eatSound->setSource(QUrl::fromLocalFile(eatPath));

    // ---- Music Toggle Button ----
    m_musicToggle = new QPushButton("🔊 音乐", parent);
    m_musicToggle->setGeometry(20, 650, 100, 30);
    m_musicToggle->setStyleSheet(
        "QPushButton { background: rgba(0,0,0,100); color: white; border-radius: 10px; }"
        "QPushButton:hover { background: rgba(0,0,0,150); }");
    connect(m_musicToggle, &QPushButton::clicked, this, &MediaManager::toggleMute);

    // ---- Game Over Sound ----
    m_gameOverSound = new QMediaPlayer(this);
    m_gameOverAudio = new QAudioOutput(this);
    m_gameOverAudio->setVolume(0.5);
    m_gameOverSound->setAudioOutput(m_gameOverAudio);

    QString gameOverPath = mediaPath("gameover.mp3");
    if (!gameOverPath.isEmpty())
        m_gameOverSound->setSource(QUrl::fromLocalFile(gameOverPath));

    // ---- Win Sound ----
    m_winSound = new QMediaPlayer(this);
    m_winAudio = new QAudioOutput(this);
    m_winAudio->setVolume(0.5);
    m_winSound->setAudioOutput(m_winAudio);

    QString winPath = mediaPath("win.mp3");
    if (!winPath.isEmpty())
        m_winSound->setSource(QUrl::fromLocalFile(winPath));

    // Start video (if available) muted
    m_videoAudio->setVolume(0.0);
    if (!videoPath.isEmpty()) {
        m_videoPlayer->play();
        m_videoWidget->show();
    }
}

MediaManager::~MediaManager() {}

void MediaManager::startMenuMusic()
{
    if (m_musicOn)
        m_menuMusic->play();
}

void MediaManager::startGameMusic()
{
    if (m_musicOn)
        m_gameMusic->play();
}

void MediaManager::stopMenuMusic()
{
    m_menuMusic->stop();
}

void MediaManager::stopGameMusic()
{
    m_gameMusic->stop();
}

void MediaManager::stopAllMusic()
{
    m_menuMusic->stop();
    m_gameMusic->stop();
}

void MediaManager::playEatSound()
{
    m_eatSound->stop();
    m_eatSound->play();
}

void MediaManager::playGameOverSound()
{
    m_gameOverSound->stop();
    m_gameOverSound->play();
}

void MediaManager::playWinSound()
{
    m_winSound->stop();
    m_winSound->play();
}

void MediaManager::stopAllEffects()
{
    m_gameOverSound->stop();
    m_winSound->stop();
    m_eatSound->stop();
}

void MediaManager::toggleMute()
{
    m_musicOn = !m_musicOn;
    m_musicToggle->setText(m_musicOn ? "🔊 音乐" : "🔇 静音");

    if (!m_musicOn) {
        stopAllMusic();
    }
}

void MediaManager::showVideo()
{
    m_videoWidget->show();
}

void MediaManager::hideVideo()
{
    m_videoWidget->hide();
}

void MediaManager::startVideo()
{
    m_videoPlayer->play();
}

void MediaManager::stopVideo()
{
    m_videoPlayer->stop();
}
