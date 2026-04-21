#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include "IStorage.h"

class JSONStorage final : public IStorage
{
private:
    QString m_project_views_save_path;
public:
    JSONStorage(const QString& project_views_save_path = "../../ProjectViews");
    ~JSONStorage() override;

    virtual ProjectSaveParameters getProjectData(const QString& path) override;
    virtual void saveProject(const ProjectSaveParameters& project_parameters,  const QString& path) override;
    virtual void deleteProject(const QString& path) override;
    virtual QVector<ProjectView> loadProjectsViews(const QString& path) override;
    virtual void saveProjectView(ProjectView* project_view) override;
    virtual void deleteProjectView(const QString& project_name) override;
};

#endif // JSONSTORAGE_H
