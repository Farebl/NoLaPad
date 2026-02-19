
#include "Project.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Project* project = Project::getInstance(8, 8, 60, nullptr);
    project->show();
    int result = a.exec();  // ← спочатку працює програма
    delete project;         // ← потім очищуємо
    return result;
}

//Зміни потенціоиетр: заміни 4 сигнали (4 чверті) на просто зміну значеня: щоб моржна було б викоритсовувати 1 клас для всіх потенціометрів
