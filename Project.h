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
    QString m_project_save_dir_path;
    QString m_records_save_dir_path;
    QString m_description;

    quint8 m_rows_count;
    quint8 m_columns_count;

    bool m_dragging;
    bool m_resizing;
    QPoint m_drag_position;
    QPoint m_resize_start;
    QSize m_resize_start_size;

    RecorderButton* m_recording_button;

    QWidget* m_title_bar;
    QTableWidget* m_table_widget;

    IAudioEngine* m_audio_engine;
    MicroTimer* m_timer;
    Metronome* m_metronome;
    TrackSettings* m_track_settings_window;
    QTimer* m_timer_for_REC;
    std::unique_ptr<QElapsedTimer> m_elapsed_timer_for_REC;

    QVector<Track*> m_tracks;
    Track* last_opend_track;
    ITrackPlayer*(*m_track_players_fabric)();

    explicit Project() = delete;
    explicit Project(const Project&) = delete;
    Project& operator=(const Project&) = delete;

    void updateTracksTable();
    void openTrackSettings(Track* track);
    void toggleMaximize();

	
protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

    explicit Project(
        IAudioEngine* audio_engine,
        ITrackPlayer*(*track_players_fabric)(),
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        QWidget *parent
    );

public:
    explicit Project(
        IAudioEngine* audio_engine,
        ITrackPlayer*(*track_players_fabric)(),
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        const QString& name,
        const QString& save_project_dir_path,
        const QString& save_records_dir_path,
        const QString& description,
        quint8 row,
        quint8 column,
        QWidget *parent
    );

    explicit Project(
        IAudioEngine* audio_engine,
        ITrackPlayer*(*track_players_fabric)(),
        MicroTimer* timer,
        Metronome* metronome,
        TrackSettings* track_settings_window,
        const ProjectSaveParameters& data,
        QWidget* parent
    );

    ~Project() override;
    ProjectSaveParameters getSaveParameters();


    void setName(const QString& name);
    QString getName() const;

    void setSize(const QSize& count);
    QSize getSize() const;

    void setTemp(quint16 value);
    quint16 getTemp() const;

    void setProjectSaveDirPath(const QString& path);
    QString getProjectSaveDirPath() const;

    void setRectordsSaveDirPath(const QString& path);
    QString getRectordsSaveDirPath() const;

    void setDescription(const QString& text);
    QString getDescription() const;

    RecorderButton* getRecordingButton() const;

signals:
    void closed();
    void settingsTriggered(Project* project);
    void saveTriggered();
};

#endif // PROJECT_H
