#include "Metronome.h"

Metronome::Metronome(MicroTimer* timer, QWidget *parent, float volume, QString strong_measure_sound_path, QString weak_measure_sound_path, const std::vector<bool>& strong_and_weak_measures)
    : QPushButton(parent),
    m_player_strong_measure(new QMediaPlayer(this)),
    m_player_weak_measure(new QMediaPlayer(this)),
    m_strong_audioOutput(new QAudioOutput(this)),
    m_weak_audioOutput(new QAudioOutput(this)),
    m_timer(timer),
    m_settingsWindow(new MetronomeSettings(this))

{
    if (strong_and_weak_measures.size() != 4) {
        m_strong_and_weak_measures[0] = &Metronome::play_strong_measure;
        m_strong_and_weak_measures[1] = &Metronome::play_weak_measure;
        m_strong_and_weak_measures[2] = &Metronome::play_weak_measure;
        m_strong_and_weak_measures[3] = &Metronome::play_weak_measure;
    }
    else{
        m_strong_and_weak_measures[0] = (strong_and_weak_measures[0]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures[1] = (strong_and_weak_measures[1]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures[2] = (strong_and_weak_measures[2]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
        m_strong_and_weak_measures[3] = (strong_and_weak_measures[3]) ? &Metronome::play_strong_measure : &Metronome::play_weak_measure;
    }


    setText("▶");
    setCheckable(true);
    setChecked(false);
    setStyleSheet(
        "QPushButton {"
        "    background-color: #28963c;" // Зелений фон
        "    color: #FFFFFF;" // Белый текст
        "    border: none;" // Без рамки
        "    border-radius: 3px;" // Скругление всех углов
        "    font-size: 26px;" // Размер символа
        "    padding: 0px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #46be5a;" // Изменение цвета при наведении
        "}"
    );

    m_player_strong_measure->setSource(QUrl::fromLocalFile(strong_measure_sound_path));
    m_player_strong_measure->setAudioOutput(m_strong_audioOutput);

    m_player_weak_measure->setSource(QUrl::fromLocalFile(weak_measure_sound_path));
    m_player_weak_measure->setAudioOutput(m_weak_audioOutput);

    m_strong_audioOutput->setVolume(volume);

    connect(m_settingsWindow, &MetronomeSettings::changedVolume, this, &Metronome::setVolume);
}


Metronome::~Metronome() {
    if (m_player_strong_measure) {
        m_player_strong_measure->stop();
        delete m_player_strong_measure;
        m_player_strong_measure = nullptr;
    }
    if (m_player_weak_measure) {
        m_player_weak_measure->stop();
        delete m_player_weak_measure;
        m_player_weak_measure = nullptr;
    }
    if (m_strong_audioOutput) {
        delete m_strong_audioOutput;
        m_strong_audioOutput = nullptr;
    }
    if (m_weak_audioOutput) {
        delete m_weak_audioOutput;
        m_weak_audioOutput = nullptr;
    }
}




void Metronome::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if (m_player_strong_measure->source().isEmpty()){
            QMessageBox::information(this, "Error", "Метроному не назначено звук для відтворювання сильної долі");
            return;
        }

        if (m_player_weak_measure->source().isEmpty()){
            QMessageBox::information(this, "Error", "Треку не назначено звук для відтворювання слабкої долі");
            return;
        }

        if (isChecked()){
            disconnect(m_timer, MicroTimer::tick1, this, m_strong_and_weak_measures[0]);
            disconnect(m_timer, MicroTimer::tick3, this, m_strong_and_weak_measures[1]);
            disconnect(m_timer, MicroTimer::tick5, this, m_strong_and_weak_measures[2]);
            disconnect(m_timer, MicroTimer::tick7, this, m_strong_and_weak_measures[3]);
            setChecked(false);
            setText("▶");
            setStyleSheet(
                "QPushButton {"
                "    background-color: #28963c;" // Зелений фон
                "    color: #FFFFFF;" // Белый текст
                "    border: none;" // Без рамки
                "    border-radius: 3px;" // Скругление всех углов
                "    font-size: 26px;" // Размер символа
                "    padding: 0px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #46be5a;" // Изменение цвета при наведении
                "}"
                );
        }
        else{
            connect(m_timer, MicroTimer::tick1, this, m_strong_and_weak_measures[0]);
            connect(m_timer, MicroTimer::tick3, this, m_strong_and_weak_measures[1]);
            connect(m_timer, MicroTimer::tick5, this, m_strong_and_weak_measures[2]);
            connect(m_timer, MicroTimer::tick7, this, m_strong_and_weak_measures[3]);
            setChecked(true);
            setStyleSheet(
                "QPushButton {"
                "    background-color: #c82828;" // Червоний фон
                "    color: #FFFFFF;" // Белый текст
                "    border: none;" // Без рамки
                "    border-radius: 3px;" // Скругление всех углов
                "    font-size: 14px;" // Размер символа
                "    padding: 0px;"
                "}"
                "QPushButton:hover {"
                "    background-color: #dc3c3c;" // Изменение цвета при наведении
                "}"
                );
            setText("| |");
        }
    }

    if(event->button() == Qt::RightButton){
        m_settingsWindow->exec();
    }

    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mousePressEvent(event);
    }
}




void Metronome::play_strong_measure(){
    m_player_strong_measure->stop();
    m_player_strong_measure->play();
}

void Metronome::play_weak_measure(){
    m_player_weak_measure->stop();
    m_player_weak_measure->play();
}


void Metronome::setVolume(int volume_percent){
    if ((volume_percent < 0) || (volume_percent > 100)){
        return;
    }

    m_strong_audioOutput->setVolume(static_cast<double>(volume_percent)/100.0);
    m_weak_audioOutput->setVolume(static_cast<double>(volume_percent)/100.0);
}


