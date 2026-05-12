
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

// RedButton:


// TrackSettings


// ProjectSettings
    // було випарвлено помилку, при якій зміна шляху збереження зарпісів змінювала шлях збереження самого проєкту

// Project
    // було додано кнопку відкритя директорії їз записами
    // було додано назву проєкту у верхній бар


//LCDDispaly


//LCDCounter


// виправлено метроном

// track:







// додай обмежувачі в сетери і QSpinBox-и










