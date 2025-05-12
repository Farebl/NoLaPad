#ifndef METRONOME_H
#define METRONOME_H

#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>
#include <QMediaPlayer>
#include <QAudioOutput>

#include <QMouseEvent>

#include "MicroTimer.h"
#include "MetronomeSettings.h"

class Metronome : public QPushButton
{
public:
    Metronome(MicroTimer* timer = nullptr, QWidget *parent = nullptr, float volume = 1.0, QString strong_measure_sound_path = "..//..//music//metronome//strong_measure.wav", QString weak_measure_sound_path = "..//..//music//metronome//weak_measure.wav", const std::vector<bool>& strong_and_weak_measures = {true, false, false, false});
    ~Metronome();
    void play_strong_measure();
    void play_weak_measure();
    void setVolume(int volume_percent);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QMediaPlayer* m_player_strong_measure;
    QMediaPlayer* m_player_weak_measure;
    QAudioOutput* m_strong_audioOutput;
    QAudioOutput* m_weak_audioOutput;
    MicroTimer* m_timer;
    MetronomeSettings* m_settingsWindow;
    std::array<void(Metronome::*)(), 4> m_strong_and_weak_measures;
};

#endif // METRONOME_H
