
#include "ProjectManager.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ProjectManager* project_manager = ProjectManager::getInstance();

    int result = a.exec();
    delete project_manager;
    return result;
}

// зроби так щоб ПРожект задавав кількість тактів та розмір такту. Передачі це все і в мікро таймер  і в трек і в  трексеттінгс.
