#include "../include/StartWin.h"
#include "../Include/Gui.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    Application app = Application();
    StartWin w = StartWin(&app, nullptr);
    Gui g = Gui(&app, nullptr);

    QObject::connect(&g, SIGNAL(back_clicked()), &w, SLOT(show()));
    QObject::connect(&w, SIGNAL(goNext()), &g, SLOT(show()));
    //QObject::connect(&w, SIGNAL(goNext()), &g, SLOT(showPreviw()));

    w.show();

    return a.exec();
}
