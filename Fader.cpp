#include "Fader.h"

Fader::Fader(QString track_image_path, QString handle_image_path, QString measures_image_path, QWidget *parent)
    : QSlider(Qt::Vertical, parent)
    , m_handle_width(20)
    , m_handle_height(20)
    , m_dragging(false)
    , m_track_x(0)
    , m_measures_x(0)
    , m_measures_y(0)
{
    setRange(0, 100);
    setValue(50);
    setStyleSheet("QSlider::groove:vertical { background: transparent; }"
                  "QSlider::handle:vertical { background: transparent; }");
    setMouseTracking(false);

    m_handle_image = QPixmap(handle_image_path);
    m_handle_image = m_handle_image.scaled(m_handle_image.width(), m_handle_image.height()+8);
    if (m_handle_image.isNull()) {
        qDebug() << "Failed to load handle image from:" << handle_image_path;
    } else {
        m_handle_width = m_handle_image.width();
        m_handle_height = m_handle_image.height();
    }
    m_track_image = QPixmap(track_image_path);
    if (m_track_image.isNull()) {
        qDebug() << "Failed to load track image from:" << track_image_path;
    }
    m_measures_image = QPixmap(measures_image_path);
    if (m_measures_image.isNull()) {
        qDebug() << "Failed to load measures image from:" << measures_image_path;
    }
    update();
}


void Fader::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Малюємо фіксовану розмітку поділки
    if (!m_measures_image.isNull()) {
        m_measures_x = 0;
        m_measures_y = 50;
        painter.drawPixmap(m_measures_x, m_measures_y, m_measures_image);
    }
    else {
        painter.fillRect(0, 0, width(), height(), Qt::gray);
    }

    // Малюємо фіксовану доріжку
    if (!m_track_image.isNull()) {
        m_track_x = m_measures_x+m_measures_image.width();
        int y = -((m_track_image.height() - m_measures_image.height())/2) + m_measures_y;
        painter.drawPixmap(m_track_x, y, m_track_image);
    }
    else {
        painter.fillRect(0, 0, width(), height(), Qt::gray);
    }

    // Малюємо повзунок відносно доріжки
    if (!m_handle_image.isNull() && !m_track_image.isNull()) {
        int total_height = m_measures_image.height() - 32; // Висота, котра доступна для повзунка
        int value_range = maximum() - minimum();
        int pos = maximum() - value(); // Інвертуємо для вертикального слайдера

        int y = ((pos * total_height) / value_range); // Позиція повзунка
        int x = m_track_x + (m_track_image.width()/2) - (m_handle_image.width()/2) ; // Центруємо в межах доріжки
        painter.drawPixmap(x, y, m_handle_image);
    }

}

void Fader::resizeEvent(QResizeEvent *event)
{
    QSlider::resizeEvent(event);
    update();
}

void Fader::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_track_image.isNull()) {
        m_dragging = true;
        int total_height = m_measures_image.height() - 32; // Висота, котра доступна для повзунка
        int value_range = maximum() - minimum();
        int y = event->y()-4; // Центруємо хендл
        y = qBound(0, y - m_track_x, total_height); // Обмежуємо позицію в межах доріжки
        int new_value = maximum() - (y * value_range) / total_height;
        setValue(qBound(minimum(), new_value, maximum()));
        update();
    }
    event->accept();
}

void Fader::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && !m_track_image.isNull()) {
        int total_height = m_measures_image.height() - 32; // Висота, котра доступна для повзунка
        int value_range = maximum() - minimum();
        int y = event->y()-4;
        y = qBound(0, y - m_track_x, total_height);
        int new_value = maximum() - (y * value_range) / total_height;
        setValue(qBound(minimum(), new_value, maximum()));
        update();
    }
    event->accept();
}

void Fader::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        m_dragging = false;
    }
    event->accept();
}

void Fader::wheelEvent(QWheelEvent *event)
{
    if (!m_track_image.isNull()) {
        int delta = event->angleDelta().y() / 120;
        int new_value = value() + delta * 1; // ← інверсія напрямку
        setValue(qBound(0, new_value, 100));
        event->accept();
        update();
    } else {
        QSlider::wheelEvent(event);
    }
}

bool Fader::event(QEvent *event){
    if (event->type() == QEvent::HoverMove
        || event->type() == QEvent::HoverEnter
        || event->type() == QEvent::HoverLeave
        ) {
        event->ignore();
        return true; // або false, якщо хочете передати її далі
    }

    return QSlider::event(event);
}
