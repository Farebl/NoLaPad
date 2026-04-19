#ifndef Track_H
#define Track_H

#include <QPushButton>
#include <QTimer>
#include <QPainter>
#include <QMouseEvent>
#include <QThread>
#include <QMessageBox>
#include "MicroTimer.h"

#include <juce_dsp/juce_dsp.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include <mutex>
#include <atomic>
#include <array>


#include "ITrackPlayer.h"

class Track : public QPushButton//, public juce::AudioSource
{
    Q_OBJECT

private:

    std::unique_ptr<ITrackPlayer> m_player;

    bool m_is_active;
    bool m_is_loop;
    bool m_is_recording_enabled;  // Новий флаг для запису
    QString m_audio_sample_path;
    QString m_style;
    QColor m_outer_color;
    QColor m_inner_color;
    MicroTimer* m_timer;
    std::array<bool, 16> m_beats_per_measure;


    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateEffectParameters();

public:

    explicit Track(

        MicroTimer* timer = nullptr,
        float volume = 1.0,
        bool is_loop = false,
        std::array<bool, 16> beats_per_measure = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        QString sound_path = "",
        const QColor& outer_background_color = Qt::gray,
        const QColor& inner_active_background_color = Qt::red,
        QWidget *parent = nullptr
    );

    ~Track();

    // Volume methods
    void setVolume(float volume);
    float getVolume();

    void setAudioSamplePath(QString path);
    QString getAudioSamplePath();

    // Loop and color methods
    void setLoopState(bool state);
    bool getLoopState();
    void setInnerActiveBackgroundColor(QColor color);
    QColor getInnerActiveBackgroundColor();
    void setOuterBackgroundColor(QColor color);
    QColor getOuterBackgroundColor();
    QString getSoundPath();
    std::array<bool, 16> getBeatsStates();

    // Recording flag methods
    void setRecordingEnabled(bool enabled);
    bool isRecordingEnabled() const;

    // Beat methods
    void setBeatState(quint8 index, bool state);

    // Effect type methods
    void setEffectType(EffectType type);
    EffectType getEffectType();

    // Reverb settings methods
    void setReverbRoomSize(float size);
    float getReverbRoomSize();
    void setReverbDamping(float damping);
    float getReverbDamping();
    void setReverbWetLevel(float wet);
    float getReverbWetLevel();
    void setReverbDryLevel(float dry);
    float getReverbDryLevel();
    void setReverbOutputVolume(float volume);
    float getReverbOutputVolume();


    // Delay settings methods
    void setDelayTime(float time);
    float getDelayTime();
    void setDelayFeedback(float feedback);
    float getDelayFeedback();
    void setDelayMixLevel(float mix);
    float getDelayMixLevel();
    void setDelayOutputVolume(float volume);
    float getDelayOutputVolume();


    // Chorus settings methods
    void setChorusRate(float rate);
    float getChorusRate();
    void setChorusDepth(float depth);
    float getChorusDepth();
    void setChorusCenterDelay(float delay);
    float getChorusCenterDelay();
    void setChorusFeedback(float feedback);
    float getChorusFeedback();
    void setChorusMix(float mix);
    float getChorusMix();
    void setChorusOutputVolume(float volume);
    float getChorusOutputVolume();

    // Distortion settings methods
    void setDistortionDrive(float drive);
    float getDistortionDrive();
    void setDistortionMix(float mix);
    float getDistortionMix();
    void setDistortionOutputVolume(float volume);
    float getDistortionOutputVolume();


    void play();
    void stop();

    ITrackPlayer* getPlayer();

signals:
    void rightClicked(Track* _this);
};

#endif // Track_H
