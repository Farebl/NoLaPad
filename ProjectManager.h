#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QMainWindow>
#include "MicroTimer.h"
#include "TrackSettings.h"
#include "Metronome.h"


class IAudioEngine;
class IStorage;
class Project;
class ProjectView;

class ProjectManager : public QMainWindow
{
    Q_OBJECT

private:
    std::unique_ptr<IStorage> m_storage;
    std::unique_ptr<IAudioEngine> m_audio_engine;
    MicroTimer m_timer;
    Metronome m_metronome;
    TrackSettings m_track_settings_window;
    std::unique_ptr<Project> m_curent_project;
    QVector<ProjectView*> m_projects_views;
    ProjectView* m_current_project_view;

    bool m_dragging;
    QPoint m_drag_position;

    QWidget* m_title_bar;
    QTableWidget* m_table_widget;

    quint8 m_columns_of_views_count;


    void initProject(const QString& path_to_project);
    void deleteProject(const QString& path_to_project);

    void openProjectSettings(Project* project);

    void saveCurrentProject();
    void updateViewsTable();


    void toggleMaximize();
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void closeEvent(QCloseEvent *event) override;

public:
    ~ProjectManager();
    explicit ProjectManager(QWidget *parent = nullptr);

};


#endif // PROJECTMANAGER_H
