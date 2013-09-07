#include "dlgjsrobokey.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DlgJsRoboKey w;
    w.show();
    
    return a.exec();
}
