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
#include "ProjectSaveParameters.h"

class Project : public QMainWindow
{
    Q_OBJECT

private:

    QString m_name;
    QString m_save_records_path;

    quint8 m_rows_count;
    quint8 m_columns_count;

    bool m_dragging;
    bool m_resizing;
    QPoint m_drag_position;
    QPoint m_resize_start;
    QSize m_resize_start_size;


    QWidget* m_title_bar;
    QTableWidget* m_table_widget;

    IAudioEngine* m_audio_engine;
    MicroTimer* m_timer;
    Metronome* m_metronome;
    TrackSettings* m_track_settings_window;
    QTimer* m_timer_for_REC;
    std::unique_ptr<QElapsedTimer> m_elapsed_timer_for_REC;

    QVector<Track*> m_tracks;
    QVector<QMetaObject::Connection> m_track_settings_connections;

    explicit Project() = delete;
    explicit Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;

    void toggleMaximize();
    void openTrackSettings(Track* track);
	
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

    explicit Project(
        IAudioEngine* audio_engine,
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        QWidget *parent = nullptr
    );

public:
    explicit Project(
        IAudioEngine* audio_engine,
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        const QString& name = "",
        const QString& save_records_path = ".",
        quint8 row = 8,
        quint8 column = 8,
        quint16 bpm_value = 60,
        QWidget *parent = nullptr
    );

    explicit Project(
        IAudioEngine* audio_engine,
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        const ProjectSaveParameters& data,
        QWidget* parent = nullptr
    );

    ~Project();
    ProjectSaveParameters getSaveParameters();

signals:
    void closed();
    void saveTriggered();
};

#endif // PROJECT_H
