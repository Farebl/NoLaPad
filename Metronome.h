#ifndef METRONOME_H
#define METRONOME_H

#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include <QMouseEvent>


class IMetronomePlayer;
class BPMCounter;
class QPushButton;
class MetronomeSettings;
class MicroTimer;
class QHBoxLayout;

class Metronome : public QWidget
{
    Q_OBJECT

private:
    std::unique_ptr<IMetronomePlayer> m_player;
    MicroTimer* m_timer;
    MetronomeSettings* m_settings_window;
    QPushButton* m_play_button;
    BPMCounter* m_bpm_counter;

protected:
    void mousePressEvent(QMouseEvent *event) override;

public:
    explicit Metronome(
        MicroTimer* timer,
        const QString& first_measure_sound_path = "..//..//music//metronome//strong_measure.wav",
        const QString& second_measure_sound_path = "..//..//music//metronome//weak_measure.wav",
        const QString& third_measure_sound_path = "..//..//music//metronome//weak_measure.wav",
        const QString& fourth_measure_sound_path = "..//..//music//metronome//weak_measure.wav",
        float volume = 1.0,
        quint16 bpm_value = 60,
        QWidget* parent = nullptr
    );

    ~Metronome();
    void play();

    void setVolume(float volume);
    float getVolume() const;

    void setBPM(quint16 value);
    quint16 getBPM() const;

    IMetronomePlayer* getPlayer();
};

#endif // METRONOME_H
