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
#include "Recorder.h"
#include "BPMCounter.h"
#include "TransportCallback.h"

class Project : public QMainWindow
{
    Q_OBJECT

private:
    static Project* m_instance;

    explicit Project() = delete;
	explicit Project(const Project&) = delete;
	Project& operator=(const Project&) = delete;
    explicit Project(int row, int column, int bpm_value, QWidget *parent);

    std::unique_ptr<Recorder> m_recorder;

    juce::AudioDeviceManager m_device_manager;
    juce::MixerAudioSource m_mixer_source;
    std::vector<Track*> m_tracks;
    TransportCallback* m_callback; // not RAII because performanse is important

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
    std::unique_ptr<MicroTimer> m_timer;

    QTimer* m_timer_for_REC;
    std::unique_ptr<QElapsedTimer> m_elapsed_timer_for_REC; // can't having a parent (not a QObject)
    RedButton* m_recording_button;
    QLabel* m_digital_clock_face;
    BPMCounter* m_bpm_counter;
    Metronome* m_metronome;
    TrackSettings* m_settings_window;
    std::vector<QMetaObject::Connection> m_current_connections;

    void toggleMaximize();
    void openTrackSettings(Track* track);
	
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
	
public:
	static Project* getInstance(int row = 8, int column = 8, int bpm_value = 60, QWidget *parent = nullptr);
    ~Project();
};

#endif // PROJECT_H
