#ifndef TRANSPORTCALLBACK_H
#define TRANSPORTCALLBACK_H

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

class TransportCallback : public juce::AudioIODeviceCallback
{
public:
    TransportCallback(juce::MixerAudioSource& mixer);

    void audioDeviceIOCallbackWithContext(const float* const* input, int numInputChannels,
                                          float* const* output, int numOutputChannels, int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context) override;

    void audioDeviceAboutToStart(juce::AudioIODevice* device) override;
    void audioDeviceStopped() override;

private:
    juce::MixerAudioSource& mixerSource;
};


#endif // TRANSPORTCALLBACK_H
