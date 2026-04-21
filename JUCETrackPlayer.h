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
    void updateEffectParameters();

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
    ReverbSettings getReverbSettings() const override;
    DelaySettings getDelaySettings() const override;;
    ChorusSettings getChorusSettings() const override;
    DistortionSettings getDistortionSettings() const override;


    // Reverb settings methods
    void setReverbRoomSize(float size) override;
    float getReverbRoomSize() const override;
    void setReverbDamping(float damping) override;
    float getReverbDamping() const override;
    void setReverbWetLevel(float wet) override;
    float getReverbWetLevel() const override;
    void setReverbDryLevel(float dry) override;
    float getReverbDryLevel() const override;
    void setReverbOutputVolume(float volume) override;
    float getReverbOutputVolume() const override;


    // Delay settings methods
    void setDelayTime(float time) override;
    float getDelayTime() const override;
    void setDelayFeedback(float feedback) override;
    float getDelayFeedback() const override;
    void setDelayMixLevel(float mix) override;
    float getDelayMixLevel() const override;
    void setDelayOutputVolume(float volume) override;
    float getDelayOutputVolume() const override;


    // Chorus settings methods
    void setChorusRate(float rate) override;
    float getChorusRate() const override;
    void setChorusDepth(float depth) override;
    float getChorusDepth() const override;
    void setChorusCenterDelay(float delay) override;
    float getChorusCenterDelay() const override;;
    void setChorusFeedback(float feedback) override;
    float getChorusFeedback() const override;
    void setChorusMix(float mix) override;
    float getChorusMix() const override;
    void setChorusOutputVolume(float volume) override;
    float getChorusOutputVolume() const override;


    // Distortion settings methods
    void setDistortionDrive(float drive) override;
    float getDistortionDrive() const override;
    void setDistortionMix(float mix) override;
    float getDistortionMix() const override;
    void setDistortionOutputVolume(float volume) override;
    float getDistortionOutputVolume() const override;
};

#endif // JUCETRACKPLAYER_H
