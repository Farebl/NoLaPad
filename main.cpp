
#include "Project.h"
#include <QApplication>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Project w(8, 8, 60);
    w.show();
    return a.exec();
}

//Зміни потенціоиетр: заміни 4 сигнали (4 чверті) на просто зміну значеня: щоб моржна було б викоритсовувати 1 клас для всіх потенціометрів
