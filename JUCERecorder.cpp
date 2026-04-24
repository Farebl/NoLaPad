#include "JUCERecorder.h"
#include "AudioBlockInfo.h"

#include <QDir>
#include <QFileInfo>

JUCERecorder::JUCERecorder() {
    m_format_manager.registerBasicFormats();
    m_background_thread.startThread();
}

JUCERecorder::~JUCERecorder() {
    stopRecording();
    m_background_thread.stopThread(500);
}

void JUCERecorder::startRecording(const QString& path) {
    stopRecording();

    juce::File outputFile(path.toStdString());
    m_file_stream = outputFile.createOutputStream();
    if (!m_file_stream || m_file_stream->failedToOpen()) return;

    auto* format = m_format_manager.findFormatForFileExtension("wav");
    if (!format) { m_file_stream.reset(); return; }

    auto* writer = format->createWriterFor(
        m_file_stream.get(), 44100.0, 2, 16, {}, 0);
    if (!writer) { m_file_stream.reset(); return; }

    m_file_stream.release();  // власність передається ThreadedWriter
    m_threaded_writer = std::make_unique<juce::AudioFormatWriter::ThreadedWriter>(
        writer, m_background_thread, 32768);  // 32768 семплів у FIFO
    m_is_recording.store(true);
}

void JUCERecorder::stopRecording() {
    m_is_recording.store(false);
    // ThreadedWriter: завершує запис черги і закриває файл у деструкторі
    m_threaded_writer.reset();
}

void JUCERecorder::writeChunk(AudioBlockInfo* block) {
    // Викликається з аудіо-потоку — кладе у lock-free FIFO, не блокується
    if (m_threaded_writer)
        m_threaded_writer->write(
            const_cast<const float**>(block->data), block->numSamples);
}

bool JUCERecorder::isRecording() const { return m_is_recording.load(); }
