
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
1) Деякі важливі файли було було внесено в resources.qrs
2) Було оновлено CmakeLists.txt, згідно з політикою прибрання бібліотек з диреткорії проєкту
3) Для важливіих парамтерів конструкторів класів були прибрані значення за замовчуванням
4) JSON сам створює диреткрію для збереження образів
*/




// додай обмежувачі в сетери і QSpinBox-и










