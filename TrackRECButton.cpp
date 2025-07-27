#include "TrackRECButton.h"

TrackRECButton::TrackRECButton (QWidget* parent, bool is_pressed, uint width,  uint height)
    : QPushButton(parent)
    , m_is_pressed(is_pressed)
{
    setFixedSize(width, height);
    setStyleSheet("background-color: #cdcdcd; border-radius: 3px;");
}

void TrackRECButton::setRECState(bool state){
    m_is_pressed = state;
    if (m_is_pressed){
        setStyleSheet("background-color: #ff5050; border-radius: 3px;");
    }
    else {
        setStyleSheet("background-color: #cdcdcd; border-radius: 3px;");
    }
}



void TrackRECButton::mousePressEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if (!m_is_pressed){
            emit(changedState(true));
            m_is_pressed = true;
            setStyleSheet("background-color: #ff5050; border-radius: 3px;");
        }
        else {
            m_is_pressed = false;
        }
    }

    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mousePressEvent(event);
    }
}

void TrackRECButton::mouseReleaseEvent(QMouseEvent *event){
    if(event->button() == Qt::LeftButton){
        if (!m_is_pressed){
            emit(changedState(false));
            m_is_pressed = false;
            setStyleSheet("background-color: #cdcdcd; border-radius: 3px;");
        }
    }
    if (event->button() != Qt::LeftButton && event->button() != Qt::RightButton) {
        QPushButton::mouseReleaseEvent(event);
    }
}
