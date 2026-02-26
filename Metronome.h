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
    Q_OBJECT

private:
    juce::AudioDeviceManager& m_device_manager;
    juce::MixerAudioSource& m_mixer_source;
    juce::AudioFormatManager m_format_manager;
    juce::AudioTransportSource m_transport_source_strong_measure;
    juce::AudioTransportSource m_transport_source_weak_measure;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_sources_strong_measure;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_sources_weak_measure;

    MicroTimer* m_timer;
    MetronomeSettings* m_settings_window;
    std::array<void(Metronome::*)(), 4> m_strong_and_weak_measures_play_call_methods;

    static Metronome* m_instance;
    explicit Metronome(juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume, QString strong_measure_sound_path, QString weak_measure_sound_path, const std::vector<bool>& strong_and_weak_measures, QWidget* parent);

protected:
    void mousePressEvent(QMouseEvent *event) override;

public:
    static Metronome* getInstance(juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume = 1.0, QString strong_measure_sound_path = "..//..//music//metronome//strong_measure.wav", QString weak_measure_sound_path = "..//..//music//metronome//weak_measure.wav", const std::vector<bool>& strong_and_weak_measures = {true, false, false, false}, QWidget* parent = nullptr);
    ~Metronome();
    void play_strong_measure();
    void play_weak_measure();
    void setVolume(float volume);
};

#endif // METRONOME_H
