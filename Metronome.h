#ifndef METRONOME_H
#define METRONOME_H

#include <QObject>
#include <QPushButton>
#include <QSlider>
#include <QSpinBox>

#include <QMouseEvent>

#include "MicroTimer.h"
#include "MetronomeSettings.h"
#include "TransportCallback.h"

class Metronome : public QPushButton
{
public:
    Metronome(QWidget *parent, juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume = 1.0, QString strong_measure_sound_path = "..//..//music//metronome//strong_measure.wav", QString weak_measure_sound_path = "..//..//music//metronome//weak_measure.wav", const std::vector<bool>& strong_and_weak_measures = {true, false, false, false});
    ~Metronome();
    void play_strong_measure();
    void play_weak_measure();
    void setVolume(float volume);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:

    juce::AudioDeviceManager& m_device_manager;
    juce::MixerAudioSource& m_mixer_source;
    juce::AudioFormatManager m_format_manager;
    juce::AudioTransportSource m_transport_source_strong_measure;
    juce::AudioTransportSource m_transport_source_weak_measure;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_sources_strong_measure;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_sources_weak_measure;

    //QMediaPlayer* m_player_strong_measure;
    //QMediaPlayer* m_player_weak_measure;
    //QAudioOutput* m_strong_audio_output;
    //QAudioOutput* m_weak_audio_output;
    MicroTimer* m_timer;
    MetronomeSettings* m_settings_window;
    std::array<void(Metronome::*)(), 4> m_strong_and_weak_measures_play_call_methods;
};

#endif // METRONOME_H
