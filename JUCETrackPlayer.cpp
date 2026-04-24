#include "JUCETrackPlayer.h"
#include "AudioBlockInfo.h"
#include <QDebug>

JUCETrackPlayer::JUCETrackPlayer() {
    m_format_manager.registerBasicFormats();
}

JUCETrackPlayer::~JUCETrackPlayer() {
    m_is_being_destroyed.store(true);
    m_is_ready.store(false);

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    m_transport_source.stop();
    m_transport_source.setSource(nullptr);
}



void JUCETrackPlayer::prepareToPlay(int samplesPerBlock, double sampleRate){

    m_sample_rate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlock;
    spec.numChannels = 2;

    m_effect_chain.prepare(spec);
    m_transport_source.prepareToPlay(samplesPerBlock, sampleRate);

    m_effect_buffer = std::make_unique<juce::AudioBuffer<float>>(2, samplesPerBlock * 2); // x2 для запасу
    m_effect_buffer->clear();

    updateEffectParameters();
    m_is_ready.store(true);
}

void JUCETrackPlayer::getNextAudioBlock(AudioBlockInfo* block) {

    if (!m_is_ready.load() || m_is_being_destroyed.load() || !m_reader_source) {
        for (int ch = 0; ch < block->numChannels; ++ch)
            juce::FloatVectorOperations::clear(block->data[ch], block->numSamples);
        return;
    }


    // Обгортаємо AudioBlockInfo у juce::AudioBuffer без копіювання (non-owning)
    juce::AudioBuffer<float> wrapper(block->data, block->numChannels, block->numSamples);
    juce::AudioSourceChannelInfo info(&wrapper, 0, block->numSamples);
    m_transport_source.getNextAudioBlock(info);


    if (m_current_effect != EffectType::None &&
        m_effect_buffer &&
        m_effect_buffer->getNumSamples() >= block->numSamples)
    {
        applyEffect(wrapper, block->numSamples);
    }
}


void JUCETrackPlayer::releaseResources()
{
    m_is_ready.store(false);
    m_transport_source.releaseResources();
    m_effect_chain.reset();
}


void JUCETrackPlayer::applyEffect(juce::AudioBuffer<float>& buffer, int numSamples) {
    const int channels = juce::jmin(2, buffer.getNumChannels());
    const int samples  = juce::jmin(numSamples, m_effect_buffer->getNumSamples());
    // Копіюємо у тимчасовий буфер для DSP
    for (int ch = 0; ch < channels; ++ch)
        m_effect_buffer->copyFrom(ch, 0, buffer.getReadPointer(ch), samples);
    juce::dsp::AudioBlock<float> fullBlock(*m_effect_buffer);
    auto block = fullBlock.getSubBlock(0, static_cast<size_t>(samples));
    juce::dsp::ProcessContextReplacing<float> ctx(block);
    switch (m_current_effect) {
    case EffectType::Reverb:
        m_effect_chain.get<0>().process(ctx);
        break;
    case EffectType::Chorus: {
        m_effect_chain.get<1>().process(ctx);
        float wet = m_chorus_settings.mix, dry = 1.0f - wet;
        for (int ch = 0; ch < channels; ++ch) {
            auto* dst = block.getChannelPointer(ch);
            const auto* src = buffer.getReadPointer(ch);
            for (size_t i = 0; i < block.getNumSamples(); ++i)
                dst[i] = dst[i] * wet + src[i] * dry;
        }
        break;
    }
    case EffectType::Delay: {
        auto& delay    = m_effect_chain.get<2>();
        float feedback = m_delay_settings.feedback;
        float wet      = m_delay_settings.mix, dry = 1.0f - wet;
        for (int ch = 0; ch < channels; ++ch) {
            auto* dst = block.getChannelPointer(ch);
            for (size_t i = 0; i < block.getNumSamples(); ++i) {
                float delayed = delay.popSample(ch,
                                                static_cast<float>(m_delay_settings.delayTime * m_sample_rate));
                delay.pushSample(ch, dst[i] + delayed * feedback);
                dst[i] = delayed * wet + dst[i] * dry;
            }
        }
        break;
    }
    case EffectType::Distortion: {
        float drive        = m_distortion_settings.drive;
        float mix          = m_distortion_settings.mix;
        float outputVolume = m_distortion_settings.outputVolume;
        for (int ch = 0; ch < channels; ++ch) {
            auto* dst       = block.getChannelPointer(ch);
            const auto* src = buffer.getReadPointer(ch);
            for (size_t i = 0; i < block.getNumSamples(); ++i) {
                float distorted = std::tanh(src[i] * drive) / std::tanh(drive);
                dst[i] = (distorted * mix + src[i] * (1.0f - mix)) * outputVolume;
            }
        }
        break;
    }
    default: break;
    }
    // Копіюємо результат назад
    for (int ch = 0; ch < channels; ++ch)
        buffer.copyFrom(ch, 0, m_effect_buffer->getReadPointer(ch), samples);
}



void JUCETrackPlayer::play() {
    m_transport_source.setPosition(0.0);
    m_transport_source.start();
}

void JUCETrackPlayer::stop() {
    m_transport_source.stop();
}

bool JUCETrackPlayer::isPlaying() const {
    return m_transport_source.isPlaying();
}

bool JUCETrackPlayer::hasFileLoaded() const {
    return
        m_reader_source != nullptr;
}

void JUCETrackPlayer::loadAudioFileForPlaying(const QString& audio_sample_path) {
    juce::File audioFile(audio_sample_path.toStdString());
    if (!audioFile.existsAsFile()) return;

    auto* reader = m_format_manager.createReaderFor(audioFile);
    if (!reader) return;


    m_transport_source.stop();
    m_transport_source.setSource(nullptr);


    m_reader_source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);


    m_transport_source.setSource(m_reader_source.get(), 0, nullptr, reader->sampleRate);
}

void JUCETrackPlayer::setVolume(float volume) {
    if (volume < 0.0f || volume > 1.0f){
        return;
    }
    m_transport_source.setGain(volume);
}


float JUCETrackPlayer::getVolume() const {
    return m_transport_source.getGain();
}


void JUCETrackPlayer::setLoopEnabled(bool loop) {
    if (m_reader_source) m_reader_source->setLooping(loop);
}

void JUCETrackPlayer::setRecordingEnabled(bool enabled) {
    m_is_recording_enabled.store(enabled);
}

bool JUCETrackPlayer::isRecordingEnabled() const {
    return m_is_recording_enabled.load();
}

void JUCETrackPlayer::setEffectType(EffectType type) {
    m_current_effect = type;
    updateEffectParameters();
}



void JUCETrackPlayer::updateEffectParameters() {
    m_effect_chain.get<0>().setParameters({
        m_reverb_settings.roomSize, m_reverb_settings.damping,
        m_reverb_settings.wetLevel, m_reverb_settings.dryLevel, 0.5f, 0.0f
    });
    auto& phaser = m_effect_chain.get<1>();
    phaser.setRate(m_chorus_settings.rate);
    phaser.setDepth(m_chorus_settings.depth);
    phaser.setCentreFrequency(m_chorus_settings.centerDelay);
    phaser.setFeedback(m_chorus_settings.feedback);
    phaser.setMix(m_chorus_settings.mix);
    auto& delay = m_effect_chain.get<2>();
    delay.setMaximumDelayInSamples(static_cast<int>(m_sample_rate * 2.0));
    delay.setDelay(static_cast<float>(m_delay_settings.delayTime * m_sample_rate));
}


EffectType JUCETrackPlayer::getCurrentEffectType() const{
    return m_current_effect;
}


ReverbSettings JUCETrackPlayer::getReverbSettings() const {
    return m_reverb_settings;
}

DelaySettings JUCETrackPlayer::getDelaySettings() const {
    return m_delay_settings;
}

ChorusSettings JUCETrackPlayer::getChorusSettings() const {
    return m_chorus_settings;
}

DistortionSettings JUCETrackPlayer::getDistortionSettings() const {
    return m_distortion_settings;
}




// Reverb methods

void JUCETrackPlayer::setReverbRoomSize(float size)
{
    m_reverb_settings.roomSize = size;
    updateEffectParameters();

}

float JUCETrackPlayer::getReverbRoomSize() const
{
    return m_reverb_settings.roomSize;
}

void JUCETrackPlayer::setReverbDamping(float damping)
{

    m_reverb_settings.damping = damping;
    updateEffectParameters();

}

float JUCETrackPlayer::getReverbDamping() const
{
    return m_reverb_settings.damping;
}

void JUCETrackPlayer::setReverbWetLevel(float wet)
{

    m_reverb_settings.wetLevel = wet;
    updateEffectParameters();

}

float JUCETrackPlayer::getReverbWetLevel() const
{
    return m_reverb_settings.wetLevel;
}

void JUCETrackPlayer::setReverbDryLevel(float dry)
{

    m_reverb_settings.dryLevel = dry;
    updateEffectParameters();

}

float JUCETrackPlayer::getReverbDryLevel() const
{
    return m_reverb_settings.dryLevel;
}



void JUCETrackPlayer::setReverbOutputVolume(float volume)
{

    m_reverb_settings.outputVolume = volume;

}

float JUCETrackPlayer::getReverbOutputVolume() const
{
    return m_reverb_settings.outputVolume;
}




// Delay methods
void JUCETrackPlayer::setDelayTime(float time)
{

    m_delay_settings.delayTime = time;
    updateEffectParameters();

}

float JUCETrackPlayer::getDelayTime() const
{
    return m_delay_settings.delayTime;
}

void JUCETrackPlayer::setDelayFeedback(float feedback)
{

    m_delay_settings.feedback = feedback;
    updateEffectParameters();

}

float JUCETrackPlayer::getDelayFeedback() const
{
    return m_delay_settings.feedback;
}

void JUCETrackPlayer::setDelayMixLevel(float mix)
{

    m_delay_settings.mix = mix;
     updateEffectParameters();

}

float JUCETrackPlayer::getDelayMixLevel() const
{
    return m_delay_settings.mix;
}


void JUCETrackPlayer::setDelayOutputVolume(float volume)
{

    m_delay_settings.outputVolume = volume;

}

float JUCETrackPlayer::getDelayOutputVolume() const
{
    return m_delay_settings.outputVolume;
}





// Chorus methods
void JUCETrackPlayer::setChorusRate(float rate)
{

    m_chorus_settings.rate = rate;
    updateEffectParameters();

}

float JUCETrackPlayer::getChorusRate() const
{
    return m_chorus_settings.rate;
}

void JUCETrackPlayer::setChorusDepth(float depth)
{

    m_chorus_settings.depth = depth;
    updateEffectParameters();

}

float JUCETrackPlayer::getChorusDepth() const
{
    return m_chorus_settings.depth;
}

void JUCETrackPlayer::setChorusCenterDelay(float delay)
{

    m_chorus_settings.centerDelay = delay;
    updateEffectParameters();

}

float JUCETrackPlayer::getChorusCenterDelay() const
{
    return m_chorus_settings.centerDelay;
}

void JUCETrackPlayer::setChorusFeedback(float feedback)
{

    m_chorus_settings.feedback = feedback;
    updateEffectParameters();

}

float JUCETrackPlayer::getChorusFeedback() const
{
    return m_chorus_settings.feedback;
}

void JUCETrackPlayer::setChorusMix(float mix)
{

    m_chorus_settings.mix = mix;
    updateEffectParameters();

}

float JUCETrackPlayer::getChorusMix() const
{
    return m_chorus_settings.mix;
}

void JUCETrackPlayer::setChorusOutputVolume(float volume)
{

    m_chorus_settings.outputVolume = volume;

}

float JUCETrackPlayer::getChorusOutputVolume() const
{
    return m_chorus_settings.outputVolume;
}




// Distortion methods
void JUCETrackPlayer::setDistortionDrive(float drive)
{

    m_distortion_settings.drive = drive;
    updateEffectParameters();

}

float JUCETrackPlayer::getDistortionDrive() const
{
    return m_distortion_settings.drive;
}

void JUCETrackPlayer::setDistortionMix(float mix)
{

    m_distortion_settings.mix = mix;
    updateEffectParameters();

}

float JUCETrackPlayer::getDistortionMix() const
{
    return m_distortion_settings.mix;
}


void JUCETrackPlayer::setDistortionOutputVolume(float volume)
{

    m_distortion_settings.outputVolume = volume;

}

float JUCETrackPlayer::getDistortionOutputVolume() const
{
    return m_distortion_settings.outputVolume;
}



