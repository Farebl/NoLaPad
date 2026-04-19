#ifndef JUCEAUDIOENGINE_H
#define JUCEAUDIOENGINE_H


#include "IAudioEngine.h"
#include <juce_audio_devices/juce_audio_devices.h>
#include <QVector>
#include <mutex>
#include <atomic>

class IRecorder;
class IMetronomePlayer;

class JUCEAudioEngine final : public IAudioEngine, private juce::AudioIODeviceCallback
{
private:

    int    m_current_block_size  = 0;
    double m_current_sample_rate = 0.0;

    juce::AudioDeviceManager m_device_manager;
    std::vector<IPlayer*> m_tracks;
    std::unique_ptr<IRecorder> m_recorder;

    juce::AudioBuffer<float> m_track_buffer;
    juce::AudioBuffer<float> m_record_buffer;

    std::mutex m_tracks_mutex;
    std::atomic<bool> m_initialized{false};
    std::atomic<bool> m_is_recording{false};


    void audioDeviceIOCallbackWithContext(
        const float* const* inputChannelData, int numInputChannels,
        float* const* outputChannelData, int numOutputChannels,
        int numSamples,
        const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;


public:
    JUCEAudioEngine();
    ~JUCEAudioEngine() override;

    void addPlayer(IPlayer* player) override;
    void removePlayer(IPlayer* player) override;

    void startRecording(const QString& outputPath) override;
    void stopRecording() override;
    bool isRecording() const override;
};

#endif // JUCEAUDIOENGINE_H
