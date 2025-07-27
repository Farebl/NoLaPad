#include "REC.h"
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

REC::REC(QWidget *parent, uint width, uint height, QString recording_format)
    : QWidget(parent),
    m_ffmpeg_process(new QProcess(this)),
    m_recording_format(recording_format),
    m_current_output_file("")
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");
}

void REC::startRecording() {
    /*логіка запису у файл формату mp3 або wav*/
}

void REC::stopRecording() {
    /*логіка припиення запису у файл відповідного формату*/
}

void REC::setRecordingFormat(const QString& format) {
    if (format == "WAV" || format == "MP3") {
        m_recording_format = format;
    }
}
