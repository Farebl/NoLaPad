
#include <QTime>
#include <QDebug>
#include "Track.h"
#include "JUCETrackPlayer.h"

#define BORDER_RADIUS_COEF 10

Track::Track(
    MicroTimer* timer,
    float volume,
    bool is_loop,
    std::array<bool, 16> beats_per_measure,
    QString sound_path,
    const QColor& outer_background_color,
    const QColor& inner_active_background_color,
    QWidget *parent
    ):
      QPushButton(parent)
    , m_player(new JUCETrackPlayer())
    , m_is_loop(is_loop)
    , m_audio_sample_path(sound_path)
    , m_is_active(false)
    , m_is_recording_enabled(false)
    , m_style("background-color: %1; color: %2; border-radius: %3px; margin: 2px;")
    , m_outer_color(outer_background_color)
    , m_inner_color(inner_active_background_color)
    , m_timer(timer)
    , m_beats_per_measure(beats_per_measure)

{
    setStyleSheet(m_style.arg(m_outer_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));
    setAudioSamplePath(sound_path);
    setVolume(volume);
}

Track::~Track() {
}

void Track::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_player->hasFileLoaded()){
            QMessageBox::information(this, "Error", "Треку не назначено звук для відтворювання");
            return;
        }

        if (!m_is_loop) {
            m_is_active = true;
            update();
            play();
        }
        else {
            if (!m_is_active) {
                m_is_active = true;
                update();
                if (m_timer) {
                    disconnect(m_timer, &MicroTimer::tick1, this, &Track::stop);
                    auto measures_count = m_beats_per_measure.size();
                    for(size_t i = 0; i < measures_count; ++i){
                        if (m_beats_per_measure[i])
                            connect(m_timer, m_timer->m_signals[i], this, &Track::play);
                    }
                }
            }
            else {
                m_is_active = false;
                if (m_timer) {
                    connect(m_timer, &MicroTimer::tick1, this, &Track::stop);

                    auto measures_count = m_beats_per_measure.size();
                    for(size_t i = 0; i < measures_count; ++i){
                        if (m_beats_per_measure[i])
                            disconnect(m_timer, m_timer->m_signals[i], this, &Track::play);
                    }
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

void Track::mouseReleaseEvent(QMouseEvent *event){

    if (event->button() == Qt::LeftButton) {
        if (!m_is_loop && m_is_active) {
            m_is_active = false;
            update();
        }
    }

    if (event->button() == Qt::RightButton) {
        event->accept();
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
    if ((volume < 0.0f) || (volume > 1.0f)) {
        return;
    }
    m_player->setVolume(volume);

}


void Track::setAudioSamplePath(QString path)
{
    m_audio_sample_path = path;
    if (!m_audio_sample_path.isEmpty()) {
        m_player->loadAudioFileForPlaying(path);

        m_is_active = false;
        if (m_timer) {
            auto measures_count = m_beats_per_measure.size();
            for(size_t i = 0; i < measures_count; ++i){
                if (m_beats_per_measure[i])
                    disconnect(m_timer, m_timer->m_signals[i], this, &Track::play);
            }
        }
        return;
    }
}

QString Track::getAudioSamplePath()
{
    return m_audio_sample_path;
}

void Track::setLoopState(bool state)
{
    m_is_loop = state;
}

void Track::setRecordingEnabled(bool state)
{
    m_is_recording_enabled = state;
    m_player->setRecordingEnabled(state);
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

std::array<bool, 16> Track::getBeatsStates()
{
    return m_beats_per_measure;
}

void Track::setBeatState(quint8 index, bool state)
{
    m_beats_per_measure[index] = state;

    if (m_beats_per_measure[index] && m_is_active)
        connect(
            m_timer,
            m_timer->m_signals[index],
            this,
            &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
}


float Track::getVolume()
{
    return m_player->getVolume();
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

    m_player->setEffectType(type);
}

EffectType Track::getEffectType()
{
    return m_player->getCurrentEffectType();
}





// Reverb methods

void Track::setReverbRoomSize(float size)
{
    if (size >= 0.0f && size <= 1.0f) {
        m_player->setReverbRoomSize(size);
    }
}

float Track::getReverbRoomSize()
{
    return m_player->getReverbRoomSize();
}

void Track::setReverbDamping(float damping)
{
    if (damping >= 0.0f && damping <= 1.0f) {
        m_player->setReverbDamping(damping);
    }

}

float Track::getReverbDamping()
{
    return m_player->getReverbDamping();
}


void Track::setReverbWetLevel(float wet)
{
    if (wet >= 0.0f && wet <= 1.0f) {
        m_player->setReverbWetLevel(wet);
    }

}

float Track::getReverbWetLevel()
{

    return m_player->getReverbWetLevel();
}

void Track::setReverbDryLevel(float dry)
{
    if (dry >= 0.0f && dry <= 1.0f) {
        m_player->setReverbDryLevel(dry);
    }

}

float Track::getReverbDryLevel()
{
    return m_player->getReverbDryLevel();
}



void Track::setReverbOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setReverbOutputVolume(volume);
    }
}

float Track::getReverbOutputVolume()
{
    return m_player->getReverbOutputVolume();
}




// Delay methods
void Track::setDelayTime(float time)
{
    if (time >= 0.0f && time <= 1.0f) {
        m_player->setDelayTime(time);
    }

}

float Track::getDelayTime()
{
    return m_player->getDelayTime();
}

void Track::setDelayFeedback(float feedback)
{
    if (feedback >= 0.0f && feedback <= 0.9f) {
        m_player->setDelayFeedback(feedback);
    }
}

float Track::getDelayFeedback()
{
    return m_player->getDelayFeedback();
}

void Track::setDelayMixLevel(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setDelayMixLevel(mix);
    }
}

float Track::getDelayMixLevel()
{
    return m_player->getDelayFeedback();
}


void Track::setDelayOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setDelayOutputVolume(volume);
    }
}

float Track::getDelayOutputVolume()
{
    return m_player->getDelayOutputVolume();
}





// Chorus methods
void Track::setChorusRate(float rate)
{
    if (rate >= 0.1f && rate <= 10.0f) {
        m_player->setChorusRate(rate);
    }
}

float Track::getChorusRate()
{
    return m_player->getChorusRate();
}

void Track::setChorusDepth(float depth)
{
    if (depth >= 0.0f && depth <= 1.0f) {
        m_player->setChorusDepth(depth);
    }
}

float Track::getChorusDepth()
{
    return m_player->getChorusDepth();
}

void Track::setChorusCenterDelay(float delay)
{
    if (delay >= 1.0f && delay <= 50.0f) {
        m_player->setChorusCenterDelay(delay);
    }
}

float Track::getChorusCenterDelay()
{
    return m_player->getChorusCenterDelay();
}

void Track::setChorusFeedback(float feedback)
{
    if (feedback >= 0.0f && feedback <= 0.5f) {
        m_player->setChorusFeedback(feedback);
    }
}

float Track::getChorusFeedback()
{
    return m_player->getChorusFeedback();
}

void Track::setChorusMix(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setChorusMix(mix);
    }

}

float Track::getChorusMix()
{
    return m_player->getChorusMix();
}

void Track::setChorusOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setChorusOutputVolume(volume);
    }
}

float Track::getChorusOutputVolume()
{
    return m_player->getChorusOutputVolume();
}




// Distortion methods
void Track::setDistortionDrive(float drive)
{
    if (drive >= 1.0f && drive <= 100.0f) {
        m_player->setDistortionDrive(drive);
    }
}

float Track::getDistortionDrive()
{
    return m_player->getDistortionDrive();
}

void Track::setDistortionMix(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setDistortionMix(mix);
    }
}

float Track::getDistortionMix()
{
    return m_player->getDistortionMix();
}


void Track::setDistortionOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setDistortionOutputVolume(volume);
    }
}

float Track::getDistortionOutputVolume()
{
    return m_player->getDistortionOutputVolume();
}



void Track::play()
{
    m_player->play();
}

void Track::stop()
{
    m_player->stop();
}


ITrackPlayer* Track::getPlayer(){
    return m_player.get();
}



