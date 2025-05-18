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
    m_ffmpegProcess(new QProcess(this)),
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
    QString ffmpegPath = "ffmpeg"; // На Ubuntu ffmpeg викликається без повного шляху, якщо встановлено системно
    QString recordingsDir = appDir + "/../records";

    // Перевірка існування ffmpeg
    qDebug() << "Checking for FFmpeg...";
    int whichResult = QProcess::execute("which ffmpeg");
    qDebug() << "'which ffmpeg' result:" << whichResult;
    if (whichResult != 0) {
        qDebug() << "FFmpeg not found with 'which'. Trying direct execution...";
        QProcess testProcess;
        testProcess.start(ffmpegPath, QStringList() << "-version");
        if (!testProcess.waitForStarted(3000) || !testProcess.waitForFinished(3000)) {
            qDebug() << "FFmpeg executable not found in system path. Please install FFmpeg.";
            return;
        }
    }

    // Створення папки records, якщо вона не існує
    QDir dir;
    if (!dir.exists(recordingsDir)) {
        if (!dir.mkpath(recordingsDir)) {
            qDebug() << "Failed to create recordings directory:" << recordingsDir;
            return;
        }
    }

    // Перевірка прав доступу до папки
    QFileInfo dirInfo(recordingsDir);
    if (!dirInfo.isWritable()) {
        qDebug() << "No write permissions for directory:" << recordingsDir;
        return;
    }

    // Формування повного шляху до файлу
    m_currentOutputFile = QString("%1/recording_%2.%3")
                              .arg(recordingsDir)
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                              .arg(m_recording_format.toLower());

    QStringList arguments;
#ifdef Q_OS_LINUX
    // Захоплення вихідного звуку через PulseAudio monitor
    // Виконай: pactl list sources
    // Знайди джерело типу "alsa_output...monitor" (наприклад, "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor")
    // Заміни "alsa_output..." на твоє джерело
    arguments << "-f" << "pulse"
              << "-i" << "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor"
              << "-c:a" << (m_recording_format == "MP3" ? "mp3" : "pcm_s16le")
              << "-y" << m_currentOutputFile;
#elif defined(Q_OS_WIN)
    // Захоплення системного звуку через dshow (для Windows)
    arguments << "-f" << "dshow"
              << "-i" << "audio=\"Stereo Mix (Realtek High Definition Audio)\""
              << "-c:a" << (m_recording_format == "MP3" ? "mp3" : "pcm_s16le")
              << "-y" << m_currentOutputFile;
#else
    qDebug() << "Unsupported platform for audio recording";
    return;
#endif

    // Налаштування шляху до ffmpeg
    m_ffmpegProcess->setProgram(ffmpegPath);

    // Додавання детального виведення помилок FFmpeg
    connect(m_ffmpegProcess, &QProcess::readyReadStandardError, this, [this]() {
        qDebug() << "FFmpeg error:" << m_ffmpegProcess->readAllStandardError();
    });

    connect(m_ffmpegProcess, &QProcess::readyReadStandardOutput, this, [this]() {
        qDebug() << "FFmpeg output:" << m_ffmpegProcess->readAllStandardOutput();
    });

    m_ffmpegProcess->setArguments(arguments);
    m_ffmpegProcess->start();
    if (!m_ffmpegProcess->waitForStarted(3000)) {
        qDebug() << "FFmpeg failed to start:" << m_ffmpegProcess->errorString();
        qDebug() << "Command:" << ffmpegPath << arguments.join(" ");
    } else {
        qDebug() << "FFmpeg started successfully, recording to:" << m_currentOutputFile;
    }
}

void REC::stopRecording() {
    if (m_ffmpegProcess->state() == QProcess::Running) {
        m_ffmpegProcess->terminate();
        m_ffmpegProcess->waitForFinished(3000);
        if (m_ffmpegProcess->state() != QProcess::NotRunning) {
            m_ffmpegProcess->kill();
        }
        qDebug() << "FFmpeg stopped.";
    }

    // Перевірка, чи створено файл запису
    QFileInfo fileInfo(m_currentOutputFile);
    if (fileInfo.exists()) {
        qDebug() << "Recording file created:" << m_currentOutputFile;
    } else {
        qDebug() << "Recording file not found:" << m_currentOutputFile;
    }

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
