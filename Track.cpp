#include "Track.h"
#include <QTime>
#include <QDebug>

#define BORDER_RADIUS_COEF 10

Track::Track(QWidget *parent, juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume, bool is_loop, std::vector<std::vector<bool>> beats_per_measure, QString sound_path, const QColor& outer_background_color, const QColor& inner_active_background_color)
    : QPushButton(parent),
    juce::AudioSource(),
    m_device_manager(deviceManager),
    m_mixer_source(mixer),
    m_transport_source(),
    m_is_loop(is_loop),
    m_audio_sample_path(sound_path),
    m_is_active(false),
    m_is_recording_enabled(false),  // Ініціалізація флагу запису
    m_style("background-color: %1; color: %2; border-radius: %3px; margin: 2px;"),
    m_outer_color(outer_background_color),
    m_inner_color(inner_active_background_color),
    m_timer(timer),
    m_beats_per_measure(beats_per_measure),
    m_current_effect(EffectType::None),
    m_delay_settings({0.3f, 0.5f, 0.5f}),
    m_sample_rate(44100.0)
{
    setStyleSheet(m_style.arg(m_outer_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));

    if (m_beats_per_measure[0].size() < 4) m_beats_per_measure[0].resize(4, false);
    if (m_beats_per_measure[1].size() < 4) m_beats_per_measure[1].resize(4, false);
    if (m_beats_per_measure[2].size() < 4) m_beats_per_measure[2].resize(4, false);
    if (m_beats_per_measure[3].size() < 4) m_beats_per_measure[3].resize(4, false);

    m_format_manager.registerBasicFormats();

    auto& reverb = m_effect_chain.get<0>();
    reverb.setParameters(juce::dsp::Reverb::Parameters{ m_reverb_settings.roomSize, m_reverb_settings.damping, m_reverb_settings.wetLevel, m_reverb_settings.dryLevel, 0.5f, 0.0f });

    auto& delay = m_effect_chain.get<2>();
    delay.setDelay(m_delay_settings.delayTime * m_sample_rate);
    delay.setMaximumDelayInSamples(static_cast<int>(m_sample_rate * 2.0));

    if (!m_audio_sample_path.isEmpty()) {
        setAudioSamplePath(m_audio_sample_path);
    }
    if (volume >= 0.0f && volume <= 1.0f) {
        m_transport_source.setGain(volume);
    } else {
        m_transport_source.setGain(1.0);
    }
    m_is_ready.store(true);
}

Track::~Track() {
    m_is_being_destroyed.store(true);
    m_is_ready.store(false);

    // Даємо час аудіо callback'у завершити роботу
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    m_transport_source.stop();
    m_transport_source.setSource(nullptr);
}

void Track::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    m_sample_rate = sampleRate;

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = samplesPerBlockExpected;
    spec.numChannels = 2;

    m_effect_chain.prepare(spec);
    m_transport_source.prepareToPlay(samplesPerBlockExpected, sampleRate);

    // ВАЖЛИВО: Створюємо буфер з ДОСТАТНІМ розміром
    m_effect_buffer = std::make_unique<juce::AudioBuffer<float>>(2, samplesPerBlockExpected * 2); // x2 для запасу
    m_effect_buffer->clear();

    updateEffectParameters();
}

void Track::releaseResources()
{
    m_transport_source.releaseResources();
    m_effect_chain.reset();
}

float calculateBlockAmplitude(const juce::dsp::AudioBlock<float>& block, size_t channel)
{
    float maxAmplitude = 0.0f;
    const float* samples = block.getChannelPointer(channel);
    for (size_t i = 0; i < block.getNumSamples(); ++i)
    {
        maxAmplitude = juce::jmax(maxAmplitude, std::abs(samples[i]));
    }
    return maxAmplitude;
}

void Track::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    if (!m_is_ready.load() || m_is_being_destroyed.load()) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if (m_reader_source == nullptr) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if (bufferToFill.buffer == nullptr ||
        bufferToFill.numSamples <= 0 ||
        bufferToFill.buffer->getNumChannels() < 2) {
        bufferToFill.clearActiveBufferRegion();
        return;
    }

    if (m_effect_buffer == nullptr ||
        m_effect_buffer->getNumSamples() < bufferToFill.numSamples) {
        m_transport_source.getNextAudioBlock(bufferToFill);
        return;
    }

    m_transport_source.getNextAudioBlock(bufferToFill);

    if (m_current_effect != EffectType::None) {
        int channelsToCopy = juce::jmin(2, bufferToFill.buffer->getNumChannels());
        int samplesToCopy = juce::jmin(bufferToFill.numSamples, m_effect_buffer->getNumSamples());

        for (int ch = 0; ch < channelsToCopy; ++ch) {
            m_effect_buffer->copyFrom(ch, 0,
                                      bufferToFill.buffer->getReadPointer(ch, bufferToFill.startSample),
                                      samplesToCopy);
        }

        // ПРАВИЛЬНО: створюємо блок, потім обмежуємо
        juce::dsp::AudioBlock<float> fullBlock(*m_effect_buffer);
        juce::dsp::AudioBlock<float> block = fullBlock.getSubBlock(0, samplesToCopy);
        juce::dsp::ProcessContextReplacing<float> context(block);

        switch (m_current_effect) {
        case EffectType::Reverb:
            m_effect_chain.get<0>().process(context);
            break;
        case EffectType::Delay:
        {
            auto& delay = m_effect_chain.get<2>();
            auto numSamples = block.getNumSamples();
            auto numChannels = block.getNumChannels();
            float feedback = m_delay_settings.feedback;
            float wet = m_delay_settings.wetLevel;
            float dry = 1.0f - wet;

            for (size_t channel = 0; channel < numChannels; ++channel) {
                auto* samples = block.getChannelPointer(channel);
                for (size_t i = 0; i < numSamples; ++i) {
                    float delayedSample = delay.popSample(static_cast<int>(channel),
                                                          static_cast<int>(m_delay_settings.delayTime * m_sample_rate));
                    float inputWithFeedback = samples[i] + delayedSample * feedback;
                    delay.pushSample(static_cast<int>(channel), inputWithFeedback);
                    samples[i] = delayedSample * wet + samples[i] * dry;
                }
            }
        }
        break;
        default:
            break;
        }

        for (int channel = 0; channel < channelsToCopy; ++channel) {
            bufferToFill.buffer->copyFrom(channel, bufferToFill.startSample,
                                          m_effect_buffer->getReadPointer(channel),
                                          samplesToCopy);
        }
    }
}

void Track::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_reader_source) {
            QMessageBox::information(this, "Error", "Треку не назначено звук для відтворювання");
            return;
        }

        if (!m_is_loop) {
            m_is_active = true;
            update();
            play();
        } else {
            if (!m_is_active) {
                m_is_active = true;
                update();
                if (m_timer) {
                    disconnect(m_timer, &MicroTimer::tick1, this, &Track::stop);
                    if (m_beats_per_measure[0][0])
                        connect(m_timer, &MicroTimer::tick1, this, &Track::play);
                    if (m_beats_per_measure[0][1])
                        connect(m_timer, &MicroTimer::tick2, this, &Track::play);
                    if (m_beats_per_measure[0][2])
                        connect(m_timer, &MicroTimer::tick3, this, &Track::play);
                    if (m_beats_per_measure[0][3])
                        connect(m_timer, &MicroTimer::tick4, this, &Track::play);
                    if (m_beats_per_measure[1][0])
                        connect(m_timer, &MicroTimer::tick5, this, &Track::play);
                    if (m_beats_per_measure[1][1])
                        connect(m_timer, &MicroTimer::tick6, this, &Track::play);
                    if (m_beats_per_measure[1][2])
                        connect(m_timer, &MicroTimer::tick7, this, &Track::play);
                    if (m_beats_per_measure[1][3])
                        connect(m_timer, &MicroTimer::tick8, this, &Track::play);
                    if (m_beats_per_measure[2][0])
                        connect(m_timer, &MicroTimer::tick9, this, &Track::play);
                    if (m_beats_per_measure[2][1])
                        connect(m_timer, &MicroTimer::tick10, this, &Track::play);
                    if (m_beats_per_measure[2][2])
                        connect(m_timer, &MicroTimer::tick11, this, &Track::play);
                    if (m_beats_per_measure[2][3])
                        connect(m_timer, &MicroTimer::tick12, this, &Track::play);
                    if (m_beats_per_measure[3][0])
                        connect(m_timer, &MicroTimer::tick13, this, &Track::play);
                    if (m_beats_per_measure[3][1])
                        connect(m_timer, &MicroTimer::tick14, this, &Track::play);
                    if (m_beats_per_measure[3][2])
                        connect(m_timer, &MicroTimer::tick15, this, &Track::play);
                    if (m_beats_per_measure[3][3])
                        connect(m_timer, &MicroTimer::tick16, this, &Track::play);
                }
            } else {
                m_is_active = false;
                if (m_timer) {
                    connect(m_timer, &MicroTimer::tick1, this, &Track::stop);
                    disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick2, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick3, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick4, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick5, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick6, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick7, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick8, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick9, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick10, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick11, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick12, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick13, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick14, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick15, this, &Track::play);
                    disconnect(m_timer, &MicroTimer::tick16, this, &Track::play);
                }
            }
        }
    }

    if (event->button() == Qt::RightButton) {
        emit rightClicked(this);
    }

    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mousePressEvent(event);
    }
}

void Track::paintEvent(QPaintEvent *event){
    setStyleSheet(m_style.arg(m_outer_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));

    QPushButton::paintEvent(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if(m_is_active){
        painter.setBrush(m_inner_color);
    }
    else{
        painter.setBrush(Qt::darkGray);
    }

    painter.setPen(Qt::NoPen);

    int center_x = width() / 2;
    int center_y = height() / 2;

    int radius;
    if (width()>=height()){
        radius = height() / 6;
    }
    if (width()<height()){
        radius = width() / 6;
    }
    painter.drawEllipse(QPoint(center_x, center_y), radius, radius);
}

void Track::setVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_transport_source.setGain(volume);
    }
}

void Track::setAudioSamplePath(QString path)
{
    m_audio_sample_path = path;
    if (m_audio_sample_path.isEmpty()) {
        m_transport_source.stop();
        m_transport_source.setSource(nullptr);
        m_reader_source.reset();
        m_is_active = false;
        if (m_timer) {
            disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick2, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick3, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick4, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick5, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick6, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick7, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick8, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick9, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick10, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick11, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick12, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick13, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick14, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick15, this, &Track::play);
            disconnect(m_timer, &MicroTimer::tick16, this, &Track::play);
        }
        return;
    }

    juce::File audioFile(m_audio_sample_path.toStdString());
    if (!audioFile.existsAsFile() || !audioFile.hasReadAccess()) {
        qDebug() << "Cannot access audio file:" << m_audio_sample_path;
        QMessageBox::information(this, "Error", "Немає доступу до аудіофайлу: " + m_audio_sample_path);
        return;
    }
    auto reader = m_format_manager.createReaderFor(audioFile);
    if (reader == nullptr) {
        qDebug() << "Failed to create reader for audio file:" << m_audio_sample_path;
        QMessageBox::information(this, "Error", "Не вдалося створити ридер для аудіофайлу: " + m_audio_sample_path);
        return;
    }
    m_reader_source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    m_transport_source.setSource(m_reader_source.get(), 0, nullptr, reader->sampleRate);
}

QString Track::getAudioSamplePath()
{
    return m_audio_sample_path;
}

void Track::setLoopState(bool state)
{
    m_is_loop = state;
    if (m_reader_source) {
        m_reader_source->setLooping(state);
    }
}

void Track::setRecordingEnabled(bool enabled)
{
    m_is_recording_enabled = enabled;
}

bool Track::isRecordingEnabled() const
{
    return m_is_recording_enabled;
}

void Track::setOuterBackgroundColor(QColor color)
{
    m_outer_color = color;
    update();
}

void Track::setInnerActiveBackgroundColor(QColor color)
{
    m_inner_color = color;
    update();
}

QString Track::getSoundPath()
{
    return m_audio_sample_path;
}

std::vector<std::vector<bool>> Track::getBeats()
{
    return m_beats_per_measure;
}

void Track::setBeat1(bool state)
{
    m_beats_per_measure[0][0] = state;
    if (m_beats_per_measure[0][0] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick1, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
}

void Track::setBeat2(bool state)
{
    m_beats_per_measure[0][1] = state;
    if (m_beats_per_measure[0][1] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick2, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick2, this, &Track::play);
}

void Track::setBeat3(bool state)
{
    m_beats_per_measure[0][2] = state;
    if (m_beats_per_measure[0][2] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick3, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick3, this, &Track::play);
}

void Track::setBeat4(bool state)
{
    m_beats_per_measure[0][3] = state;
    if (m_beats_per_measure[0][3] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick4, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick4, this, &Track::play);
}

void Track::setBeat5(bool state)
{
    m_beats_per_measure[1][0] = state;
    if (m_beats_per_measure[1][0] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick5, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick5, this, &Track::play);
}

void Track::setBeat6(bool state)
{
    m_beats_per_measure[1][1] = state;
    if (m_beats_per_measure[1][1] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick6, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick6, this, &Track::play);
}

void Track::setBeat7(bool state)
{
    m_beats_per_measure[1][2] = state;
    if (m_beats_per_measure[1][2] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick7, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick7, this, &Track::play);
}

void Track::setBeat8(bool state)
{
    m_beats_per_measure[1][3] = state;
    if (m_beats_per_measure[1][3] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick8, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick8, this, &Track::play);
}

void Track::setBeat9(bool state)
{
    m_beats_per_measure[2][0] = state;
    if (m_beats_per_measure[2][0] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick9, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick9, this, &Track::play);
}

void Track::setBeat10(bool state)
{
    m_beats_per_measure[2][1] = state;
    if (m_beats_per_measure[2][1] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick10, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick10, this, &Track::play);
}

void Track::setBeat11(bool state)
{
    m_beats_per_measure[2][2] = state;
    if (m_beats_per_measure[2][2] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick11, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick11, this, &Track::play);
}

void Track::setBeat12(bool state)
{
    m_beats_per_measure[2][3] = state;
    if (m_beats_per_measure[2][3] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick12, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick12, this, &Track::play);
}

void Track::setBeat13(bool state)
{
    m_beats_per_measure[3][0] = state;
    if (m_beats_per_measure[3][0] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick13, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick13, this, &Track::play);
}

void Track::setBeat14(bool state)
{
    m_beats_per_measure[3][1] = state;
    if (m_beats_per_measure[3][1] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick14, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick14, this, &Track::play);
}

void Track::setBeat15(bool state)
{
    m_beats_per_measure[3][2] = state;
    if (m_beats_per_measure[3][2] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick15, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick15, this, &Track::play);
}

void Track::setBeat16(bool state)
{
    m_beats_per_measure[3][3] = state;
    if (m_beats_per_measure[3][3] && m_is_loop && m_is_active)
        connect(m_timer, &MicroTimer::tick16, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick16, this, &Track::play);
}

float Track::getVolume()
{
    return m_transport_source.getGain();
}

bool Track::getLoopState()
{
    return m_is_loop;
}

QColor Track::getInnerActiveBackgroundColor()
{
    return m_inner_color;
}

QColor Track::getOuterBackgroundColor()
{
    return m_outer_color;
}

void Track::setEffectType(EffectType type)
{
    m_current_effect = type;
    updateEffectParameters();
}

Track::EffectType Track::getEffectType()
{
    return m_current_effect;
}

void Track::setReverbRoomSize(float size)
{
    if (size >= 0.0f && size <= 1.0f) {
        m_reverb_settings.roomSize = size;
        updateEffectParameters();
    }
}

float Track::getReverbRoomSize()
{
    return m_reverb_settings.roomSize;
}

void Track::setReverbDamping(float damping)
{
    if (damping >= 0.0f && damping <= 1.0f) {
        m_reverb_settings.damping = damping;
        updateEffectParameters();
    }
}

float Track::getReverbDamping()
{
    return m_reverb_settings.damping;
}

void Track::setReverbWetLevel(float wet)
{
    if (wet >= 0.0f && wet <= 1.0f) {
        m_reverb_settings.wetLevel = wet;
        updateEffectParameters();
    }
}

float Track::getReverbWetLevel()
{
    return m_reverb_settings.wetLevel;
}

void Track::setReverbDryLevel(float dry)
{
    if (dry >= 0.0f && dry <= 1.0f) {
        m_reverb_settings.dryLevel = dry;
        updateEffectParameters();
    }
}

float Track::getReverbDryLevel()
{
    return m_reverb_settings.dryLevel;
}

void Track::setDelayTime(float time)
{
    if (time >= 0.0f && time <= 1.0f) {
        m_delay_settings.delayTime = time;
        updateEffectParameters();
    }
}

float Track::getDelayTime()
{
    return m_delay_settings.delayTime;
}

void Track::setDelayFeedback(float feedback)
{
    if (feedback >= 0.0f && feedback <= 0.9f) {
        m_delay_settings.feedback = feedback;
        updateEffectParameters();
    }
}

float Track::getDelayFeedback()
{
    return m_delay_settings.feedback;
}

void Track::setDelayWetLevel(float wet)
{
    if (wet >= 0.0f && wet <= 1.0f) {
        m_delay_settings.wetLevel = wet;
        updateEffectParameters();
    }
}

float Track::getDelayWetLevel()
{
    return m_delay_settings.wetLevel;
}

void Track::updateEffectParameters()
{
    auto& reverb = m_effect_chain.get<0>();
    reverb.setParameters(juce::dsp::Reverb::Parameters{ m_reverb_settings.roomSize, m_reverb_settings.damping, m_reverb_settings.wetLevel, m_reverb_settings.dryLevel, 0.5f, 0.0f });

    auto& delay = m_effect_chain.get<2>();
    delay.setDelay(m_delay_settings.delayTime * m_sample_rate);
    delay.setMaximumDelayInSamples(static_cast<int>(m_sample_rate));
}

void Track::play()
{
    if (m_reader_source) {
        m_transport_source.setPosition(0);
        m_transport_source.start();
    }
}

void Track::stop()
{
    m_transport_source.stop();
    auto& delay = m_effect_chain.get<2>();
    delay.reset();
}
