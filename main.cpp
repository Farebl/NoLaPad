
#include "ProjectManager.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ProjectManager* project_manager = new ProjectManager();
    project_manager->show();

    int result = a.exec();
    delete project_manager;
    return result;
}


/*

1) тепер за назву файлу-запису відповідає не Recorder а проєкт
2) тррек сеттінгс лишился m_track_settings_connections
*/



//!!! коли ти фарбуєш кнопку тто разом з нею фарбується і вікно попередлження про відсутність треку
//!
