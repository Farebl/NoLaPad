#ifndef TrackSETTINGS_H
#define TrackSETTINGS_H

#include <QWidget>
#include <QTableWidget>
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


#include "LCDDisplay.h"
#include "Fader.h"

#include "TrackColorButtons.h"
#include "RedButton.h"
#include "EffectsSwitcher.h"

#include "AudioSampleSelector.h"

class LCDCounter;
class QStackedWidget;

// in Track.h:
enum class EffectType;
struct ReverbSettings;
struct DelaySettings;
struct ChorusSettings;
struct DistortionSettings;

class Potentiometer;

class TrackSettings: public QDialog
{
    Q_OBJECT

private:
    LCDDisplay* m_volume_display;
    Fader* m_volume_fader;

    TrackColorButtons* m_select_track_colors_button;
    RedButton* m_loop_button;
    QTableWidget* m_16th_matrix;

    LCDCounter* m_lag_whole_takts_setter;
    LCDCounter* m_lag_16_th_setter;
    LCDCounter* m_duration_whole_takts_setter;
    LCDCounter* m_duration_16_th_setter;

    EffectsSwitcher* m_effects_switcher;

    AudioSampleSelector* m_audio_input_connector;
    std::array<bool, 16> m_beats_per_measure;
    std::vector<QMetaObject::Connection> m_current_connections;

    QCheckBox* m_recording_checkbox;

    QStackedWidget* m_effects_settings_stack_widget;
    LCDDisplay* m_effect_value_display;
    Potentiometer* m_reverb_room_size_potentiometer;
    Potentiometer* m_reverb_damping_potentiometer;
    Potentiometer* m_reverb_wet_level_potentiometer;
    Potentiometer* m_reverb_dry_level_potentiometer;
    Potentiometer* m_reverb_output_volume_potentiometer;

    Potentiometer* m_delay_time_potentiometer;
    Potentiometer* m_delay_feedback_potentiometer;
    Potentiometer* m_delay_mix_potentiometer;
    Potentiometer* m_delay_output_volume_potentiometer;

    Potentiometer* m_chorus_rate_potentiometer;
    Potentiometer* m_chorus_depth_potentiometer;
    Potentiometer* m_chorus_center_delay_potentiometer;
    Potentiometer* m_chorus_feedback_potentiometer;
    Potentiometer* m_chorus_mix_potentiometer;
    Potentiometer* m_chorus_output_volume_potentiometer;

    Potentiometer* m_distortion_drive_potentiometer;
    Potentiometer* m_distortion_mix_potentiometer;
    Potentiometer* m_distortion_output_volume_potentiometer;

    void connectEffectsPotentiometersWithDisplay();

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    explicit TrackSettings(quint8 volume_percent = 100, bool is_loop = false, const std::array<bool, 16>& beats_per_measure = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray, QWidget *parent = nullptr);

    void setLoopState(bool state);
    void setVolume(float volume);
    void setEffectVolume(int volum_percent);
    void setInnerActiveBackgroundColor(QColor color);
    void setOuterBackgroundColor(QColor color);
    void setBeats(std::array<bool, 16> beats_per_measure);
    void setIsAudioSampleSelectedState(bool state);
    void setRecordingEnabled(bool enabled);
    bool getRecordingEnabled() const;


private slots:
    void changedBeat();

signals:
    void changedLoopState(bool state);
    void changedVolume(float volum_percent);
    void changedInnerActiveBackgroundColor(QColor color);
    void changedOuterBackgroundColor(QColor color);
    void changedAudioSamplePath(QString path);
    void changedBeatState(int index, bool state);
    void changedRecordingEnabled(bool enabled);


    // Effects

    void changedEffectType(EffectType);

    void changedReverbRoomSize(int value);
    void changedReverbDamping(int value);
    void changedReverbWetLevel(int value);
    void changedReverbDryLevel(int value);
    void changedReverbOutputVolume(int value);

    void changedDelayTime(int value);
    void changedDelayFeedback(int value);
    void changedDelayMix(int value);
    void changedDelayOutputVolume(int value);

    void changedChorusRate(int value);
    void changedChorusDepth(int value);
    void changedChorusCenterDelay(int value);
    void changedChorusFeedback(int value);
    void changedChorusMix(int value);
    void changedChorusOutputVolume(int value);

    void changedDistortionDrive(int value);
    void changedDistortionMix(int value);
    void changedDistortionOutputVolume(int value);

};

#endif // TrackSETTINGS_H
