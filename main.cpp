
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

// з JSONStorage приюбрано зайві методи конвератції

// додай до audioEngine метод який буде примусовов встановлювати прапор знищення, чи неактивності при яскому AdioCallBack...не буде оброблятись

// коли ти створив новий проєект - ти ставиш мод вікна налаштувань на зикриття по кнопці Apply --> знім цей режим після відкрииття вікна.

// додай обмежувачі в сетери і QSpinBox-и

// додано в projectsettings кнорпки apply та cancel (щоб зміни вносилися 1 раз після підтвредження)

//  виправлено UB із сигналами в вікнах налаштувань



// замінено в лямбдах [&] на this, де треюба тільки this;

// усуненно UB --> Project в деструкторі видаляв треки, хоча їхнім лайвтаймом керував m_table_widget

// поток таймера перенесено з Project в ProjectManager

// додавання метроному в аудіорушій перенесено з Project в ProjectManager






