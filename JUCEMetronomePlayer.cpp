#include "JUCEMetronomePlayer.h"
#include "AudioBlockInfo.h"
#include <QDebug>
JUCEMetronomePlayer::JUCEMetronomePlayer(const std::array<std::string, 4>& measures_sounds_path, float volume)
    : m_current_measure(0)
{
    m_format_manager.registerBasicFormats();
    setVolume(volume);
    loadAudioFilesForPlaying(measures_sounds_path); 
}

JUCEMetronomePlayer::~JUCEMetronomePlayer(){}

void JUCEMetronomePlayer::loadAudioFilesForPlaying(const std::array<std::string, 4>& samples_paths){
    auto loadSource = [&](const std::string& path,
                          juce::AudioTransportSource& transport,
                          std::unique_ptr<juce::AudioFormatReaderSource>& readerSource)
    {
        juce::File file(path);
        if (!file.existsAsFile()) return;
        auto* reader = m_format_manager.createReaderFor(file);
        if (!reader) return;
        readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        transport.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
    };

    loadSource(samples_paths[0], m_transport_0, m_reader_0);
    loadSource(samples_paths[1], m_transport_1, m_reader_1);
    loadSource(samples_paths[2], m_transport_2, m_reader_2);
    loadSource(samples_paths[3], m_transport_3, m_reader_3);
}

void JUCEMetronomePlayer::prepareToPlay(int samplesPerBlock, double sampleRate) {
    m_transport_0.prepareToPlay(samplesPerBlock, sampleRate);
    m_transport_1.prepareToPlay(samplesPerBlock, sampleRate);
    m_transport_2.prepareToPlay(samplesPerBlock, sampleRate);
    m_transport_3.prepareToPlay(samplesPerBlock, sampleRate);
    m_is_ready.store(true);
}

void JUCEMetronomePlayer::getNextAudioBlock(AudioBlockInfo* block) {

    if (!m_is_ready.load()) {
        for (int ch = 0; ch < block->numChannels; ++ch)
            juce::FloatVectorOperations::clear(block->data[ch], block->numSamples);
        return;
    }

    // Мікшуємо обидва транспорти
    juce::AudioBuffer<float> wrapper(block->data, block->numChannels, block->numSamples);

    juce::AudioBuffer<float> tempBuffer(block->numChannels, block->numSamples);
    tempBuffer.clear();


    juce::AudioSourceChannelInfo info_0(&tempBuffer, 0, block->numSamples);
    m_transport_0.getNextAudioBlock(info_0);
    for (int ch = 0; ch < block->numChannels; ++ch)
        wrapper.addFrom(ch, 0, tempBuffer, ch, 0, block->numSamples);

    tempBuffer.clear();
    juce::AudioSourceChannelInfo info_1(&tempBuffer, 0, block->numSamples);
    m_transport_1.getNextAudioBlock(info_1);
    for (int ch = 0; ch < block->numChannels; ++ch)
        wrapper.addFrom(ch, 0, tempBuffer, ch, 0, block->numSamples);

    tempBuffer.clear();
    juce::AudioSourceChannelInfo info_2(&tempBuffer, 0, block->numSamples);
    m_transport_2.getNextAudioBlock(info_2);
    for (int ch = 0; ch < block->numChannels; ++ch)
        wrapper.addFrom(ch, 0, tempBuffer, ch, 0, block->numSamples);

    tempBuffer.clear();
    juce::AudioSourceChannelInfo info_3(&tempBuffer, 0, block->numSamples);
    m_transport_3.getNextAudioBlock(info_3);
    for (int ch = 0; ch < block->numChannels; ++ch)
        wrapper.addFrom(ch, 0, tempBuffer, ch, 0, block->numSamples);
}

void JUCEMetronomePlayer::releaseResources() {
    m_is_ready.store(false);
    m_transport_0.releaseResources();
    m_transport_1.releaseResources();
    m_transport_2.releaseResources();
    m_transport_3.releaseResources();
}


bool JUCEMetronomePlayer::hasFileLoaded() const{
    return (m_reader_0 != nullptr || m_reader_1 != nullptr || m_reader_2 != nullptr || m_reader_3 != nullptr);
}



void JUCEMetronomePlayer::setVolume(float volume) {
    if (volume < 0.0f || volume > 1.0f){
        return;
    }
    m_transport_0.setGain(volume);
    m_transport_1.setGain(volume);
    m_transport_2.setGain(volume);
    m_transport_3.setGain(volume);
}

float JUCEMetronomePlayer::getVolume() const {
    return m_transport_0.getGain();
}

void JUCEMetronomePlayer::setRecordingEnabled(bool enabled) {
    m_is_recording_enabled.store(enabled);
}
bool JUCEMetronomePlayer::isRecordingEnabled() const {
    //return m_is_recording_enabled.load();
    return false;
}

void JUCEMetronomePlayer::play() {
    switch (m_current_measure){
    case 0:
        m_transport_0.setPosition(0.0);
        m_transport_0.start();
        break;
    case 1:
        m_transport_1.setPosition(0.0);
        m_transport_1.start();
        break;
    case 2:
        m_transport_2.setPosition(0.0);
        m_transport_2.start();
        break;
    case 3:
        m_transport_3.setPosition(0.0);
        m_transport_3.start();
        m_current_measure = -1;
        break;
    }
    ++m_current_measure;
}


void JUCEMetronomePlayer::stop() {
    m_transport_0.stop();
    m_transport_1.stop();
    m_transport_2.stop();
    m_transport_3.stop();
}

