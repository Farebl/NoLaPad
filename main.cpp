
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


// EffectSwitcher  - станні сінглтон пав - тепр він звичайни клас
//  Виправлено збереженя проєкту --> тепер опис та шляхи збереження проєкту і записів теж зберігаються

// тепер плеєри для треків створюються через надану фабрику в проєкт

// додай обмежувачі в сетери і QSpinBox-и










