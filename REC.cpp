#include "REC.h"
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

REC::REC(QWidget *parent, uint width, uint height)
    : QWidget(parent),
    m_current_output_file("")
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");

    m_format_manager.registerBasicFormats();
}

REC::~REC()
{
    stopRecording();
}


void REC::startRecording()
{
    m_current_output_file = QCoreApplication::applicationDirPath() + "/recording_" +
                            QDateTime::currentDateTime().toString("yyyyMMdd_hhmmss") + ".wav";

    juce::File outputFile(m_current_output_file.toStdString());
    m_file_stream = outputFile.createOutputStream();

    if (!m_file_stream || m_file_stream->failedToOpen()) {
        qDebug() << "Failed to open file for writing: " << m_current_output_file;
        return;
    }

    juce::AudioFormat* format = m_format_manager.findFormatForFileExtension("wav");

    if (!format) {
        qDebug() << "Unsupported format: wav";
        return;
    }

    m_writer.reset(format->createWriterFor(
        m_file_stream.get(),
        44100.0, // стандартна частота дискретизації
        2,       // стерео
        16,      // бітність
        {},      // метадані
        0));     // якість для MP3, не використовується для WAV

    if (!m_writer) {
        qDebug() << "Failed to create writer for: " << m_current_output_file;
        m_file_stream.reset();
        return;
    }

    m_file_stream.release(); // передаємо власність writer'у
    emit recordingStarted();
}

void REC::stopRecording()
{
    if (m_writer) {
        m_writer.reset();
        m_current_output_file.clear();
        emit recordingStopped();
    }
}

void REC::writeAudioData(const juce::AudioBuffer<float>& buffer)
{
    if (m_writer) {
        m_writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
}
