
#include "ProjectManager.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    ProjectManager* project_manager = new ProjectManager();

    int result = a.exec();
    delete project_manager;
    return result;
}


