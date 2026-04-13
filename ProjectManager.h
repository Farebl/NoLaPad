#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "Project.h"

class ProjectManager : public QMainWindow
{
    Q_OBJECT
private:

    QTimer* m_timer_for_REC;
    TrackSettings* m_settings_window;
    Metronome* m_metronome;
    Project* m_curent_project;

    static ProjectManager* m_instance;

    explicit ProjectManager(QWidget *parent = nullptr);


public:
    static ProjectManager* getInstance(QWidget *parent = nullptr);
};


#endif // PROJECTMANAGER_H
