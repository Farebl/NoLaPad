#include "ProjectManager.h"
#include "JUCEAudioEngine.h"
#include "Project.h"

#include "JSONStorage.h"

ProjectManager::ProjectManager(QWidget *parent)
    : QMainWindow(parent)
    , m_storage(new JSONStorage("../../projectsViews"))
    , m_audio_engine(new JUCEAudioEngine())
    , m_timer(static_cast<quint32>(60.0/(120*4)*1'000'000), nullptr)
    , m_metronome(
        &m_timer,
        "../../music/metronome/strong_measure.wav",
        "../../music/metronome/weak_measure.wav",
        "../../music/metronome/weak_measure.wav",
        "../../music/metronome/weak_measure.wav",
        1.0,
        60,
        nullptr
    )
    , m_track_settings_window(100, false, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, Qt::gray, Qt::darkGray, this)
    , m_curent_project(nullptr)
{
    initProject("../../projects/Project1.json");
    m_curent_project->show();

    connect(m_curent_project.get(), &Project::saveTriggered, this, &ProjectManager::saveCurrentProject);
    connect(m_curent_project.get(), &Project::closed, this, [&](){
        QCoreApplication::quit();
    });
}

ProjectManager::~ProjectManager(){
}

void ProjectManager::initProject(const QString& path){
    ProjectSaveParameters data = m_storage->getProjectData(path);
    m_metronome.setBPM(data.metronome_info.bpm);
    m_metronome.setVolume(data.metronome_info.volume);
    m_curent_project.reset(new Project(m_audio_engine.get(), &m_timer, &m_metronome, &m_track_settings_window, data, this));
}


void ProjectManager::saveCurrentProject(){
    ProjectSaveParameters current_project_save_data = m_curent_project->getSaveParameters();
    m_storage->saveProject(current_project_save_data, "../../projects/Project1.json");
}


