#include "RedButton.h"

RedButton::RedButton(bool is_pressed, int width, int height, QWidget* parent):
    QPushButton(parent)
    , m_is_pressed(is_pressed)
{
    setFixedSize(width, height);
    setStyleSheet("background-color: #cdcdcd; border-radius: 3px;");
}

void RedButton::setPressed(bool state){
    m_is_pressed = state;
    if (m_is_pressed){
        setStyleSheet("background-color: #ff5050; border-radius: 3px;");
    }
    else {
        setStyleSheet("background-color: #cdcdcd; border-radius: 3px;");
    }
}

bool RedButton::isPressed(){
    return m_is_pressed;
}


void RedButton::mousePressEvent(QMouseEvent *event){
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

void RedButton::mouseReleaseEvent(QMouseEvent *event){
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
