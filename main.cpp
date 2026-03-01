
#include "Project.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Project* project = Project::getInstance("", "", 8, 8, 60, nullptr);
    project->show();
    int result = a.exec();
    delete project;
    return result;
}

// зроби так щоб ПРожект задавав кількість тактів та розмір такту. Передачі це все і в мікро таймер  і в трек і в  трексеттінгс.
