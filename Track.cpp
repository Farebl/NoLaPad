#include "Track.h"

#define BORDER_RADIUS_COEF 10

Track::Track(QWidget *parent, juce::AudioDeviceManager& deviceManager, juce::MixerAudioSource& mixer, MicroTimer* timer, float volume, bool is_recording, bool is_loop, std::vector<std::vector<bool>> beats_per_measure, QString sound_path, const QColor& outer_background_color, const QColor& inner_active_background_color)
    : QPushButton(parent),
    m_device_manager(deviceManager),
    m_mixer_source(mixer),
    m_transport_source(juce::AudioTransportSource()),
    m_is_loop(is_loop),
    m_is_recording(is_recording),
    m_audio_sample_path(sound_path),
    m_is_active(false),
    m_style("background-color: %1; color: %2; border-radius: %3px; margin: 2px; "),
    m_outer_color(outer_background_color),
    m_inner_color(inner_active_background_color),
    m_timer(timer),
    m_beats_per_measure(beats_per_measure)
{
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

    m_format_manager.registerBasicFormats();

    if (!m_audio_sample_path.isEmpty()){
        juce::File audioFile(m_audio_sample_path.toStdString());
        if (!audioFile.existsAsFile() || !audioFile.hasReadAccess()) {
            std::cerr << "Cannot access MP3 file: " << audioFile.getFullPathName().toStdString() << std::endl;
            QMessageBox::information(this, "Error", "Немає доступу до MP3 файлу: " + m_audio_sample_path);
            return;
        }
        auto reader = m_format_manager.createReaderFor(audioFile);
        if (reader == nullptr) {
            std::cerr << "Failed to create reader for MP3 file: " << audioFile.getFullPathName().toStdString() << std::endl;
            QMessageBox::information(this, "Error", "Не вдалося створити ридер для MP3 файлу: " + m_audio_sample_path);
            return;
        }
        m_reader_source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
        m_transport_source.setSource(m_reader_source.get(), 0, nullptr, reader->sampleRate);
        m_mixer_source.addInputSource(&m_transport_source, false);
    }
    if ((volume < 0.0f) || (volume > 1.0f)){
        m_transport_source.setGain(1.0);
    }
    else{
        m_transport_source.setGain(volume);
    }
}


Track::~Track() {
    m_transport_source.stop();
    m_transport_source.setSource(nullptr);
    m_mixer_source.removeInputSource(&m_transport_source);
}




void Track::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if (!m_reader_source){
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
                if (m_timer){
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
            }
            else{
                m_is_active = false;
                if (m_timer){
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

    if(event->button() == Qt::RightButton){
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


void Track::play(){
    m_transport_source.setPosition(0); // Сброс позиции в начало
    m_transport_source.start();
}

void Track::stop(){
    m_transport_source.stop();
    if (m_is_loop){
        disconnect(m_timer, &MicroTimer::tick1, this, &Track::stop);
    }
}



void Track::setVolume(float volume){
    if ((volume < 0.0f) || (volume > 1.0f)){
        return;
    }
    m_transport_source.setGain(volume);
}


void Track::setLoopState(bool state){
    m_is_loop = state;
    if(m_is_active && !m_is_loop){
        m_is_active = false;
        if (m_timer){
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



void Track::setRECState(bool state){
    m_is_recording = state;
    if (m_is_recording){
        /*відправити на запис*/
    }
    else{
        /* зняти з запису*/
    }
}



void Track::setOuterBackgroundColor(QColor color){
    m_outer_color=color;
    update();
}

void Track::setInnerActiveBackgroundColor(QColor color){
    m_inner_color=color;
    update();
}


void Track::setAudioSamplePath(QString path){
    m_audio_sample_path = path;
    if (m_audio_sample_path.isEmpty()){
        m_transport_source.stop();
        m_mixer_source.removeInputSource(&m_transport_source);
        m_transport_source.setSource(nullptr);
        m_reader_source.reset();
        m_is_active = false;
        if (m_timer){
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
        std::cerr << "Cannot access MP3 file: " << audioFile.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Немає доступу до MP3 файлу: " + path);
        return;
    }
    auto reader = m_format_manager.createReaderFor(audioFile);
    if (reader == nullptr) {
        std::cerr << "Failed to create reader for MP3 file: " << audioFile.getFullPathName().toStdString() << std::endl;
        QMessageBox::information(this, "Error", "Не вдалося створити ридер для MP3 файлу: " + path);
        return;
    }
    m_reader_source = std::make_unique<juce::AudioFormatReaderSource>(reader, true);
    m_transport_source.setSource(m_reader_source.get(), 0, nullptr, reader->sampleRate);
    m_mixer_source.addInputSource(&m_transport_source, false);
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




float Track::getVolume(){
    return m_transport_source.getGain();
}


bool Track::getLoopState(){
    return m_is_loop;
}


bool Track::getRECState(){
    return m_is_recording;
}


QColor Track::getInnerActiveBackgroundColor(){
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
