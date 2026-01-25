#include "Metronome.h"


Metronome::Metronome(QWidget *parent, juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume, QString strong_measure_sound_path, QString weak_measure_sound_path, const std::vector<bool>& strong_and_weak_measures)
    : QPushButton(parent),
    m_device_manager(deviceManager),
    m_mixer_source(mixer),
    m_transport_source_strong_measure(juce::AudioTransportSource()),
    m_transport_source_weak_measure(juce::AudioTransportSource()),
    m_timer(timer),
    m_settings_window(new MetronomeSettings(this))
{
    if (strong_and_weak_measures.size() != 4) {
        m_strong_and_weak_measures_play_call_methods[0] = &Metronome::play_strong_measure;
        m_strong_and_weak_measures_play_call_methods[1] = &Metronome::play_weak_measure;
        m_strong_and_weak_measures_play_call_methods[2] = &Metronome::play_weak_measure;
        m_strong_and_weak_measures_play_call_methods[3] = &Metronome::play_weak_measure;
    }
    else{
        m_strong_and_weak_measures_play_call_methods[0] = (strong_and_weak_measures[0]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures_play_call_methods[1] = (strong_and_weak_measures[1]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures_play_call_methods[2] = (strong_and_weak_measures[2]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures_play_call_methods[3] = (strong_and_weak_measures[3]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
    }

    setText("▶");
    setCheckable(true);
    setChecked(false);
    setStyleSheet(
        "QPushButton {"
        "    background-color: #28963c;"
        "    color: #FFFFFF;"
        "    border: none;"
        "    border-radius: 3px;"
        "    font-size: 26px;"
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #46be5a;"
        "}"
    );

    m_format_manager.registerBasicFormats();

    juce::File strong_measure_audio_file(strong_measure_sound_path.toStdString());
    if (!strong_measure_audio_file.existsAsFile() || !strong_measure_audio_file.hasReadAccess()) {
        std::cerr << "Cannot access MP3 file: " << strong_measure_audio_file.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Немає доступу до MP3 файлу: " + strong_measure_sound_path);
        return;
    }

    auto strong_measure_audio_reader = m_format_manager.createReaderFor(strong_measure_audio_file);
    if (strong_measure_audio_reader == nullptr) {
        std::cerr << "Failed to create strong_measure_audio_reader for MP3 file: " << strong_measure_audio_file.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Не вдалося створити ридер для MP3 файлу: " + strong_measure_sound_path);
        return;
    }

    m_reader_sources_strong_measure = std::make_unique<juce::AudioFormatReaderSource>(strong_measure_audio_reader, true);
    m_transport_source_strong_measure.setSource(m_reader_sources_strong_measure.get(), 0, nullptr, strong_measure_audio_reader->sampleRate);
    m_mixer_source.addInputSource(&m_transport_source_strong_measure, false);


    juce::File weak_measure_audio_file(weak_measure_sound_path.toStdString());
    if (!weak_measure_audio_file.existsAsFile() || !weak_measure_audio_file.hasReadAccess()) {
        std::cerr << "Cannot access MP3 file: " << weak_measure_audio_file.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Немає доступу до MP3 файлу: " + weak_measure_sound_path);
        return;
    }

    auto weak_measure_audio_reader = m_format_manager.createReaderFor(weak_measure_audio_file);
    if (weak_measure_audio_reader == nullptr) {
        std::cerr << "Failed to create weak_measure_audio_reader for MP3 file: " << weak_measure_audio_file.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Не вдалося створити ридер для MP3 файлу: " + weak_measure_sound_path);
        return;
    }

    m_reader_sources_weak_measure = std::make_unique<juce::AudioFormatReaderSource>(weak_measure_audio_reader, true);
    m_transport_source_weak_measure.setSource(m_reader_sources_weak_measure.get(), 0, nullptr, weak_measure_audio_reader->sampleRate);
    m_mixer_source.addInputSource(&m_transport_source_weak_measure, false);



    m_transport_source_strong_measure.setGain(volume);
    m_transport_source_weak_measure.setGain(volume);

    connect(m_settings_window, &MetronomeSettings::changedVolume, this, [this](int value){
        setVolume(value/100.0f);
    });
}


Metronome::~Metronome() {
    m_transport_source_strong_measure.stop();
    m_transport_source_weak_measure.stop();
    m_mixer_source.removeInputSource(&m_transport_source_strong_measure);
    m_mixer_source.removeInputSource(&m_transport_source_weak_measure);
    m_transport_source_strong_measure.setSource(nullptr);
    m_transport_source_weak_measure.setSource(nullptr);
}




void Metronome::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){

        if (!m_reader_sources_strong_measure){
            QMessageBox::information(this, "Error", "Метроному не назначено звук для відтворювання сильної долі");
            return;
        }

        if (!m_reader_sources_weak_measure){
            QMessageBox::information(this, "Error", "Метроному не назначено звук для відтворювання слабкої долі");
            return;
        }

        if (isChecked()){
            disconnect(m_timer, &MicroTimer::tick1, this, m_strong_and_weak_measures_play_call_methods[0]);
            disconnect(m_timer, &MicroTimer::tick5, this, m_strong_and_weak_measures_play_call_methods[1]);
            disconnect(m_timer, &MicroTimer::tick9, this, m_strong_and_weak_measures_play_call_methods[2]);
            disconnect(m_timer, &MicroTimer::tick13, this, m_strong_and_weak_measures_play_call_methods[3]);
            setChecked(false);
            setText("▶");
            setStyleSheet(
                "QPushButton {"
                "    background-color: #28963c;"
                "    color: #FFFFFF;"
                "    border: none;"
                "    border-radius: 3px;"
                "    font-size: 26px;"
                "    padding: 0px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #46be5a;" // Изменение цвета при наведении
                "}"
                );
        }
        else{
            connect(m_timer, &MicroTimer::tick1, this, m_strong_and_weak_measures_play_call_methods[0]);
            connect(m_timer, &MicroTimer::tick5, this, m_strong_and_weak_measures_play_call_methods[1]);
            connect(m_timer, &MicroTimer::tick9, this, m_strong_and_weak_measures_play_call_methods[2]);
            connect(m_timer, &MicroTimer::tick13, this, m_strong_and_weak_measures_play_call_methods[3]);
            setChecked(true);
            setStyleSheet(
                "QPushButton {"
                "    background-color: #c82828;"
                "    color: #FFFFFF;"
                "    border: none;"
                "    border-radius: 3px;"
                "    font-size: 14px;"
                "    padding: 0px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #dc3c3c;"
                "}"
                );
            setText("| |");
        }
    }

    if(event->button() == Qt::RightButton){
        m_settings_window->exec();
    }

    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mousePressEvent(event);
    }
}




void Metronome::play_strong_measure(){
    m_transport_source_strong_measure.setPosition(0);
    m_transport_source_strong_measure.start();

}

void Metronome::play_weak_measure(){
    m_transport_source_weak_measure.setPosition(0);
    m_transport_source_weak_measure.start();
}


void Metronome::setVolume(float volume){
    if ((volume < 0.0f) || (volume > 1.0f)){
        return;
    }

    m_transport_source_strong_measure.setGain(volume);
    m_transport_source_weak_measure.setGain(volume);
}


