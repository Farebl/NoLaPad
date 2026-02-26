#ifndef RecorderORDER_H
#define RecorderORDER_H

#include <QWidget>
#include <QLabel>
#include <QTimer>
#include <QTime>
#include <QPushButton>
#include <QHBoxLayout>
#include <QElapsedTimer>
#include <QPainter>
#include <QColor>
#include <QProcess>

#include <juce_audio_formats/juce_audio_formats.h>
#include <juce_audio_basics/juce_audio_basics.h>

#include "RecorderButton.h"

class Recorder : public QWidget
{
    Q_OBJECT

private:
    QString m_current_output_file;
    std::unique_ptr<juce::AudioFormatWriter> m_writer;
    std::unique_ptr<juce::FileOutputStream> m_file_stream;
    juce::AudioFormatManager m_format_manager;

    static Recorder* m_instance;
    explicit Recorder(uint width, uint height, QWidget *parent);

public:
    static Recorder* getInstance(uint width = 35, uint height = 35, QWidget *parent = nullptr);
    ~Recorder();

    void startRecording();
    void stopRecording();
    void writeAudioData(const juce::AudioBuffer<float>& buffer);

    bool isRecording() const;

signals:
    void RecordingStarted();
    void RecordingStopped();
};

#endif // RecorderORDER_H
