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
    m_elapsed_timer(new QElapsedTimer()),
    m_ffmpeg_process(new QProcess(this)),
    m_recording_format(recording_format),
    m_current_output_file("")
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
            m_elapsed_timer->invalidate();
            m_digital_clock_face->setText("00:00:00:0");
            m_REC_button->setChecked(false);
            stopRecording();
            emit recordingStopped();
        } else {
            m_REC_button->setChecked(true);
            m_timer->start();
            m_elapsed_timer->start();
            startRecording();
            emit recordingStarted();
        }
    });

    connect(m_timer, &QTimer::timeout, this, &REC::updateDigitalClockFace);
}

void REC::startRecording() {
    // Отримання шляху до директорії виконуваного файлу
    QString appDir = QCoreApplication::applicationDirPath();
    QString ffmpeg_path = "ffmpeg"; // На Ubuntu ffmpeg викликається без повного шляху, якщо встановлено системно
    QString recordingsDir = appDir + "/../records";

    // Перевірка існування ffmpeg
    qDebug() << "Checking for FFmpeg...";
    int whichResult = QProcess::execute("which ffmpeg");
    qDebug() << "'which ffmpeg' result:" << whichResult;
    if (whichResult != 0) {
        qDebug() << "FFmpeg not found with 'which'. Trying direct execution...";
        QProcess testProcess;
        testProcess.start(ffmpeg_path, QStringList() << "-version");
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
    m_current_output_file = QString("%1/recording_%2.%3")
                              .arg(recordingsDir)
                              .arg(QDateTime::currentDateTime().toString("yyyyMMdd_HHmmss"))
                              .arg(m_recording_format.toLower());

    QStringList arguments;
#ifdef Q_OS_LINU_x
    // Захоплення вихідного звуку через PulseAudio monitor
    // Виконай: pactl list sources
    // Знайди джерело типу "alsa_output...monitor" (наприклад, "alsa_output.pci-0000_00_1b.0.analog-stereo.monitor")
    // Заміни "alsa_output..." на твоє джерело
    arguments << "-f" << "pulse"
              << "-i" << "alsa_output.pci-0000_00_1f.3.analog-stereo.monitor"
              << "-c:a" << (m_recording_format == "MP3" ? "mp3" : "pcm_s16le")
              << "-y" << m_current_output_file;
#elif defined(Q_OS_WIN)
    // Захоплення системного звуку через dshow (для Windows)
    arguments << "-f" << "dshow"
              << "-i" << "audio=\"Stereo Mix (Realtek High Definition Audio)\""
              << "-c:a" << (m_recording_format == "MP3" ? "mp3" : "pcm_s16le")
              << "-y" << m_current_output_file;
#else
    qDebug() << "Unsupported platform for audio recording";
    return;
#endif

    // Налаштування шляху до ffmpeg
    m_ffmpeg_process->setProgram(ffmpeg_path);

    // Додавання детального виведення помилок FFmpeg
    connect(m_ffmpeg_process, &QProcess::readyReadStandardError, this, [this]() {
        qDebug() << "FFmpeg error:" << m_ffmpeg_process->readAllStandardError();
    });

    connect(m_ffmpeg_process, &QProcess::readyReadStandardOutput, this, [this]() {
        qDebug() << "FFmpeg output:" << m_ffmpeg_process->readAllStandardOutput();
    });

    m_ffmpeg_process->setArguments(arguments);
    m_ffmpeg_process->start();
    if (!m_ffmpeg_process->waitForStarted(3000)) {
        qDebug() << "FFmpeg failed to start:" << m_ffmpeg_process->errorString();
        qDebug() << "Command:" << ffmpeg_path << arguments.join(" ");
    } else {
        qDebug() << "FFmpeg started successfully, recording to:" << m_current_output_file;
    }
}

void REC::stopRecording() {
    if (m_ffmpeg_process->state() == QProcess::Running) {
        m_ffmpeg_process->terminate();
        m_ffmpeg_process->waitForFinished(3000);
        if (m_ffmpeg_process->state() != QProcess::NotRunning) {
            m_ffmpeg_process->kill();
        }
        qDebug() << "FFmpeg stopped.";
    }

    // Перевірка, чи створено файл запису
    QFileInfo file_info(m_current_output_file);
    if (file_info.exists()) {
        qDebug() << "Recording file created:" << m_current_output_file;
    } else {
        qDebug() << "Recording file not found:" << m_current_output_file;
    }

    // Очистка імені файлу після завершення запису
    m_current_output_file = "";
}

void REC::setRecordingFormat(const QString& format) {
    if (format == "WAV" || format == "MP3") {
        m_recording_format = format;
    }
}

void REC::updateDigitalClockFace() {
    QTime time(0, 0);
    qint64 elapsed = m_elapsed_timer->elapsed();
    time = time.addMSecs(elapsed);
    m_digital_clock_face->setText(time.toString("HH:mm:ss:%1").arg(time.msec() / 100));

    if (elapsed >= 359999999) { // 99:59:59:999 у мілісекундах
        m_timer->stop();
        m_elapsed_timer->invalidate();
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
