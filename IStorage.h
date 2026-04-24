#ifndef ISTORAGE_H
#define ISTORAGE_H

#include "ProjectSaveParameters.h"
class ProjectView;

class IStorage{
public:
    virtual ~IStorage() = default;
    virtual ProjectSaveParameters getProjectData(const QString& path) = 0;
    virtual void saveProject(const ProjectSaveParameters& project_save_parameters, const QString& path) = 0;
    virtual void deleteProject(const QString& path) = 0;
    virtual QVector<ProjectView*> loadProjectsViews() = 0;
    virtual void saveProjectView(ProjectView* project_view) = 0;
    virtual void deleteProjectView(const QString& project_name) = 0;
};

#endif // ISTORAGE_H
