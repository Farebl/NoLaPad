#pragma once
#include "IStorage.h"
#include <QMap>

class MockStorage : public IStorage {
public:
    QVector<ProjectView*> stored_views;
    QMap<QString, ProjectSaveParameters> stored_projects;
    int save_project_calls = 0;
    int save_view_calls = 0;
    bool fail_next_save = false;

    QVector<ProjectView*> loadProjectsViews() override { return stored_views; }

    QString saveProject(const ProjectSaveParameters& data) override {
        ++save_project_calls;
        if (fail_next_save) { fail_next_save = false; return ""; }
        stored_projects[data.name] = data;
        return "/tmp/test/" + data.name + ".json";
    }

    bool saveProjectView(ProjectView* view) override {
        ++save_view_calls;
        if (!stored_views.contains(view)) stored_views.append(view);
        return true;
    }

    ProjectSaveParameters getProjectData(const QString&) override {
        ProjectSaveParameters p;
        p.name = "Loaded"; p.rows_count = 4; p.columns_count = 4;
        p.tracks_info.resize(16);
        for (auto& t : p.tracks_info) {
            t.volume = 0.8; t.is_loop = false; t.is_recording = false;
            t._16th_beats = {1,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0};
        }
        return p;
    }
    void deleteProject(const QString&) override {}
    void deleteProjectView(const QString&) override {}
};
