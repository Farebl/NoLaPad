#ifndef JUCEMETRONOMEPLAYER_H
#define JUCEMETRONOMEPLAYER_H

#include "IMetronomePlayer.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <atomic>
#include <memory>
#include <QString>

class JUCEMetronomePlayer final : public IMetronomePlayer {
private:
    qint8 m_current_measure;

    juce::AudioFormatManager m_format_manager;
    juce::AudioTransportSource m_transport_0;
    juce::AudioTransportSource m_transport_1;
    juce::AudioTransportSource m_transport_2;
    juce::AudioTransportSource m_transport_3;

    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_0;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_1;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_2;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_3;

    std::atomic<bool> m_is_ready{false};
    std::atomic<bool> m_is_recording_enabled{false};

public:
    JUCEMetronomePlayer(const std::array<std::string, 4>& measures_sounds_path, float volume = 1.0);

    ~JUCEMetronomePlayer() override;

    void prepareToPlay(int samplesPerBlock, double sampleRate) override;
    void getNextAudioBlock(AudioBlockInfo* block) override;
    void releaseResources() override;

    void play() override;
    void stop() override;
    bool isPlaying() const override { return false; }
    bool hasFileLoaded() const override;

    void setVolume(float volume) override;
    float getVolume() const override;

    void setRecordingEnabled(bool enabled) override;
    bool isRecordingEnabled() const override;

    void loadAudioFilesForPlaying(const std::array<std::string, 4>& samples_paths) override;
};

#endif // JUCEMETRONOMEPLAYER_H
