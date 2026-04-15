#ifndef PROJECTSAVEPARAMETERSANDPROJECTVIEW_H
#define PROJECTSAVEPARAMETERSANDPROJECTVIEW_H

#include <qtypes.h>
#include <QString>
#include <QVector>
#include <QPixmap>

class Track;

struct ProjectSaveParameters{
    QString name;
    quint8 row;
    quint8 column;
    QString description;
    QVector<Track*> tracks;
};

class ProjectView{
    QString m_project_name;
    QString m_path_to_project;
    QString m_date_of_last_use;
    QString m_description;
    QPixmap m_preview_icon;
    // other gui_fields : ...
};

#endif // PROJECTSAVEPARAMETERSANDPROJECTVIEW_H
