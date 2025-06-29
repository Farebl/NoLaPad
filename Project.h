#ifndef PROJECT_H
#define PROJECT_H

#include <QMainWindow>
#include <QWindow>
#include <QTableWidget>

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QVector>

#include "Track.h"
#include "TrackSettings.h"
#include "Metronome.h"
#include "REC.h"
#include "BPM.h"


class Project : public QMainWindow {
    Q_OBJECT

public:
    explicit Project(int row = 8, int column = 8, int bpm_value = 60, QWidget *parent = nullptr);
    ~Project();

private:
    int m_row;
    int m_column;

    QWidget* m_central_widget;
    QWidget* m_title_bar;
    bool m_dragging;
    bool m_resizing;
    QPoint m_drag_position;
    QPoint m_resize_start;
    QSize m_resize_start_size;

    QTableWidget *m_table_widget;
    MicroTimer* m_timer;

    BPM* m_bpm;
    Metronome* m_metronome;
    TrackSettings* m_settings_window;

    std::vector<QMetaObject::Connection> m_current_connections;

    juce::AudioDeviceManager m_device_manager;
    juce::MixerAudioSource m_mixer_source;
    TransportCallback m_callback;

    void toggleMaximize();
    void openTrackSettings(Track* track);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
};

#endif // PROJECT_H
