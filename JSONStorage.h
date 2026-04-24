#ifndef JSONSTORAGE_H
#define JSONSTORAGE_H

#include "IStorage.h"

class JSONStorage final : public IStorage
{
private:
    QString m_projects_views_save_dir_path;
    QString m_projects_views_file_name;
    QString m_project_view_preview_icon_save_format;

    QString getProjectsViewsFilePath() const;
    QString getPreviewIconPath(const QString& project_name) const;

public:
    JSONStorage(const QString& projects_views_save_dir_path, const QString& projects_views_file_name, const QString& project_view_preview_icon_save_format);
    ~JSONStorage() override;

    virtual ProjectSaveParameters getProjectData(const QString& path) override;
    virtual void saveProject(const ProjectSaveParameters& project_save_parameters, const QString& path) override;
    virtual void deleteProject(const QString& path) override;
    virtual QVector<ProjectView*> loadProjectsViews() override;
    virtual void saveProjectView(ProjectView* project_view) override;
    virtual void deleteProjectView(const QString& project_name) override;
};

#endif // JSONSTORAGE_H
