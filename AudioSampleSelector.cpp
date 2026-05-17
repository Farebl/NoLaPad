#include "AudioSampleSelector.h"
#include <QFileDialog>
#include <QMouseEvent>
#include <QPainter>

AudioSampleSelector::AudioSampleSelector(const QString& active_state_image_path, const QString& inactive_state_image_path, const QString& title, QWidget *parent)
    : QWidget(parent), m_is_active(false), m_title(title)
{
    QString appDir = QCoreApplication::applicationDirPath(); // Шлях до виконавчого файлу
    QDir dir(appDir);

    dir.cdUp(); // Переходимо на один рівень вгору (батьківська директорія)
    m_path_to_music = dir.absolutePath() + "/music";

    m_active_state_image = QPixmap(active_state_image_path);
    if (m_active_state_image.isNull()) {
        qDebug() << "Failed to load activeState image from:" << active_state_image_path;
    } else {
        m_active_state_image = m_active_state_image.scaled(m_active_state_image.width(), m_active_state_image.height() + 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setMinimumSize(m_active_state_image.width(), m_active_state_image.height()); // Адаптуємо розмір віджета під зображення
    }

    m_inactive_state_image = QPixmap(inactive_state_image_path);
    if (m_inactive_state_image.isNull()) {
        qDebug() << "Failed to load inactiveState image from:" << inactive_state_image_path;
    } else {
        m_inactive_state_image = m_inactive_state_image.scaled(m_inactive_state_image.width(), m_active_state_image.height() + 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setMinimumSize(m_inactive_state_image.width(), m_inactive_state_image.height()/2); // Адаптуємо розмір віджета
    }

    if (!m_active_state_image.isNull()) {
        setFixedSize(m_active_state_image.width(), m_active_state_image.height());
    }
    update();


}

void AudioSampleSelector::setIsAudioSampleSelectedState(bool state){
    m_is_active = state;
    update();
}


void AudioSampleSelector::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QColor("#ebebeb"));
    int font_height = QFontMetrics(painter.font()).height();

    painter.drawText(QRect{0, 20, width(), font_height}, Qt::AlignCenter, m_title);
    // Малюємо зображення залежно від стану
    if (m_is_active && !m_active_state_image.isNull()) {
        painter.drawPixmap(0, 40, m_active_state_image);
    } else if (!m_is_active && !m_inactive_state_image.isNull()) {
        painter.drawPixmap(0, 40, m_inactive_state_image);
    } else {
        painter.fillRect(rect(), Qt::gray); // Показуємо сірий фон, якщо зображення не завантажено
    }
}

void AudioSampleSelector::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_is_active) {
            m_is_active = false;
            emit selectedAudioSample("");
            update();
        } else {

            QFileDialog dialog(this, "Choose MP3 file", m_path_to_music, "Audio Files (*.mp3 *.wav)");;
            dialog.setFileMode(QFileDialog::ExistingFile);
            dialog.setOption(QFileDialog::DontUseNativeDialog); // Вимикаємо нативний діалог
            if (dialog.exec() == QDialog::Accepted) {
                QString path = dialog.selectedFiles().first();
                if (!path.isEmpty()) {
                    emit selectedAudioSample(path);
                    m_is_active = true;
                    update();
                }
            }
        }
    }
    event->accept();
}



