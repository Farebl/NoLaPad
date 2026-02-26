#include "TransportCallback.h"
#include "Track.h"


TransportCallback* TransportCallback::m_instance = nullptr;

TransportCallback* TransportCallback::getInstance(juce::MixerAudioSource& mixer, Recorder* rec, std::vector<Track*>& tracks){
    if (m_instance == nullptr)
        m_instance = new TransportCallback(mixer, rec, tracks);

    return m_instance;
}



TransportCallback::TransportCallback(juce::MixerAudioSource& mixer, Recorder* rec, std::vector<Track*>& tracks)
    : mixerSource(mixer), m_rec(rec), m_tracks(tracks) {}

void TransportCallback::audioDeviceIOCallbackWithContext(const float* const* input, int numInputChannels,
                                                         float* const* output, int numOutputChannels, int numSamples,
                                                         const juce::AudioIODeviceCallbackContext& context)
{
    // ЗАХИСТ: Перевіряємо валідність параметрів
    if (output == nullptr || numOutputChannels <= 0 || numSamples <= 0)
        return;

    // ЗАХИСТ: Перевіряємо що вектор треків не порожній і має правильний розмір
    if (m_tracks.empty() || m_tracks.size() == 0)
    {
        juce::AudioBuffer<float> outputBuffer(output, numOutputChannels, numSamples);
        outputBuffer.clear();
        return;
    }

    // Буфер для виходу (те що чує користувач)
    juce::AudioBuffer<float> outputBuffer(output, numOutputChannels, numSamples);
    outputBuffer.clear();

    // Буфер для запису (тільки треки з увімкненим флагом)
    juce::AudioBuffer<float> recordBuffer(numOutputChannels, numSamples);
    recordBuffer.clear();

    // Тимчасовий буфер для кожного треку
    juce::AudioBuffer<float> trackBuffer(numOutputChannels, numSamples);

    // Проходимо по всіх треках
    for (auto* track : m_tracks)
    {
        // КРИТИЧНО ВАЖЛИВО: Перевіряємо валідність вказівника
        if (track == nullptr)
            continue;

        try
        {
            // Очищуємо буфер треку
            trackBuffer.clear();

            // Отримуємо аудіо від треку
            juce::AudioSourceChannelInfo trackInfo(trackBuffer);
            track->getNextAudioBlock(trackInfo);

            // ЗАХИСТ: Перевіряємо що буфер не порожній і має правильні розміри
            if (trackBuffer.getNumChannels() == 0 || trackBuffer.getNumSamples() == 0)
                continue;

            // Додаємо аудіо треку до виходу (користувач завжди чує всі активні треки)
            for (int channel = 0; channel < juce::jmin(numOutputChannels, trackBuffer.getNumChannels()); ++channel)
            {
                outputBuffer.addFrom(channel, 0, trackBuffer, channel, 0,
                                     juce::jmin(numSamples, trackBuffer.getNumSamples()));
            }

            // Якщо трек має флаг запису - додаємо його до буфера запису
            if (track->isRecordingEnabled())
            {
                for (int channel = 0; channel < juce::jmin(numOutputChannels, trackBuffer.getNumChannels()); ++channel)
                {
                    recordBuffer.addFrom(channel, 0, trackBuffer, channel, 0,
                                         juce::jmin(numSamples, trackBuffer.getNumSamples()));
                }
            }
        }
        catch (...)
        {
            // Якщо виникла помилка з одним треком - пропускаємо його і продовжуємо з іншими
            continue;
        }
    }

    // Записуємо буфер, якщо Recorder активний
    if (m_rec && m_rec->isRecording())
    {
        try
        {
            m_rec->writeAudioData(recordBuffer);
        }
        catch (...)
        {
            // Ігноруємо помилки запису, щоб не зупиняти аудіо
        }
    }
}

void TransportCallback::audioDeviceAboutToStart(juce::AudioIODevice* device)
{
    if (device == nullptr)
        return;

    // Готуємо всі треки до відтворення
    for (auto* track : m_tracks)
    {
        if (track != nullptr)
        {
            try
            {
                track->prepareToPlay(device->getCurrentBufferSizeSamples(), device->getCurrentSampleRate());
            }
            catch (...)
            {
                // Продовжуємо навіть якщо один трек не вдалося підготувати
                continue;
            }
        }
    }
}

void TransportCallback::audioDeviceStopped()
{
    // Звільняємо ресурси всіх треків
    for (auto* track : m_tracks)
    {
        if (track != nullptr)
        {
            try
            {
                track->releaseResources();
            }
            catch (...)
            {
                continue;
            }
        }
    }
}
