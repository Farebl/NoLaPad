#ifndef JUCETRACKPLAYER_H
#define JUCETRACKPLAYER_H

#include <atomic>
#include <memory>


#include <juce_dsp/juce_dsp.h>
#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>
#include <juce_core/juce_core.h>

#include "ITrackPlayer.h"

class JUCETrackPlayer final : public ITrackPlayer {
public:
    JUCETrackPlayer();
    ~JUCETrackPlayer() override;
    void prepareToPlay(int samplesPerBlock, double sampleRate) override;
    void getNextAudioBlock(AudioBlockInfo* buffer_to_fill) override;
    void releaseResources() override;
    void play() override;
    void stop() override;
    bool isPlaying()    const override;
    bool hasFileLoaded() const override;

    void loadAudioFileForPlaying(const QString& audio_sample_path) override;
    void setVolume(float volume) override;
    float getVolume() const override;
    void setLoopEnabled(bool loop) override;
    void setRecordingEnabled(bool enabled) override;
    bool isRecordingEnabled() const override;

    void setEffectType(EffectType type) override;
    EffectType getCurrentEffectType() const override;

    // Reverb settings methods
    void setReverbRoomSize(float size) override;
    float getReverbRoomSize() override;
    void setReverbDamping(float damping) override;
    float getReverbDamping() override;
    void setReverbWetLevel(float wet) override;
    float getReverbWetLevel() override;
    void setReverbDryLevel(float dry) override;
    float getReverbDryLevel() override;
    void setReverbOutputVolume(float volume) override;
    float getReverbOutputVolume() override;


    // Delay settings methods
    void setDelayTime(float time) override;
    float getDelayTime() override;
    void setDelayFeedback(float feedback) override;
    float getDelayFeedback() override;
    void setDelayMixLevel(float mix) override;
    float getDelayMixLevel() override;
    void setDelayOutputVolume(float volume) override;
    float getDelayOutputVolume() override;


    // Chorus settings methods
    void setChorusRate(float rate) override;
    float getChorusRate() override;
    void setChorusDepth(float depth) override;
    float getChorusDepth() override;
    void setChorusCenterDelay(float delay) override;
    float getChorusCenterDelay() override;;
    void setChorusFeedback(float feedback) override;
    float getChorusFeedback() override;
    void setChorusMix(float mix) override;
    float getChorusMix() override;
    void setChorusOutputVolume(float volume) override;
    float getChorusOutputVolume() override;

    // Distortion settings methods
    void setDistortionDrive(float drive) override;
    float getDistortionDrive() override;
    void setDistortionMix(float mix) override;
    float getDistortionMix() override;
    void setDistortionOutputVolume(float volume) override;
    float getDistortionOutputVolume() override;




private:
    juce::AudioFormatManager m_format_manager;
    juce::AudioTransportSource m_transport_source;
    std::unique_ptr<juce::AudioFormatReaderSource> m_reader_source;

    using EffectChain = juce::dsp::ProcessorChain<
        juce::dsp::Reverb,
        juce::dsp::Phaser<float>,
        juce::dsp::DelayLine<float, juce::dsp::DelayLineInterpolationTypes::Linear>,
        juce::dsp::LadderFilter<float>
    >;

    EffectChain m_effect_chain;
    std::unique_ptr<juce::AudioBuffer<float>> m_effect_buffer;
    double m_sample_rate   = 44100.0;
    EffectType m_current_effect = EffectType::None;
    ReverbSettings m_reverb_settings;
    DelaySettings m_delay_settings;
    ChorusSettings m_chorus_settings;
    DistortionSettings m_distortion_settings;
    std::atomic<bool> m_is_ready{false};
    std::atomic<bool> m_is_being_destroyed{false};
    std::atomic<bool> m_is_recording_enabled{false};

    void applyEffect(juce::AudioBuffer<float>& buffer, int numSamples);
    void updateEffectParameters() override;
};

#endif // JUCETRACKPLAYER_H
