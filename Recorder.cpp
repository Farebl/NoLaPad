#include "Recorder.h"
#include <QDateTime>
#include <QDebug>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>

Recorder* Recorder::m_instance = nullptr;

Recorder* Recorder::getInstance(uint width, uint height, QWidget *parent)
{
    if (m_instance == nullptr)
        m_instance = new Recorder(width, height, parent);

    return m_instance;
}


Recorder::Recorder(uint width, uint height, QWidget *parent)
    : QWidget(parent),
    m_current_output_file("")
{
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_TranslucentBackground);
    setStyleSheet("background-color: transparent;");

    m_format_manager.registerBasicFormats();
}


Recorder::~Recorder()
{
    stopRecording();
}


void Recorder::startRecording()
{
    m_current_output_file = QCoreApplication::applicationDirPath() + "/Recorderording_" +
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
    emit RecordingStarted();
}


void Recorder::stopRecording()
{
    if (m_writer) {
        m_writer.reset();
        m_current_output_file.clear();
        emit RecordingStopped();
    }
}


void Recorder::writeAudioData(const juce::AudioBuffer<float>& buffer)
{
    if (m_writer) {
        m_writer->writeFromAudioSampleBuffer(buffer, 0, buffer.getNumSamples());
    }
}


bool Recorder::isRecording() const { return m_writer != nullptr; }
