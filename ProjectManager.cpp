#include "ProjectManager.h"

ProjectManager* ProjectManager::m_instance = nullptr;
ProjectManager* ProjectManager::getInstance(QWidget* parent){
    if (m_instance == nullptr)
        m_instance = new ProjectManager(parent);

    return m_instance;
}


ProjectManager::ProjectManager(QWidget *parent)
    : QMainWindow{parent}
    , m_curent_project(Project::getInstance("", "", 8, 8, 60, this))
{
    m_curent_project->show();
}
