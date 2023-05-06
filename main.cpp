#include "MainWindow.h"
#include <QApplication>
#include <QPushButton>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);
    MainWindow w;
    w.setWindowTitle("Raycast 2D");
    w.show();
    return QApplication::exec();
}
