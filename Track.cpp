#include "Track.h"

#define BORDER_RADIUS_COEF 10

Track::Track(MicroTimer* timer, QWidget *parent, bool is_loop, float volume, ushort beats_per_measure, QString sound_path, const QColor& background_color)
    : QPushButton(parent),
    m_is_loop(is_loop),
    m_is_active(false),
    m_beats_per_measure(beats_per_measure),
    m_player (new QMediaPlayer(this)),
    m_audioOutput(new QAudioOutput(this)),
    m_settingsWindow(new TrackSettings(is_loop, static_cast<ushort>(volume)*100, beats_per_measure, background_color, this)),
    m_style("#Track {background-color: %1; color: %2; border-radius: %3px;}"),
    m_color(background_color),
    m_timer(timer)
{
    setObjectName("Track");
    setStyleSheet(m_style.arg(background_color.name()).arg("black").arg(width()/BORDER_RADIUS_COEF));

    m_player->setSource(QUrl::fromLocalFile(sound_path));
    m_player->setAudioOutput(m_audioOutput);
    m_audioOutput->setVolume(1.0);

    connect(m_settingsWindow, &TrackSettings::changedLoopState, this, &Track::setLoopState);
    connect(m_settingsWindow, &TrackSettings::changedVolume, this, &Track::setVolume);
    connect(m_settingsWindow, &TrackSettings::changedBeatsPerMeasure, this, &Track::setBeatsPerMeasure);
    connect( m_settingsWindow, &TrackSettings::changedBackgroundColor, this, &Track::setBackgroundColor);
    connect(m_settingsWindow, &TrackSettings::changedSoundPath, this, &Track::setSoundPath);
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
                connect(m_timer, MicroTimer::tick1, this, play);
                connect(m_timer, MicroTimer::tick3, this, play);
                connect(m_timer, MicroTimer::tick5, this, play);
                connect(m_timer, MicroTimer::tick7, this, play);
            }
            else{
                disconnect(m_timer, MicroTimer::tick1, this, play);
                disconnect(m_timer, MicroTimer::tick3, this, play);
                disconnect(m_timer, MicroTimer::tick5, this, play);
                disconnect(m_timer, MicroTimer::tick7, this, play);
                m_is_active = false;
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

void Track::setBeatsPerMeasure(int beats_per_measure){
    m_beats_per_measure = beats_per_measure;
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





void Track::play(){
    m_player->stop();
    m_player->play();
}





