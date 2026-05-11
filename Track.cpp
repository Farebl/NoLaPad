
#include <QTime>
#include <QDebug>
#include "Track.h"


#define BORDER_RADIUS_COEF 10

Track::Track(
    ITrackPlayer* player,
    MicroTimer* timer,
    float volume,
    bool is_loop,
    bool is_recording,
    qint16 whole_tackt_lag,
    qint16 whole_tackt_duration,
    std::array<bool, 16> beats_per_measure,
    const QString& sound_path,
    const QColor& outer_background_color,
    const QColor& inner_active_background_color,
    QWidget *parent
    ):
      QPushButton(parent)
    , m_is_active(false)
    , m_is_loop(is_loop)
    , m_is_recording(is_recording)
    , m_is_ready(false)
    , m_whole_tackt_lag(whole_tackt_lag)
    , m_whole_tackt_lag_fixed(whole_tackt_lag)
    , m_whole_tackt_duration(whole_tackt_duration)
    , m_whole_tackt_duration_fixed(whole_tackt_duration)
    , m_audio_sample_path(sound_path)
    , m_style("background-color: %1; color: %2; border-radius: %3px; margin: 2px;")
    , m_outer_color(outer_background_color)
    , m_inner_color(inner_active_background_color)
    , m_timer(timer)
    , m_player(player)
    , m_beats_per_measure(beats_per_measure)
{
     setStyleSheet(
        "QPushButton { outline: none; border: none; background: transparent; }"
        "QPushButton:focus { border: none; outline: none; }"
        );
    setStyleSheet(m_style.arg(m_outer_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));
    setAudioSamplePath(sound_path);
    setVolume(volume);
}

Track::~Track() {}

void Track::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        if (!m_player->hasFileLoaded()){
            QMessageBox::information(this, "Error", "Треку не назначено звук для відтворювання");
            return;
        }

        if (!m_is_loop) {
            m_is_active = true;
            m_is_ready = true;
            update();
            play();
        }
        else {
            if (!m_is_active) {
                m_is_active = true;
                update();
                if (m_timer) {
                    auto measures_count = m_beats_per_measure.size();
                    for(qsizetype i = 0; i < measures_count; ++i){
                        if (m_beats_per_measure[i]){
                            disconnect(m_timer, m_timer->m_signals[i], this, &Track::play);
                        }
                    }

                    if (m_whole_tackt_lag_fixed > 0){
                        qDebug()<<"start with lag_fixed: "<< m_whole_tackt_lag_fixed;
                        m_whole_tackt_lag = m_whole_tackt_lag_fixed;
                        m_is_ready = false;
                        connect(m_timer, m_timer->m_signals[0], this, &Track::lagCountdownPlayConnect);
                    }
                    else{
                        qDebug()<<"start immediately";
                        m_is_ready = true;
                        auto measures_count = m_beats_per_measure.size();
                        for(qsizetype i = 0; i < measures_count; ++i){
                            if (m_beats_per_measure[i]){
                                connect(m_timer, m_timer->m_signals[i], this, &Track::play);
                            }
                        }
                    }
                }
            }
            else {
                qDebug()<<"stop";
                m_is_active = false;
                if (m_timer) {
                    disconnect(m_timer, m_timer->m_signals[0], this, &Track::lagCountdownPlayConnect);
                    disconnect(m_timer, m_timer->m_signals[0], this, &Track::durationCountdownPlay);
                    disconnect(m_timer, m_timer->m_signals[0], this, &Track::play);

                    connect(m_timer, m_timer->m_signals[0], this, &Track::stop);
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
            m_is_ready = false;
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




void Track::lagCountdownPlayConnect(){
    qDebug()<<"lagCountdownPlayConnect (lag: "<<m_whole_tackt_lag<<")";
    if (m_whole_tackt_lag > 1){
        --m_whole_tackt_lag;
    }
    else if (m_whole_tackt_lag == 1){ // preparatory stage 1 beat before the start
        auto measures_count = m_beats_per_measure.size();
        for(qsizetype i = 0; i < measures_count; ++i){
            if (m_beats_per_measure[i]){
                connect(m_timer, m_timer->m_signals[i], this, &Track::play);
            }
        }
        --m_whole_tackt_lag;
    }
    else if (m_whole_tackt_lag < 1){
        m_is_ready = true;
        disconnect(m_timer, m_timer->m_signals[0], this, &Track::lagCountdownPlayConnect);

        connect(m_timer, m_timer->m_signals[0], this, &Track::durationCountdownPlay);
        // The end of the lag and the start of the duration should occur at the same time,
        // but the connection above will only be processed on the next tackt, but during
        // this next tackt the track is already playing (m_is_ready == true ) so to compensate
        // for this delay we will reduce the duration by 1.
        m_whole_tackt_duration = m_whole_tackt_duration_fixed - 1;
    }
}

void Track::durationCountdownPlay(){
    qDebug()<<"durationCountdownPlay (duration: "<<m_whole_tackt_duration<<")";
    if (!m_is_ready) {return;}

    if (m_whole_tackt_duration > 0){
        --m_whole_tackt_duration;
    }
    else if(m_whole_tackt_duration < 1){
        stop();
    }
}


void Track::play()
{
    qDebug()<<"Track::paly (m_is_ready: "<<m_is_ready<<")";
    if (m_is_ready == true){
        m_player->play();
    }
}

void Track::stop() {
    qDebug()<<"Track::stop";
    disconnect(m_timer, m_timer->m_signals[0], this, &Track::stop);
    m_player->stop();
    auto measures_count = m_beats_per_measure.size();

    for(qsizetype i = 0; i < measures_count; ++i){
        if (m_beats_per_measure[i]){
            disconnect(m_timer, m_timer->m_signals[i], this, &Track::play);
        }
    }

    m_is_active = false;
    m_is_ready = false;
    disconnect(m_timer, m_timer->m_signals[0], this, &Track::durationCountdownPlay);
    update();
}




void Track::setWholeTacktLag(qint16 value)
{
    m_whole_tackt_lag_fixed = value;
    m_whole_tackt_lag = m_whole_tackt_lag_fixed;
    qDebug()<<"Track::setWholeTacktLag fixed: "<< m_whole_tackt_lag_fixed;
}

qint16 Track::getWholeTacktLag() const {
    return m_whole_tackt_lag_fixed;
}


void Track::setWholeTacktDuration(qint16 value)
{
    m_whole_tackt_duration_fixed = value;
    m_whole_tackt_duration = m_whole_tackt_duration_fixed;
    qDebug()<<"Track::setWholeTacktDuration fixed: "<< m_whole_tackt_duration_fixed;
}

qint16 Track::getWholeTacktDuration() const {
    return m_whole_tackt_duration_fixed;
}




void Track::setVolume(float volume)
{
    m_player->setVolume(volume);
}


void Track::setAudioSamplePath(const QString& path)
{
    m_audio_sample_path = path;
    if (!m_audio_sample_path.isEmpty()) {
        m_player->loadAudioFileForPlaying(path);

        m_is_active = false;
        stop();
        return;
    }
}

QString Track::getAudioSamplePath() const
{
    return m_audio_sample_path;
}


bool Track::getLoopState() const
{
    return m_is_loop;
}

void Track::setLoopState(bool state)
{
    m_is_loop = state;
    if (!m_is_loop){
        disconnect(m_timer, m_timer->m_signals[0], this, &Track::lagCountdownPlayConnect);
        disconnect(m_timer, m_timer->m_signals[0], this, &Track::durationCountdownPlay);
        disconnect(m_timer, m_timer->m_signals[0], this, &Track::play);
        connect(m_timer, m_timer->m_signals[0], this, &Track::stop);
    };
}


void Track::setRecordingState(bool state)
{
    m_is_recording = state;
    m_player->setRecordingEnabled(state);
}

bool Track::getRecordingState() const
{
    return m_is_recording;
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

QString Track::getSoundPath() const
{
    return m_audio_sample_path;
}

std::array<bool, 16> Track::getBeatsStates() const
{
    return m_beats_per_measure;
}

void Track::setBeatState(quint8 index, bool state)
{
    qDebug()<<"Track::changedBeatState --> "<<index<<": "<<state;

    m_beats_per_measure[index] = state;

    if (m_beats_per_measure[index]){
        if (m_is_active){
            connect(
                m_timer,
                m_timer->m_signals[index],
                this,
                &Track::play);
        }
    }
    else {
        disconnect(m_timer, m_timer->m_signals[index], this, &Track::play);
    }
}

float Track::getVolume() const
{
    return m_player->getVolume();
}


QColor Track::getInnerActiveBackgroundColor() const
{
    return m_inner_color;
}

QColor Track::getOuterBackgroundColor() const
{
    return m_outer_color;
}


void Track::setCurrentEffectType(EffectType type)
{

    m_player->setEffectType(type);
}

EffectType Track::getEffectType() const
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

float Track::getReverbRoomSize() const
{
    return m_player->getReverbRoomSize();
}

void Track::setReverbDamping(float damping)
{
    if (damping >= 0.0f && damping <= 1.0f) {
        m_player->setReverbDamping(damping);
    }

}

float Track::getReverbDamping() const
{
    return m_player->getReverbDamping();
}


void Track::setReverbWetLevel(float wet)
{
    if (wet >= 0.0f && wet <= 1.0f) {
        m_player->setReverbWetLevel(wet);
    }

}

float Track::getReverbWetLevel() const
{

    return m_player->getReverbWetLevel();
}

void Track::setReverbDryLevel(float dry)
{
    if (dry >= 0.0f && dry <= 1.0f) {
        m_player->setReverbDryLevel(dry);
    }

}

float Track::getReverbDryLevel() const
{
    return m_player->getReverbDryLevel();
}



void Track::setReverbOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setReverbOutputVolume(volume);
    }
}

float Track::getReverbOutputVolume() const
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

float Track::getDelayTime() const
{
    return m_player->getDelayTime();
}

void Track::setDelayFeedback(float feedback)
{
    if (feedback >= 0.0f && feedback <= 0.9f) {
        m_player->setDelayFeedback(feedback);
    }
}

float Track::getDelayFeedback() const
{
    return m_player->getDelayFeedback();
}

void Track::setDelayMixLevel(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setDelayMixLevel(mix);
    }
}

float Track::getDelayMixLevel() const
{
    return m_player->getDelayFeedback();
}


void Track::setDelayOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setDelayOutputVolume(volume);
    }
}

float Track::getDelayOutputVolume() const
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

float Track::getChorusRate() const
{
    return m_player->getChorusRate();
}

void Track::setChorusDepth(float depth)
{
    if (depth >= 0.0f && depth <= 1.0f) {
        m_player->setChorusDepth(depth);
    }
}

float Track::getChorusDepth() const
{
    return m_player->getChorusDepth();
}

void Track::setChorusCenterDelay(float delay)
{
    if (delay >= 1.0f && delay <= 50.0f) {
        m_player->setChorusCenterDelay(delay);
    }
}

float Track::getChorusCenterDelay() const
{
    return m_player->getChorusCenterDelay();
}

void Track::setChorusFeedback(float feedback)
{
    if (feedback >= 0.0f && feedback <= 0.5f) {
        m_player->setChorusFeedback(feedback);
    }
}

float Track::getChorusFeedback() const
{
    return m_player->getChorusFeedback();
}

void Track::setChorusMix(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setChorusMix(mix);
    }

}

float Track::getChorusMix() const
{
    return m_player->getChorusMix();
}

void Track::setChorusOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setChorusOutputVolume(volume);
    }
}

float Track::getChorusOutputVolume() const
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

float Track::getDistortionDrive() const
{
    return m_player->getDistortionDrive();
}

void Track::setDistortionMix(float mix)
{
    if (mix >= 0.0f && mix <= 1.0f) {
        m_player->setDistortionMix(mix);
    }
}

float Track::getDistortionMix() const
{
    return m_player->getDistortionMix();
}


void Track::setDistortionOutputVolume(float volume)
{
    if (volume >= 0.0f && volume <= 1.0f) {
        m_player->setDistortionOutputVolume(volume);
    }
}

float Track::getDistortionOutputVolume() const
{
    return m_player->getDistortionOutputVolume();
}



ITrackPlayer* Track::getPlayer() const
{
    return m_player.get();
}



