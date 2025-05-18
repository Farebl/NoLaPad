#include "Track.h"

#define BORDER_RADIUS_COEF 10

Track::Track(MicroTimer* timer, QWidget *parent, float volume, bool is_loop, std::vector<bool> beats_per_measure, QString sound_path, const QColor& background_color)
    : QPushButton(parent),
    m_is_loop(is_loop),
    m_is_active(false),
    m_player (new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_settingsWindow(new TrackSettings(static_cast<quint8>(((volume>0)? (volume<101)? volume : 100 : 0)*100), is_loop, beats_per_measure, background_color, this)),
    m_style("#Track {background-color: %1; color: %2; border-radius: %3px;}"),
    m_color(background_color),
    m_timer(timer),
    m_beats_per_measure(beats_per_measure)
{
    setObjectName("Track");
    setStyleSheet(m_style.arg(background_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));

    if (m_beats_per_measure.size() < 16){
        m_beats_per_measure.resize(16, false);
    }

    m_player->setSource(QUrl::fromLocalFile(sound_path));
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0);

    connect(m_settingsWindow, &TrackSettings::changedLoopState, this, &Track::setLoopState);
    connect(m_settingsWindow, &TrackSettings::changedVolume, this, &Track::setVolume);
    connect( m_settingsWindow, &TrackSettings::changedBackgroundColor, this, &Track::setBackgroundColor);
    connect(m_settingsWindow, &TrackSettings::changedSoundPath, this, &Track::setSoundPath);

    connect(m_settingsWindow, &TrackSettings::changedBeat1, this, &Track::setBeat1);
    connect(m_settingsWindow, &TrackSettings::changedBeat2, this, &Track::setBeat2);
    connect(m_settingsWindow, &TrackSettings::changedBeat3, this, &Track::setBeat3);
    connect(m_settingsWindow, &TrackSettings::changedBeat4, this, &Track::setBeat4);
    connect(m_settingsWindow, &TrackSettings::changedBeat5, this, &Track::setBeat5);
    connect(m_settingsWindow, &TrackSettings::changedBeat6, this, &Track::setBeat6);
    connect(m_settingsWindow, &TrackSettings::changedBeat7, this, &Track::setBeat7);
    connect(m_settingsWindow, &TrackSettings::changedBeat8, this, &Track::setBeat8);
    connect(m_settingsWindow, &TrackSettings::changedBeat9, this, &Track::setBeat9);
    connect(m_settingsWindow, &TrackSettings::changedBeat10, this, &Track::setBeat10);
    connect(m_settingsWindow, &TrackSettings::changedBeat11, this, &Track::setBeat11);
    connect(m_settingsWindow, &TrackSettings::changedBeat12, this, &Track::setBeat12);
    connect(m_settingsWindow, &TrackSettings::changedBeat13, this, &Track::setBeat13);
    connect(m_settingsWindow, &TrackSettings::changedBeat14, this, &Track::setBeat14);
    connect(m_settingsWindow, &TrackSettings::changedBeat15, this, &Track::setBeat15);
    connect(m_settingsWindow, &TrackSettings::changedBeat16, this, &Track::setBeat16);
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
                if (m_beats_per_measure[0])
                    connect(m_timer, &MicroTimer::tick1, this, &Track::play);
                if (m_beats_per_measure[1])
                    connect(m_timer, &MicroTimer::tick2, this, &Track::play);
                if (m_beats_per_measure[2])
                    connect(m_timer, &MicroTimer::tick3, this, &Track::play);
                if (m_beats_per_measure[3])
                    connect(m_timer, &MicroTimer::tick4, this, &Track::play);
                if (m_beats_per_measure[4])
                    connect(m_timer, &MicroTimer::tick5, this, &Track::play);
                if (m_beats_per_measure[5])
                    connect(m_timer, &MicroTimer::tick6, this, &Track::play);
                if (m_beats_per_measure[6])
                    connect(m_timer, &MicroTimer::tick7, this, &Track::play);
                if (m_beats_per_measure[7])
                    connect(m_timer, &MicroTimer::tick8, this, &Track::play);
                if (m_beats_per_measure[8])
                    connect(m_timer, &MicroTimer::tick9, this, &Track::play);
                if (m_beats_per_measure[9])
                    connect(m_timer, &MicroTimer::tick10, this, &Track::play);
                if (m_beats_per_measure[10])
                    connect(m_timer, &MicroTimer::tick11, this, &Track::play);
                if (m_beats_per_measure[11])
                    connect(m_timer, &MicroTimer::tick12, this, &Track::play);
                if (m_beats_per_measure[12])
                    connect(m_timer, &MicroTimer::tick13, this, &Track::play);
                if (m_beats_per_measure[13])
                    connect(m_timer, &MicroTimer::tick14, this, &Track::play);
                if (m_beats_per_measure[14])
                    connect(m_timer, &MicroTimer::tick15, this, &Track::play);
                if (m_beats_per_measure[15])
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
        m_settingsWindow->exec();
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
    QPushButton::paintEvent(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);



    QColor complementary_color;
    QColor compColor;
    // Получаем RGB-компоненты исходного цвета
    int r = m_color.red();
    int g = m_color.green();
    int b = m_color.blue();

    // Проверка на низкую насыщенность (сероватый цвет)
    int max = qMax(r, qMax(g, b));
    int min = qMin(r, qMin(g, b));
    int saturationThreshold = 50;
    bool isLowSaturation = (max - min) < saturationThreshold;

    if (isLowSaturation) {
        // Для сероватых цветов возвращаем красный
       complementary_color = QColor(255, 0, 0);
    }
    else{
        compColor;
        if (r > 200 && g < 50 && b < 50) {
            compColor = QColor(0, 255, 0); // Зеленый
        }
        else if (g > 200 && r < 50 && b < 50) {
            compColor = QColor(255, 0, 0); // Красный
        }
        else if (b > 200 && r < 50 && g < 50) {
            compColor = QColor(255, 165, 0); // Оранжевый
        }
        else if (r > 200 && g > 200 && b < 50) {
            compColor = QColor(128, 0, 128); // Фиолетовый
        }
        else if (g > 200 && b > 200 && (r < 50 || r > 200)) {
            compColor = QColor(255, 0, 0); // Красный
        }

        else if (r > 200 && b > 200 && g < 50) {
            compColor = QColor(0, 255, 0); // Зеленый
        }
        else {
            compColor = QColor(255 - r, 255 - g, 255 - b);
        }
    }

    if(m_is_active){
        painter.setBrush(compColor);
    }
    else{
        painter.setBrush(QColor(90, 90, 90));
    }

    painter.setPen(Qt::NoPen);

    int centerX = width() / 2;
    int centerY = height() / 2;

    int radius;
    if (width()>=height()){
        radius = height() / 8;
    }
    if (width()<height()){
        radius = width() / 8;
    }
    painter.drawEllipse(QPoint(centerX, centerY), radius, radius);
 }



void Track::setLoopState(Qt::CheckState state){
    if (state == Qt::CheckState::Checked){
        m_is_loop = true;
    }
    else{
        m_is_loop = false;
    }
}


void Track::setVolume(int volume_percent){
    if ((volume_percent < 0) || (volume_percent > 100)){
        return;
    }

    m_audioOutput->setVolume(static_cast<double>(volume_percent)/100.0);
}


void Track::setBackgroundColor(QColor background_color){
    m_color=background_color;
    setStyleSheet(m_style.arg(m_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));
    update();
}


void Track::setSoundPath(QString path){
    if (path.isEmpty()){
        return;
    }
    m_player->setSource(QUrl::fromLocalFile(path));
}



void Track::setBeat1(bool state){
    m_beats_per_measure[0] = state;
    if (m_beats_per_measure[0])
        connect(m_timer, &MicroTimer::tick1, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick1, this, &Track::play);
}

void Track::setBeat2(bool state){
    m_beats_per_measure[1] = state;
    if (m_beats_per_measure[1])
        connect(m_timer, &MicroTimer::tick2, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick2, this, &Track::play);
}
void Track::setBeat3(bool state){
    m_beats_per_measure[2] = state;
    if (m_beats_per_measure[2])
        connect(m_timer, &MicroTimer::tick3, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick3, this, &Track::play);
}

void Track::setBeat4(bool state){
    m_beats_per_measure[3] = state;
    if (m_beats_per_measure[3])
        connect(m_timer, &MicroTimer::tick4, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick4, this, &Track::play);
}

void Track::setBeat5(bool state){
    m_beats_per_measure[4] = state;
    if (m_beats_per_measure[4])
        connect(m_timer, &MicroTimer::tick5, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick5, this, &Track::play);
}

void Track::setBeat6(bool state){
    m_beats_per_measure[5] = state;
    if (m_beats_per_measure[5])
        connect(m_timer, &MicroTimer::tick6, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick6, this, &Track::play);
}

void Track::setBeat7(bool state){
    m_beats_per_measure[6] = state;
    if (m_beats_per_measure[6])
        connect(m_timer, &MicroTimer::tick7, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick7, this, &Track::play);
}

void Track::setBeat8(bool state){
    m_beats_per_measure[7] = state;
    if (m_beats_per_measure[7])
        connect(m_timer, &MicroTimer::tick8, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick8, this, &Track::play);
}

void Track::setBeat9(bool state){
    m_beats_per_measure[8] = state;
    if (m_beats_per_measure[8])
        connect(m_timer, &MicroTimer::tick9, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick9, this, &Track::play);
}

void Track::setBeat10(bool state){
    m_beats_per_measure[9] = state;
    if (m_beats_per_measure[9])
        connect(m_timer, &MicroTimer::tick10, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick10, this, &Track::play);
}

void Track::setBeat11(bool state){
    m_beats_per_measure[10] = state;
    if (m_beats_per_measure[10])
        connect(m_timer, &MicroTimer::tick11, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick11, this, &Track::play);
}

void Track::setBeat12(bool state){
    m_beats_per_measure[11] = state;
    if (m_beats_per_measure[11])
        connect(m_timer, &MicroTimer::tick12, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick12, this, &Track::play);
}

void Track::setBeat13(bool state){
    m_beats_per_measure[12] = state;
    if (m_beats_per_measure[12])
        connect(m_timer, &MicroTimer::tick13, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick13, this, &Track::play);
}

void Track::setBeat14(bool state){
    m_beats_per_measure[13] = state;
    if (m_beats_per_measure[13])
        connect(m_timer, &MicroTimer::tick14, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick14, this, &Track::play);
}

void Track::setBeat15(bool state){
    m_beats_per_measure[14] = state;
    if (m_beats_per_measure[14])
        connect(m_timer, &MicroTimer::tick15, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick15, this, &Track::play);
}

void Track::setBeat16(bool state){
    m_beats_per_measure[15] = state;
    if (m_beats_per_measure[15])
        connect(m_timer, &MicroTimer::tick16, this, &Track::play);
    else
        disconnect(m_timer, &MicroTimer::tick16, this, &Track::play);
}


void Track::play(){
    m_player->stop();
    m_player->play();
}





