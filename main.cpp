
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
1) виправлено підпис "audio in'
2) track_Settings --> прибрано старе поле, що вже не використовувалось
3) в Project було переіменовано деякі поля
4) ProjectSettings копка "Apply" була прибрана з полів класу (вона як і "Cancel" створюється в конструкіторі)

*/




// додай обмежувачі в сетери і QSpinBox-и










