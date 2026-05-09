
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
//У тебе падає менеджер проєктів, після ствоення проєекту і закриття йогшо



// було додано видалення проєкту
// вікно опису проєкту переїхало з вью у проєкт-менеджер
//     + виправлено проблему затініння образу проєкту

//додано сорутвання обоазів (тепер образ береже дату не як рядок а як кількість секунд)




// виправелно баг: тепр программа не крашиться після закривання проєкту або його видалення
// випарвдено баг: тепре для відкриття проєкту достатньо лише подвйного натискання

// в ProjectView додано bpm та назву проєкту

// додай обмежувачі в сетери і QSpinBox-и










