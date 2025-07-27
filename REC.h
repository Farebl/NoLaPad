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

#include "RECButton.h"

class REC : public QWidget
{
    Q_OBJECT
public:
    explicit REC(QWidget *parent = nullptr, uint width = 35, uint height = 35, QString recording_format = "WAV");
    void setRecordingFormat(const QString& format);
    void startRecording();
    void stopRecording();

private:
    QProcess* m_ffmpeg_process;
    QString m_recording_format;
    QString m_current_output_file; // Зберігаємо ім'я файлу для перевірки



signals:
    void recordingStarted();
    void recordingStopped();
};

#endif // REC_H
