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
#include "RecorderButton.h"
//#include "TransportCallback.h"
#include "IAudioEngine.h"




struct ProjectSaveParameters {
    QString name;
    QString save_project_path;
    int m_row;
    int m_column;
};

class Project : public QMainWindow
{
    Q_OBJECT

private:

    explicit Project() = delete;
	explicit Project(const Project&) = delete;
	Project& operator=(const Project&) = delete;
    IAudioEngine* m_audio_engine;



/*
    juce::AudioDeviceManager m_device_manager;
    juce::MixerAudioSource m_mixer_source;
    TransportCallback* m_callback; // not RAII because performanse is important
  */

    std::vector<Track*> m_tracks;

    ProjectSaveParameters m_save_params;
    QString m_save_records_path;
    QWidget* m_central_widget;
    QWidget* m_title_bar;
    bool m_dragging;
    bool m_resizing;
    QPoint m_drag_position;
    QPoint m_resize_start;
    QSize m_resize_start_size;
    QTableWidget *m_table_widget;

    MicroTimer* m_timer;
    QTimer* m_timer_for_REC;
    std::unique_ptr<QElapsedTimer> m_elapsed_timer_for_REC; // can't having a parent (not a QObject)
    RecorderButton* m_recording_button;
    QLabel* m_digital_clock_face;
    BPMCounter* m_bpm_counter;
    Metronome* m_metronome;
    TrackSettings* m_track_settings_window;
    std::vector<QMetaObject::Connection> m_current_connections;

    void toggleMaximize();
    void openTrackSettings(Track* track);
	
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

public:
    explicit Project(const QString& name = "", const QString& save_project_path = "", const QString& save_records_path = ".", int row = 8, int column = 8, int bpm_value = 60, IAudioEngine* audio_engine = nullptr, MicroTimer* m_timer = nullptr,  TrackSettings* m_track_settings_window = nullptr,
QWidget *parent = nullptr);
    ~Project();

signals:
    void closed();
};

#endif // PROJECT_H
