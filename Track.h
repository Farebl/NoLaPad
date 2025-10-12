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


class Track : public QPushButton, public juce::AudioSource
{
    Q_OBJECT
public:
    enum class EffectType {
        None,
        Reverb,
        Delay,
    };

    struct ReverbSettings {
        float roomSize = 0.5f;
        float damping = 0.5f;
        float wetLevel = 0.33f;
        float dryLevel = 0.4f;
    };

    struct DelaySettings {
        float delayTime = 0.2f;
        float feedback = 0.2f;
        float wetLevel = 1.0f;
    };

    explicit Track(QWidget *parent, juce::AudioDeviceManager& device_manager, juce::MixerAudioSource& mixer, MicroTimer* timer = nullptr, float volume = 1.0, bool is_loop = false, std::vector<std::vector<bool>> beats_per_measure = {{1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, QString sound_path = "", const QColor& outer_background_color = Qt::gray, const QColor& inner_active_background_color = Qt::red);

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
    std::vector<std::vector<bool>> getBeats();

    // Recording flag methods
    void setRecordingEnabled(bool enabled);
    bool isRecordingEnabled() const;

    // Beat methods
    void setBeat1(bool state);
    void setBeat2(bool state);
    void setBeat3(bool state);
    void setBeat4(bool state);
    void setBeat5(bool state);
    void setBeat6(bool state);
    void setBeat7(bool state);
    void setBeat8(bool state);
    void setBeat9(bool state);
    void setBeat10(bool state);
    void setBeat11(bool state);
    void setBeat12(bool state);
    void setBeat13(bool state);
    void setBeat14(bool state);
    void setBeat15(bool state);
    void setBeat16(bool state);

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

    // Delay settings methods
    void setDelayTime(float time);
    float getDelayTime();
    void setDelayFeedback(float feedback);
    float getDelayFeedback();
    void setDelayWetLevel(float wet);
    float getDelayWetLevel();

    void play();
    void stop();

    // AudioSource methods
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Getter for effect chain
    juce::dsp::ProcessorChain<
        juce::dsp::Reverb,
        juce::dsp::Phaser<float>,
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>,
        juce::dsp::LadderFilter<float>>& getEffectChain() { return m_effect_chain; }

private:
private:
    std::atomic<bool> m_is_ready{false};
    std::atomic<bool> m_is_being_destroyed{false};
    std::mutex m_audio_mutex;  // Для захисту доступу до аудіо ресурсів

    juce::AudioDeviceManager& m_device_manager;
    juce::MixerAudioSource& m_mixer_source;
    juce::AudioFormatManager m_format_manager;
    juce::AudioTransportSource m_transport_source;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_source;
    juce::dsp::ProcessorChain<
        juce::dsp::Reverb,
        juce::dsp::Phaser<float>,
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>,
        juce::dsp::LadderFilter<float>> m_effect_chain;
    std::unique_ptr<juce::AudioBuffer<float>> m_effect_buffer;

    double m_sample_rate;

    bool m_is_active;
    bool m_is_loop;
    bool m_is_recording_enabled;  // Новий флаг для запису
    QString m_audio_sample_path;
    QString m_style;
    QColor m_outer_color;
    QColor m_inner_color;
    MicroTimer* m_timer;
    std::vector<std::vector<bool>> m_beats_per_measure;
    EffectType m_current_effect;
    ReverbSettings m_reverb_settings;
    DelaySettings m_delay_settings;

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateEffectParameters();

signals:
    void rightClicked(Track* _this);
};

#endif // Track_H
