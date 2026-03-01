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
#include "RedButton.h"
#include "EffectsSwitcher.h"

#include "AudioSampleSelector.h"

class LCDCounter;

// in Track.h:
enum class EffectType;
struct ReverbSettings;
struct DelaySettings;
struct ChorusSettings;
struct DistortionSettings;


class TrackSettings: public QDialog
{
    Q_OBJECT

private:
    explicit TrackSettings(quint8 volume_percent, bool is_loop, const std::array<bool, 16>& beats_per_measure, const QColor& outer_background_color, const QColor& inner_background_color, QWidget *parent);

    QSpinBox* m_volume_display;
    QSpinBox* m_effect_volume_display;
    Fader* m_volume_fader;
    Fader* m_effect_volume_fader;

    TrackColorButtons* m_select_track_colors_button;
    RedButton* m_loop_button;
    QVBoxLayout* m_beats_matrix_layout;

    LCDCounter* m_lag_whole_takts_setter;
    LCDCounter* m_lag_16_th_setter;
    LCDCounter* m_duration_whole_takts_setter;
    LCDCounter* m_duration_16_th_setter;

    EffectsSwitcher* m_effects_switcher;

    AudioSampleSelector* m_audio_input_connector;
    std::array<bool, 16> m_beats_per_measure;
    std::vector<QMetaObject::Connection> m_current_connections;

    QCheckBox* m_recording_checkbox;

    static TrackSettings* m_instance;

protected:
    void closeEvent(QCloseEvent *event) override;

public:
    static TrackSettings* getInstance(quint8 volume_percent = 100, bool is_loop = false, std::array<bool, 16> beats_per_measure = {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0}, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray, QWidget *parent = nullptr);

    void setLoopState(bool state);
    void setVolume(int volum_percent);
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
    void changedVolume(int volum_percent);
    void changedInnerActiveBackgroundColor(QColor color);
    void changedOuterBackgroundColor(QColor color);
    void changedAudioSamplePath(QString path);
    void changedBeatState(int index, bool state);

    void changedRecordingEnabled(bool enabled);
};

#endif // TrackSETTINGS_H
