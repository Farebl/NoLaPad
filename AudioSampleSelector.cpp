#include "AudioSampleSelector.h"
#include <QFileDialog>

AudioSampleSelector::AudioSampleSelector(QWidget *parent, QString activeStateImagePath, QString inactiveStateImagePath)
    : QWidget(parent), m_is_active(false)
{
    setMinimumSize(100, 100); // Встановлюємо мінімальний розмір, щоб віджет був видимим
    m_activeStateImage = QPixmap(activeStateImagePath);
    if (m_activeStateImage.isNull()) {
        qDebug() << "Failed to load activeState image from:" << activeStateImagePath;
    } else {
        m_activeStateImage = m_activeStateImage.scaled(m_activeStateImage.width(), m_activeStateImage.height() + 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setMinimumSize(m_activeStateImage.width(), m_activeStateImage.height()); // Адаптуємо розмір віджета під зображення
    }

    m_inactiveStateImage = QPixmap(inactiveStateImagePath);
    if (m_inactiveStateImage.isNull()) {
        qDebug() << "Failed to load inactiveState image from:" << inactiveStateImagePath;
    } else {
        m_inactiveStateImage = m_inactiveStateImage.scaled(m_inactiveStateImage.width(), m_activeStateImage.height() + 8, Qt::KeepAspectRatio, Qt::SmoothTransformation);
        setMinimumSize(m_inactiveStateImage.width(), m_inactiveStateImage.height()); // Адаптуємо розмір віджета
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

    // Малюємо зображення залежно від стану
    if (m_is_active && !m_activeStateImage.isNull()) {
        painter.drawPixmap(0, 0, m_activeStateImage);
    } else if (!m_is_active && !m_inactiveStateImage.isNull()) {
        painter.drawPixmap(0, 0, m_inactiveStateImage);
    } else {
        painter.fillRect(rect(), Qt::gray); // Показуємо сірий фон, якщо зображення не завантажено
    }
}

void AudioSampleSelector::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (m_is_active) {
            m_is_active = false;
            update();
        } else {
            QFileDialog dialog(this, "Choose MP3 file", "../../music", "Audio Files (*.mp3 *.wav)");;
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

