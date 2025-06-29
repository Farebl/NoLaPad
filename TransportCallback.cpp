#include "TransportCallback.h"


TransportCallback::TransportCallback(juce::MixerAudioSource& mixer) : mixerSource(mixer) {}

void TransportCallback::audioDeviceIOCallbackWithContext(const float* const* input, int numInputChannels,
                                          float* const* output, int numOutputChannels, int numSamples,
                                          const juce::AudioIODeviceCallbackContext& context){
    juce::AudioBuffer<float> buffer(output, numOutputChannels, numSamples);
    mixerSource.getNextAudioBlock(juce::AudioSourceChannelInfo(buffer));
}

void TransportCallback::audioDeviceAboutToStart(juce::AudioIODevice* device){
    mixerSource.prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
}

void TransportCallback::audioDeviceStopped(){
        mixerSource.releaseResources();
}
