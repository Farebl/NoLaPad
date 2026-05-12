#include "Metronome.h"
#include "BPMCounter.h"
#include "JUCEMetronomePlayer.h"
#include "MicroTimer.h"
#include "MetronomeSettings.h"

Metronome::Metronome(
    MicroTimer* timer,
    const QString& first_measure_sound_path,
    const QString& second_measure_sound_path,
    const QString& third_measure_sound_path,
    const QString& fourth_measure_sound_path,
    float volume,
    quint16 bpm_value,
    QWidget *parent)
    : QWidget(parent)
    , m_is_muted(true)
    , m_volume(volume)
    , m_player(new JUCEMetronomePlayer(first_measure_sound_path, second_measure_sound_path, third_measure_sound_path, fourth_measure_sound_path, volume))
    , m_timer(timer)
    , m_settings_window(new MetronomeSettings(100, this))
    , m_bpm_counter(new BPMCounter(m_timer, bpm_value, this))
    , m_play_button(new QPushButton(this))

{
    QString unmute_style = QString(
                               "QPushButton {"
                               "    background-color: #b0b0b0;"
                               "    color: #ebebeb;"
                               "    border: none;"
                               "    border-radius: 3px;"
                               "    font-size: %1px;"
                               "    padding: 0px;"
                               "}"
                               "QPushButton:hover {"
                               "    background-color: #b3b3b3;"
                               "}"
                               ).arg(m_play_button->height()-10);

    QString mute_style = QString(
                                 "QPushButton {"
                                 "    background-color: #fb1e22;"
                                 "    color: #ebebeb;"
                                 "    border: none;"
                                 "    border-radius: 3px;"
                                 "    font-size: %1px;"
                                 "    padding: 0px;"
                                 "}"
                                 "QPushButton:hover {"
                                 "    background-color: #ff3b3e;"
                                 "}"
                                 ).arg(m_play_button->height()-10);


    m_play_button->setFixedSize(55, 30);
    m_play_button->setText("mute");
    m_play_button->setCheckable(true);

    QHBoxLayout* layout = new QHBoxLayout(this);
    m_bpm_counter->setFixedHeight(35);
    layout->setSpacing(4);
    layout->setAlignment(Qt::AlignVCenter);
    layout->addWidget(m_play_button, 0, Qt::AlignVCenter);
    layout->addWidget(m_bpm_counter, 0, Qt::AlignVCenter);
    layout->setContentsMargins(0,0,0,0);

    setVolume(volume);
    connect(m_settings_window, &MetronomeSettings::changedVolume, this, [this](int value){
        setVolume(value/100.0f);
    });

    connect(m_timer, m_timer->m_signals[0], this, &Metronome::play);
    connect(m_timer, m_timer->m_signals[4], this, &Metronome::play);
    connect(m_timer, m_timer->m_signals[8], this, &Metronome::play);
    connect(m_timer, m_timer->m_signals[12], this, &Metronome::play);

    connect(m_play_button, &QPushButton::toggled, this, [this, mute_style, unmute_style](bool checked){
        if (checked){
            unmute();
            m_play_button->setStyleSheet(unmute_style);
        }
        else{
            mute();
            m_play_button->setStyleSheet(mute_style);
        }
    });

    m_play_button->blockSignals(true);
    m_play_button->setChecked(true);
    m_play_button->blockSignals(false);
    mute();
}


Metronome::~Metronome() {
    qDebug()<<"Metronome::~Metronome() ";
}



void Metronome::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::RightButton){
        m_settings_window->exec();
    }

    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        event->ignore();
    }
}


void Metronome::play(){
    m_player->play();
}

void Metronome::mute(){
    m_is_muted = true;
    m_player->setVolume(0);
    m_play_button->setChecked(false);
}

void Metronome::unmute(){
    qDebug()<<"Unmute_start: "<<m_volume;
    m_is_muted = false;
    m_player->setVolume(m_volume);
    m_play_button->setChecked(true);
}

bool Metronome::isMuted(){return m_is_muted;}


void Metronome::setVolume(float volume){
    if ((volume < 0.0f) || (volume > 1.0f)){
        return;
    }
    m_volume = volume;
    if(!m_is_muted){
        m_player->setVolume(m_volume);
    }
    m_settings_window->setVolume(volume*100);
}

float Metronome::getVolume() const{
    return m_volume;
}


IPlayer* Metronome::getPlayer(){
    return m_player.get();
}


void Metronome::setBPM(quint16 value){
    m_bpm_counter->setBPM(value);
}

quint16 Metronome::getBPM() const{
    return m_bpm_counter->getBPM();
}
