#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include <QMainWindow>
#include "MicroTimer.h"
#include "TrackSettings.h"
#include "Metronome.h"

class IAudioEngine;
class IStorage;
class Project;

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

    void createCurrentProjectView();
    void saveCurrentProjectView();

public:
    ~ProjectManager();
    explicit ProjectManager(QWidget *parent = nullptr);
    void initProject(const QString& project_name);
    void saveCurrentProject();
};


#endif // PROJECTMANAGER_H
