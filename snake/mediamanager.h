#ifndef MEDIAMANAGER_H
#define MEDIAMANAGER_H

#include <QObject>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <QPushButton>

class MediaManager : public QObject
{
    Q_OBJECT

public:
    explicit MediaManager(QWidget *parent);
    ~MediaManager();

    void startMenuMusic();
    void startGameMusic();
    void stopMenuMusic();
    void stopGameMusic();
    void stopAllMusic();
    void playEatSound();
    void playGameOverSound();
    void playWinSound();
    void stopAllEffects();

    void toggleMute();
    bool isMusicOn() const { return m_musicOn; }

    void showVideo();
    void hideVideo();
    void startVideo();
    void stopVideo();

    QPushButton *musicToggleButton() const { return m_musicToggle; }
    QVideoWidget *videoWidget() const { return m_videoWidget; }

private:
    bool m_musicOn = true;

    QVideoWidget *m_videoWidget = nullptr;
    QAudioOutput *m_videoAudio = nullptr;
    QMediaPlayer *m_videoPlayer = nullptr;

    QMediaPlayer *m_menuMusic = nullptr;
    QAudioOutput *m_menuAudio = nullptr;

    QMediaPlayer *m_gameMusic = nullptr;
    QAudioOutput *m_gameAudio = nullptr;

    QMediaPlayer *m_eatSound = nullptr;
    QAudioOutput *m_eatAudio = nullptr;

    QMediaPlayer *m_gameOverSound = nullptr;
    QAudioOutput *m_gameOverAudio = nullptr;

    QMediaPlayer *m_winSound = nullptr;
    QAudioOutput *m_winAudio = nullptr;

    QPushButton *m_musicToggle = nullptr;
};

#endif // MEDIAMANAGER_H
