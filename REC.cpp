#include "REC.h"
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

REC::REC(QWidget *parent, uint width, uint height, QString recording_format)
    : QWidget(parent),
    m_REC_button(new RECButton(this, (width < height) ? width : height)),
    m_digital_clock_face(new QLabel(this)),
    m_timer(new QTimer(this)),
    m_elapsedTimer(new QElapsedTimer()),
    m_recording_format(recording_format),
    m_currentOutputFile("")
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");

    m_REC_button->setCheckable(true);
    m_REC_button->setChecked(false);

    m_timer->setInterval(100);

    m_digital_clock_face->setStyleSheet(QString("font-size: %1px; height: %1px").arg(height));
    m_digital_clock_face->setText("00:00:00:0");
    m_digital_clock_face->setAlignment(Qt::AlignCenter);

    connect(m_REC_button, &RECButton::clicked, this, [this]() {
        if (m_timer->isActive()) {
            m_timer->stop();
            m_elapsedTimer->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_REC_button->setChecked(false);
            stopRecording();
            emit recordingStopped();
        } else {
            m_REC_button->setChecked(true);
            m_timer->start();
            m_elapsedTimer->start();
            startRecording();
            emit recordingStarted();
        }
    });

    connect(m_timer, &QTimer::timeout, this, &REC::updateDigitalClockFace);
}

void REC::startRecording() {
    // Отримання шляху до директорії виконуваного файлу
    QString appDir = QCoreApplication::applicationDirPath();
    QString recordingsDir = appDir + "/../records";
    qDebug() << "Start recording";

    QDir dir;
    if (!dir.exists(recordingsDir)) {
        if (!dir.mkpath(recordingsDir)) {
            qDebug() << "Failed to create recordings directory:" << recordingsDir;
            return;
        }
    }


    QFileInfo dirInfo(recordingsDir);
    if (!dirInfo.isWritable()) {
        qDebug() << "No write permissions for directory:" << recordingsDir;
        return;
    }


    m_currentOutputFile = QString("%1/recording_%2.%3")
                              .arg(recordingsDir)
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                              .arg(m_recording_format.toLower());
}

void REC::stopRecording() {
    qDebug() << "Recording stopped.";
    // Очистка імені файлу після завершення запису
    m_currentOutputFile = "";
}

void REC::setRecordingFormat(const QString& format) {
    if (format == "WAV" || format == "MP3") {
        m_recording_format = format;
    }
}

void REC::updateDigitalClockFace() {
    QTime time(0, 0);
    qint64 elapsed = m_elapsedTimer->elapsed();
    time = time.addMSecs(elapsed);
    m_digital_clock_face->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

    if (elapsed >= 359999999) { // 99:59:59:999 у мілісекундах
        m_timer->stop();
        m_elapsedTimer->invalidate();
        m_digital_clock_face->setText("00:00:00:0");
        m_REC_button->setChecked(false);
        stopRecording();
        emit recordingStopped();
    }
}

void REC::redrawStartAndStopButton() {
    m_REC_button->update();
}

RECButton* REC::getRecButton() { return m_REC_button; }
QLabel* REC::getDigitalClockFace() { return m_digital_clock_face; }
