#ifndef REC_H
#define REC_H

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

#include "RECButton.h"

class REC : public QWidget
{
    Q_OBJECT
public:
    explicit REC(QWidget *parent = nullptr, uint width = 35, uint height = 35);
    ~REC();

    void startRecording();
    void stopRecording();
    void writeAudioData(const juce::AudioBuffer<float>& buffer);

    // Новий метод для перевірки статусу запису
    bool isRecording() const { return m_writer != nullptr; }

private:
    QString m_current_output_file;
    std::unique_ptr<juce::AudioFormatWriter> m_writer;
    std::unique_ptr<juce::FileOutputStream> m_file_stream;
    juce::AudioFormatManager m_format_manager;

signals:
    void recordingStarted();
    void recordingStopped();
};

#endif // REC_H
