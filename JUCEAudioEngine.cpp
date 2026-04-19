#include "JUCEAudioEngine.h"
#include "IPlayer.h"
#include "IRecorder.h"
#include <algorithm>

#include "IPlayer.h"
#include "IRecorder.h"
#include "AudioBlockInfo.h"

#include "JUCERecorder.h"
#include <QDebug>


JUCEAudioEngine::JUCEAudioEngine():
    m_recorder(new JUCERecorder())
{
    auto error = m_device_manager.initialise(0, 2, nullptr, true);
    if (error.isNotEmpty()){
        throw std::runtime_error("Cannot initialize juce::AudioDeviceManager");
    }

    m_device_manager.addAudioCallback(this);
    m_initialized.store(true);
}


JUCEAudioEngine::~JUCEAudioEngine() {
    if (!m_initialized.load()) return;
    m_device_manager.removeAudioCallback(this);
    m_device_manager.closeAudioDevice();
    m_initialized.store(false);
}



void JUCEAudioEngine::addPlayer(IPlayer* player) {
    std::lock_guard<std::mutex> lock(m_tracks_mutex);
    m_tracks.push_back(player);

    if (m_current_sample_rate > 0.0){
        player->prepareToPlay(m_current_block_size, m_current_sample_rate);
    }


}

void JUCEAudioEngine::removePlayer(IPlayer* player) {
    std::lock_guard<std::mutex> lock(m_tracks_mutex);
    m_tracks.erase(std::remove(m_tracks.begin(), m_tracks.end(), player),
                   m_tracks.end());
}

void JUCEAudioEngine::startRecording(const QString& outputPath) {

    if (!m_recorder) {
        return;
    };

    m_recorder->startRecording(outputPath);
    m_is_recording.store(true);
}

void JUCEAudioEngine::stopRecording() {
    m_is_recording.store(false);
    if (m_recorder){
        m_recorder->stopRecording();
    }
}

bool JUCEAudioEngine::isRecording() const { return m_is_recording.load(); }


// ----------------------------------------------------------------
// Аудіо-тік — викликається ОС кожні ~10 мс
// ----------------------------------------------------------------
void JUCEAudioEngine::audioDeviceIOCallbackWithContext(
    const float* const*, int,
    float* const* outputChannelData, int numOutputChannels,
    int numSamples,
    const juce::AudioIODeviceCallbackContext&)
{
    if (!outputChannelData || numOutputChannels <= 0 || numSamples <= 0) return;

    // Вихідний буфер — non-owning wrapper (дані ідуть напряму на ЦАП)
    juce::AudioBuffer<float> outputBuffer(outputChannelData, numOutputChannels, numSamples);
    outputBuffer.clear();
    m_record_buffer.clear();

    // Опис блоку для одного треку (вказівники переключаємо на m_track_buffer)
    AudioBlockInfo trackBlock;
    trackBlock.numChannels = numOutputChannels;
    trackBlock.numSamples  = numSamples;

    // Будуємо масив вказівників на канали m_track_buffer
    std::array<float*, 2> trackPtrs{};
    for (int ch = 0; ch < juce::jmin(numOutputChannels, 2); ++ch)
        trackPtrs[ch] = m_track_buffer.getWritePointer(ch);
    trackBlock.data = trackPtrs.data();


    std::lock_guard<std::mutex> lock(m_tracks_mutex);

    for (auto* player : m_tracks) {
        if (!player) continue;

        m_track_buffer.clear();
        player->getNextAudioBlock(&trackBlock);

        // Мікшуємо у вихід (всі треки)
        for (int ch = 0; ch < juce::jmin(numOutputChannels, m_track_buffer.getNumChannels()); ++ch) {
            const float* src = m_track_buffer.getReadPointer(ch);
            for (int i = 0; i < numSamples; ++i)
                outputChannelData[ch][i] += src[i];
        }

        // Мікшуємо у буфер запису (тільки позначені)
        if (m_is_recording.load() && player->isRecordingEnabled()) {
            for (int ch = 0; ch < juce::jmin(numOutputChannels, m_record_buffer.getNumChannels()); ++ch) {
                const float* src = m_track_buffer.getReadPointer(ch);
                float* dst = m_record_buffer.getWritePointer(ch);
                for (int i = 0; i < numSamples; ++i)
                    dst[i] += src[i];
            }
        }
    }



    if (m_is_recording.load() && m_recorder && m_recorder->isRecording()) {
        std::array<float*, 2> recPtrs{};
        for (int ch = 0; ch < juce::jmin(numOutputChannels, 2); ++ch)
            recPtrs[ch] = m_record_buffer.getWritePointer(ch);

        AudioBlockInfo recBlock{ recPtrs.data(), numOutputChannels, numSamples };
        m_recorder->writeChunk(&recBlock);
    }
}






void JUCEAudioEngine::audioDeviceAboutToStart(juce::AudioIODevice* device) {
    if (!device){
        return;
    }
    m_current_block_size  = device->getCurrentBufferSizeSamples();
    m_current_sample_rate = device->getCurrentSampleRate();
    m_track_buffer.setSize(2, m_current_block_size, false, true, false);
    m_record_buffer.setSize(2, m_current_block_size, false, true, false);
    std::lock_guard<std::mutex> lock(m_tracks_mutex);
    for (auto* player : m_tracks)
        if (player) player->prepareToPlay(m_current_block_size, m_current_sample_rate);
}


void JUCEAudioEngine::audioDeviceStopped() {
    std::lock_guard<std::mutex> lock(m_tracks_mutex);
    for (auto* player : m_tracks)
        if (player) player->releaseResources();
}
