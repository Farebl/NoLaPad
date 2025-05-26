#include "Track.h"

#define BORDER_RADIUS_COEF 10

Track::Track(MicroTimer* timer, QWidget *parent, float volume, bool is_loop, std::vector<std::vector<bool>> beats_per_measure, QString sound_path, const QColor& outer_background_color, const QColor& inner_background_color)
    : QPushButton(parent),
    m_is_loop(is_loop),
    m_is_active(false),
    m_player (new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_style("#Track {background-color: %1; color: %2; border-radius: %3px;}"),
    m_outer_color(outer_background_color),
    m_inner_color(outer_background_color),
    m_timer(timer),
    m_beats_per_measure(beats_per_measure)
{
    setObjectName("Track");
    setStyleSheet(m_style.arg(m_outer_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));

    if (m_beats_per_measure[0].size() < 4){
        m_beats_per_measure[0].resize(4, false);
    }
    if (m_beats_per_measure[1].size() < 4){
        m_beats_per_measure[1].resize(4, false);
    }
    if (m_beats_per_measure[2].size() < 4){
        m_beats_per_measure[2].resize(4, false);
    }
    if (m_beats_per_measure[3].size() < 4){
        m_beats_per_measure[3].resize(4, false);
    }


    m_player->setSource(QUrl::fromLocalFile(sound_path));
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(volume);
}


Track::~Track() {
    if (m_player) {
        m_player->stop();
        delete m_player;
        m_player = nullptr;
    }
    if (m_audioOutput) {
        delete m_audioOutput;
        m_audioOutput = nullptr;
    }
}

void Track::mousePressEvent(QMouseEvent *event){

    if(event->button() == Qt::LeftButton){

        if (m_player->source().isEmpty()){
            QMessageBox::information(this, "Error", "Треку не назначено звук для відтворювання");
            return;
        }

        if (!m_is_loop){
            m_is_active = true;
            update();
            play();
        }
        else{ // is loop
            if (!m_is_active){
                m_is_active = true;
                update();
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
            else{
                m_is_active = false;
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

    if(event->button() == Qt::RightButton){
        qDebug()<<"emit from" << this;
        emit(rightClicked(this));
    }




    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mousePressEvent(event);
    }
}

void Track::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if (!m_is_loop){
            m_is_active = false;
        }
        update();
    }

    QPushButton::mouseReleaseEvent(event);
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

    int centerX = width() / 2;
    int centerY = height() / 2;

    int radius;
    if (width()>=height()){
        radius = height() / 6;
    }
    if (width()<height()){
        radius = width() / 6;
    }
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
}


void Track::setLoopState(bool state){
    m_is_loop = state;
    if(m_is_active && !m_is_loop){
        m_is_active = false;
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


void Track::setVolume(int volume_percent){
    if ((volume_percent < 0) || (volume_percent > 100)){
        return;
    }

    m_audioOutput->setVolume(static_cast<double>(volume_percent)/100.0);
}


void Track::setOuterBackgroundColor(QColor color){
    m_outer_color=color;
    update();
}

void Track::setInnerBackgroundColor(QColor color){
    m_inner_color=color;
    update();
}




void Track::setAudioSamplePath(QString path){
    if (path.isEmpty()){
        m_audio_sample_path.clear();
        return;
    }
    m_audio_sample_path = path;
    m_player->setSource(QUrl::fromLocalFile(m_audio_sample_path));
}



void Track::setBeat1(bool state){
    m_beats_per_measure[0][0] = state;
    if (m_beats_per_measure[0][0] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick1, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
}

void Track::setBeat2(bool state){
    m_beats_per_measure[0][1] = state;
    if (m_beats_per_measure[0][1] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick2, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick2, this, &Track::play);
}
void Track::setBeat3(bool state){
    m_beats_per_measure[0][2] = state;
    if (m_beats_per_measure[0][2] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick3, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick3, this, &Track::play);
}

void Track::setBeat4(bool state){
    m_beats_per_measure[0][3] = state;
    if (m_beats_per_measure[0][3] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick4, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick4, this, &Track::play);
}

void Track::setBeat5(bool state){
    m_beats_per_measure[1][0] = state;
    if (m_beats_per_measure[1][0] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick5, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick5, this, &Track::play);
}

void Track::setBeat6(bool state){
    m_beats_per_measure[1][1] = state;
    if (m_beats_per_measure[1][1] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick6, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick6, this, &Track::play);
}

void Track::setBeat7(bool state){
    m_beats_per_measure[1][2] = state;
    if (m_beats_per_measure[1][2] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick7, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick7, this, &Track::play);
}

void Track::setBeat8(bool state){
    m_beats_per_measure[1][3] = state;
    if (m_beats_per_measure[1][3] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick8, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick8, this, &Track::play);
}

void Track::setBeat9(bool state){
    m_beats_per_measure[2][0] = state;
    if (m_beats_per_measure[2][0] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick9, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick9, this, &Track::play);
}

void Track::setBeat10(bool state){
    m_beats_per_measure[2][1] = state;
    if (m_beats_per_measure[2][1] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick10, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick10, this, &Track::play);
}

void Track::setBeat11(bool state){
    m_beats_per_measure[2][2] = state;
    if (m_beats_per_measure[2][2] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick11, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick11, this, &Track::play);
}

void Track::setBeat12(bool state){
    m_beats_per_measure[2][3] = state;
    if (m_beats_per_measure[2][3] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick12, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick12, this, &Track::play);
}

void Track::setBeat13(bool state){
    m_beats_per_measure[3][0] = state;
    if (m_beats_per_measure[3][0] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick13, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick13, this, &Track::play);
}

void Track::setBeat14(bool state){
    m_beats_per_measure[3][1] = state;
    if (m_beats_per_measure[3][1] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick14, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick14, this, &Track::play);
}

void Track::setBeat15(bool state){
    m_beats_per_measure[3][2] = state;
    if (m_beats_per_measure[3][2] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick15, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick15, this, &Track::play);
}

void Track::setBeat16(bool state){
    m_beats_per_measure[3][3] = state;
    if (m_beats_per_measure[3][3] && m_is_loop && m_is_active )
        connect(m_timer, &MicroTimer::tick16, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick16, this, &Track::play);
}


void Track::play(){
    m_player->stop();
    m_player->play();
}


bool Track::getLoopState(){
    return m_is_loop;
}

float Track::getVolume(){
    return m_audioOutput->volume();
}

QColor Track::getInnerBackgroundColor(){
    return m_inner_color;
}
QColor Track::getOuterBackgroundColor(){
    return m_outer_color;
}
QString Track::getSoundPath(){
    return m_audio_sample_path;
}
std::vector<std::vector<bool>> Track::getBeats(){
    return m_beats_per_measure;
}



