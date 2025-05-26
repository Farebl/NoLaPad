#include "Fader.h"

Fader::Fader(QWidget *parent, QString trackImagePath, QString handleImagePath, QString measuresImagePath)
    : QSlider(Qt::Vertical, parent)
    , m_handleWidth(20)
    , m_handleHeight(20)
    , m_dragging(false)
    , m_trackX(0)
    , m_measuresX(0)
    , m_measuresY(0)
{
    setRange(0, 100);
    setValue(50);
    setStyleSheet("QSlider::groove:vertical { background: transparent; }"
                  "QSlider::handle:vertical { background: transparent; }");
    setMouseTracking(false);

    m_handleImage = QPixmap(handleImagePath);
    m_handleImage = m_handleImage.scaled(m_handleImage.width(), m_handleImage.height()+8);
    if (m_handleImage.isNull()) {
        qDebug() << "Failed to load handle image from:" << handleImagePath;
    } else {
        m_handleWidth = m_handleImage.width();
        m_handleHeight = m_handleImage.height();
    }
    m_trackImage = QPixmap(trackImagePath);
    if (m_trackImage.isNull()) {
        qDebug() << "Failed to load track image from:" << trackImagePath;
    }
    m_measuresImage = QPixmap(measuresImagePath);
    if (m_measuresImage.isNull()) {
        qDebug() << "Failed to load measures image from:" << measuresImagePath;
    }
    update();
}


void Fader::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    // Малюємо фіксовану розмітку поділки
    if (!m_measuresImage.isNull()) {
        m_measuresX = 0;
        m_measuresY = 50;
        painter.drawPixmap(m_measuresX, m_measuresY, m_measuresImage);
    }
    else {
        painter.fillRect(0, 0, width(), height(), Qt::gray);
    }

    // Малюємо фіксовану доріжку
    if (!m_trackImage.isNull()) {
        m_trackX = m_measuresX+m_measuresImage.width();
        int y = -((m_trackImage.height() - m_measuresImage.height())/2) + m_measuresY;
        painter.drawPixmap(m_trackX, y, m_trackImage);
    }
    else {
        painter.fillRect(0, 0, width(), height(), Qt::gray);
    }

    // Малюємо повзунок відносно доріжки
    if (!m_handleImage.isNull() && !m_trackImage.isNull()) {
        int totalHeight = m_measuresImage.height() - 32; // Висота, котра доступна для повзунка
        int valueRange = maximum() - minimum();
        int pos = maximum() - value(); // Інвертуємо для вертикального слайдера

        int y = ((pos * totalHeight) / valueRange); // Позиція повзунка
        int x = m_trackX + (m_trackImage.width()/2) - (m_handleImage.width()/2) ; // Центруємо в межах доріжки
        painter.drawPixmap(x, y, m_handleImage);
    }

}

void Fader::resizeEvent(QResizeEvent *event)
{
    QSlider::resizeEvent(event);
    update();
}

void Fader::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && !m_trackImage.isNull()) {
        m_dragging = true;
        int totalHeight = m_measuresImage.height() - 32; // Висота, котра доступна для повзунка
        int valueRange = maximum() - minimum();
        int y = event->y()-4; // Центруємо хендл
        y = qBound(0, y - m_trackX, totalHeight); // Обмежуємо позицію в межах доріжки
        int newValue = maximum() - (y * valueRange) / totalHeight;
        setValue(qBound(minimum(), newValue, maximum()));
        update();
    }
    event->accept();
}

void Fader::mouseMoveEvent(QMouseEvent *event)
{
    if (m_dragging && !m_trackImage.isNull()) {
        int totalHeight = m_measuresImage.height() - 32; // Висота, котра доступна для повзунка
        int valueRange = maximum() - minimum();
        int y = event->y()-4;
        y = qBound(0, y - m_trackX, totalHeight);
        int newValue = maximum() - (y * valueRange) / totalHeight;
        setValue(qBound(minimum(), newValue, maximum()));
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
    if (!m_trackImage.isNull()) {
        int delta = event->angleDelta().y() / 120;
        int newValue = value() + delta * 1; // ← інверсія напрямку
        setValue(qBound(0, newValue, 100));
        qDebug() << value();
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
