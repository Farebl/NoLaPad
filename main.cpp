
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

//В ProjectSetting:
 //   помітси кожден потенціометр в вертикальний бокс щоб додати підписи
 // метож Track::setEffectType переіменовано на Track::setCurrentEffect

// EffectSwitcher отримав 5-те положення - відсутність еффекту

// додай обмежувачі в сетери і QSpinBox-и










