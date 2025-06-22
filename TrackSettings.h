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

#include "MicroTimer.h"
#include "Fader.h"
#include "BeatCheckBox.h"
#include "TrackColorButtons.h"
#include "LoopButton.h"
#include "Potentiometer.h"
#include "ClickableLabel.h"
#include "AudioSampleSelector.h"

class TrackSettings: public QDialog
{
    Q_OBJECT
public:
    explicit TrackSettings(uint8_t volume_percent = 100, bool is_loop = false, std::vector<std::vector<bool>> beats_per_measure = {{1,0,0,0},{0,0,0,0},{0,0,0,0},{0,0,0,0}}, const QColor& outer_background_color = Qt::gray, const QColor& inner_background_color = Qt::darkGray, QWidget *parent = nullptr);
    void setLoopState(bool state);
    void setVolume(int volum_percent);
    void setEffectVolume(int volum_percent);
    void setInnerActiveBackgroundColor(QColor color);
    void setOuterBackgroundColor(QColor color);
    void setBeats(std::vector<std::vector<bool>> beats_per_measure);
    void setIsAudioSampleSelectedState(bool state);

private:
    QSpinBox* m_volume_display;
    QSpinBox* m_effect_volume_display;
    Fader* m_volume_fader;
    Fader* m_effect_volume_fader;
    TrackColorButtons* m_select_track_colors_button;
    LoopButton* m_loop_button;
    Potentiometer* m_effects_switch;
    QVBoxLayout* m_beats_matrix_layout;
    AudioSampleSelector* m_audio_input_connector;
    std::vector<std::vector<bool>> m_beats_per_measure;
    std::vector<QMetaObject::Connection> m_current_connections;

protected:
    void closeEvent(QCloseEvent *event) override;

signals:
    void changedLoopState(bool state);
    void changedVolume(int volum_percent);
    void changedInnerActiveBackgroundColor(QColor color);
    void changedOuterBackgroundColor(QColor color);
    void changedAudioSamplePath(QString path);
    void changedBeat1(bool state);
    void changedBeat2(bool state);
    void changedBeat3(bool state);
    void changedBeat4(bool state);
    void changedBeat5(bool state);
    void changedBeat6(bool state);
    void changedBeat7(bool state);
    void changedBeat8(bool state);
    void changedBeat9(bool state);
    void changedBeat10(bool state);
    void changedBeat11(bool state);
    void changedBeat12(bool state);
    void changedBeat13(bool state);
    void changedBeat14(bool state);
    void changedBeat15(bool state);
    void changedBeat16(bool state);
};

#endif // TrackSETTINGS_H
