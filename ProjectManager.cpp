#include "ProjectManager.h"
#include "JUCEAudioEngine.h"
#include "JUCEMetronomePlayer.h"
#include "Project.h"


ProjectManager::ProjectManager(QWidget *parent)
    : QMainWindow(parent)
    , m_audio_engine(new JUCEAudioEngine())
    , m_timer(static_cast<quint32>(60.0/(120*4)*1'000'000), nullptr)
    , m_track_settings_window(100, false, {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, Qt::gray, Qt::darkGray, this)
    , m_curent_project(nullptr)
{


    //initProject("Project 1");
    m_curent_project.reset(new Project("Project 1", "", ".", 8, 8, 60, m_audio_engine.get(), &m_timer, &m_track_settings_window, this));
    m_curent_project->show();
    connect(m_curent_project.get(), &Project::closed, this, [&](){
        QCoreApplication::quit();
    });
}


void ProjectManager::initProject(const QString& project_name){
    //m_curent_project.reset(new Project(project_name, "", ".", 8, 8, 60, m_audio_engine.get(), &m_timer, &m_track_settings_window, this));
}

ProjectManager::~ProjectManager(){
}


