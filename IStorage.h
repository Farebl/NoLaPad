#ifndef ISTORAGE_H
#define ISTORAGE_H

#include "ProjectSaveParametersAndProjectView.h"


class IStorage{
public:
    virtual ProjectSaveParameters getProjectData(const QString&, const QString&) = 0;
    virtual void saveProject(const ProjectSaveParameters&,  const QString&) = 0;
    virtual void deleteProject(const QString&, const QString&) = 0;
    virtual QVector<ProjectView> getProjectsViews(const QString&) = 0;
    virtual void saveProjectView(ProjectView*, const QString&) = 0;
    virtual void deleteProjectView(const QString&, const QString&) = 0;
};

#endif // ISTORAGE_H
