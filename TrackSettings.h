#ifndef TrackSETTINGS_H
#define TrackSETTINGS_H

#include <QWidget>
#include <QDialog>
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QPushButton>
#include <QCloseEvent>
#include <QCheckBox>
#include <QSlider>
#include <QSpinBox>
#include <QColorDialog>
#include <QFileDialog>
#include <QFontDatabase>


#include "Fader.h"

#include "TrackColorButtons.h"
#include "LoopButton.h"
#include "Potentiometer.h"

#include "AudioSampleSelector.h"

class LCDCounter;

enum class EffectType; // in Track.h

class TrackSettings: public QDialog
{
    Q_OBJECT
public:
    explicit TrackSettings(uint8_t volume_percent = 100, bool is_loop = false, std::array<std::array<bool, 4>, 4> beats_per_measure = {{{1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}}, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray, QWidget *parent = nullptr);
    void setLoopState(bool state);
    void setVolume(int volum_percent);
    void setEffectVolume(int volum_percent);
    void setInnerActiveBackgroundColor(QColor color);
    void setOuterBackgroundColor(QColor color);
    void setBeats(std::array<std::array<bool, 4>, 4> beats_per_measure);
    void setIsAudioSampleSelectedState(bool state);
    void setRecordingEnabled(bool enabled);
    bool getRecordingEnabled() const;

private:
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

    QSpinBox* m_volume_display;
    QSpinBox* m_effect_volume_display;
    Fader* m_volume_fader;
    Fader* m_effect_volume_fader;

    TrackColorButtons* m_select_track_colors_button;
    LoopButton* m_loop_button;
    QVBoxLayout* m_beats_matrix_layout;

    QSpinBox* m_lag_whole_takts_display;
    QSpinBox* m_lag_16th_takts_display;
    QSpinBox* m_duration_whole_takts_display;
    QSpinBox* m_duration_16th_takts_display;

    LCDCounter* m_lag_whole_takts_setter;
    LCDCounter* m_lag_16_th_setter;
    LCDCounter* m_duration_whole_takts_setter;
    LCDCounter* m_duration_16_th_setter;

    Potentiometer* m_effects_switcher;

    AudioSampleSelector* m_audio_input_connector;
    std::array<std::array<bool, 4>, 4> m_beats_per_measure;
    std::vector<QMetaObject::Connection> m_current_connections;

    QCheckBox* m_recording_checkbox;

protected:
    void closeEvent(QCloseEvent *event) override;

private slots:
    void changedBeat();

signals:
    void changedLoopState(bool state);
    void changedVolume(int volum_percent);
    void changedInnerActiveBackgroundColor(QColor color);
    void changedOuterBackgroundColor(QColor color);
    void changedAudioSamplePath(QString path);
    void changedBeatState(int index, bool state);

    void changedRecordingEnabled(bool enabled);
};

#endif // TrackSETTINGS_H
