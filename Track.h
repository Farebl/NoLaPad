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


enum class EffectType {
    None,
    Reverb,
    Chorus,
    Delay,
    Distortion
};


struct ReverbSettings {
    float roomSize = 0.5f;      // Розмір кімнати (характер звуку) [0.0 - 1.0]
    float damping = 0.5f;       // Заглушення (характер звуку) [0.0 - 1.0]
    float wetLevel = 0.33f;     // Скільки реверберації [0.0 - 1.0]
    float dryLevel = 0.4f;      // Скільки оригіналу [0.0 - 1.0]
    float outputVolume = 1.0f;  // Загальна гучність після ефекту [0.0 - 1.0]
};

struct DelaySettings {
    float delayTime = 0.2f;     // Час затримки в секундах (характер ефекту) [0.0 - 2.0]
    float feedback = 0.2f;      // Кількість повторень (характер ефекту) [0.0 - 0.95]
    float mix = 0.5f;           // Баланс dry/wet [0.0 - 1.0]
    float outputVolume = 1.0f;  // Загальна гучність після ефекту [0.0 - 1.0]
};

struct ChorusSettings {
    float rate = 1.0f;          // Швидкість модуляції в Hz (характер звуку) [0.1 - 10.0]
    float depth = 0.25f;        // Глибина модуляції (характер звуку) [0.0 - 1.0]
    float centerDelay = 7.0f;   // Центральна затримка в мс (характер звуку) [1.0 - 100.0]
    float feedback = 0.0f;      // Зворотний зв'язок (характер звуку) [0.0 - 0.9]
    float mix = 0.5f;           // Баланс dry/wet [0.0 - 1.0]
    float outputVolume = 1.0f;  // Загальна гучність після ефекту [0.0 - 1.0]
};

struct DistortionSettings {
    float drive = 10.0f;        // Ступінь спотворення в dB (характер звуку) [0.0 - 40.0]
    float mix = 0.5f;           // Баланс dry/wet [0.0 - 1.0]
    float outputVolume = 0.5f;  // Загальна гучність після ефекту [0.0 - 1.0]
};


class Track : public QPushButton, public juce::AudioSource
{
    Q_OBJECT

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
        juce::dsp::LadderFilter<float>
    > m_effect_chain;
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
    std::array<bool, 16> m_beats_per_measure;
    EffectType m_current_effect;
    ReverbSettings m_reverb_settings;
    DelaySettings m_delay_settings;
    ChorusSettings m_chorus_settings;
    DistortionSettings m_distortion_settings;

    void mousePressEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void updateEffectParameters();

public:

    explicit Track(
        juce::AudioDeviceManager& device_manager,
        juce::MixerAudioSource& mixer,
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

    // AudioSource methodsector
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void releaseResources() override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;

    // Getter for effect chain
    juce::dsp::ProcessorChain<
        juce::dsp::Reverb,
        juce::dsp::Phaser<float>,
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>,
        juce::dsp::LadderFilter<float>
    >& getEffectChain() { return m_effect_chain; }

signals:
    void rightClicked(Track* _this);
};

#endif // Track_H
