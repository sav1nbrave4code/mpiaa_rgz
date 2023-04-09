#include <QApplication>

#include "gui/scene.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    Scene scene {};
    scene.show();
    return QApplication::exec();
}
