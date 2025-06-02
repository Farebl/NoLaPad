#include "Project.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Project w("Project 1", 8, 8, 60);
    w.show();
    return a.exec();
}
