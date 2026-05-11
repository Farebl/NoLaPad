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

class Track : public QPushButton
{
    Q_OBJECT

private:
    bool m_is_active;
    bool m_is_loop;
    bool m_is_recording;
    bool m_is_ready;

    qint16 m_whole_tackt_lag;
    qint16 m_whole_tackt_lag_fixed;
    qint16 m_whole_tackt_duration;
    qint16 m_whole_tackt_duration_fixed;

    QString m_audio_sample_path;
    QString m_style;

    QColor m_outer_color;
    QColor m_inner_color;

    MicroTimer* m_timer;

    std::unique_ptr<ITrackPlayer> m_player;

    std::array<bool, 16> m_beats_per_measure;



    void lagCountdownPlayConnect();
    void durationCountdownPlay();

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateEffectParameters() const;

public:

    explicit Track(
        ITrackPlayer* player,
        MicroTimer* timer = nullptr,
        float volume = 1.0,
        bool is_loop = false,
        bool is_recording = false,
        qint16 whole_tackt_lag = 0,
        qint16 whole_tackt_duration = 0,
        std::array<bool, 16> beats_per_measure = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0},
        const QString& sound_path = "",
        const QColor& outer_background_color = Qt::gray,
        const QColor& inner_active_background_color = Qt::red,
        QWidget *parent = nullptr
    );

    ~Track();

    void play();
    void stop();

    void setWholeTacktLag(qint16 value);
    qint16 getWholeTacktLag() const;

    void setWholeTacktDuration(qint16 value);
    qint16 getWholeTacktDuration() const;



    // Volume methods
    void setVolume(float volume);
    float getVolume() const;

    void setAudioSamplePath(const QString& path);
    QString getAudioSamplePath() const;

    // Loop and color methods
    void setLoopState(bool state);
    bool getLoopState() const;
    void setInnerActiveBackgroundColor(QColor color);
    QColor getInnerActiveBackgroundColor() const;
    void setOuterBackgroundColor(QColor color);
    QColor getOuterBackgroundColor() const;
    QString getSoundPath() const;
    std::array<bool, 16> getBeatsStates() const;

    // Recording flag methods
    void setRecordingState(bool enabled);
    bool getRecordingState() const;

    // Beat methods
    void setBeatState(quint8 index, bool state);

    // Effect type methods
    void setCurrentEffectType(EffectType type);
    EffectType getEffectType() const;

    // Reverb settings methods
    void setReverbRoomSize(float size);
    float getReverbRoomSize() const;
    void setReverbDamping(float damping);
    float getReverbDamping() const;
    void setReverbWetLevel(float wet);
    float getReverbWetLevel() const;
    void setReverbDryLevel(float dry);
    float getReverbDryLevel() const;
    void setReverbOutputVolume(float volume);
    float getReverbOutputVolume() const;


    // Delay settings methods
    void setDelayTime(float time);
    float getDelayTime() const;
    void setDelayFeedback(float feedback);
    float getDelayFeedback() const;
    void setDelayMixLevel(float mix);
    float getDelayMixLevel() const;
    void setDelayOutputVolume(float volume);
    float getDelayOutputVolume() const;


    // Chorus settings methods
    void setChorusRate(float rate);
    float getChorusRate() const;
    void setChorusDepth(float depth);
    float getChorusDepth() const;
    void setChorusCenterDelay(float delay);
    float getChorusCenterDelay() const;
    void setChorusFeedback(float feedback);
    float getChorusFeedback() const;
    void setChorusMix(float mix);
    float getChorusMix() const;
    void setChorusOutputVolume(float volume);
    float getChorusOutputVolume() const;

    // Distortion settings methods
    void setDistortionDrive(float drive);
    float getDistortionDrive() const;
    void setDistortionMix(float mix);
    float getDistortionMix() const;
    void setDistortionOutputVolume(float volume);
    float getDistortionOutputVolume() const;


    ITrackPlayer* getPlayer() const;

signals:
    void rightClicked(Track* _this);
};

#endif // Track_H
