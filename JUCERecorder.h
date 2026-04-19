#ifndef JUCERECORDER_H
#define JUCERECORDER_H

#include "IRecorder.h"
#include <juce_audio_formats/juce_audio_formats.h>
#include <atomic>
#include <memory>

class JUCERecorder final : public IRecorder {
public:
    JUCERecorder();
    ~JUCERecorder() override;

    void startRecording(const QString& path) override;
    void stopRecording() override;
    void writeChunk(AudioBlockInfo* block) override;
    bool isRecording() const override;

private:
    juce::AudioFormatManager m_format_manager;
    juce::TimeSliceThread    m_background_thread{"JUCERecorder-writer"};
    std::unique_ptr<juce::AudioFormatWriter::ThreadedWriter> m_threaded_writer;
    std::unique_ptr<juce::FileOutputStream> m_file_stream;
    std::atomic<bool> m_is_recording{false};
};

#endif // JUCERECORDER_H
