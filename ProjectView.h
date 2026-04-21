#ifndef PROJECTVIEW_H
#define PROJECTVIEW_H

#include <QPushButton>

class ProjectView : public QPushButton
{
    Q_OBJECT

private:
    QString m_project_name;
    QString m_path_to_project;
    QString m_date_of_last_use;
    QString m_description;
    QPixmap m_preview_icon;
public:
    ProjectView();
};


#endif // PROJECTVIEW_H
