#pragma once
#include <QMainWindow>
#include "ControllerWidget.h"

    class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow();
private:
    QToolBar* modes;
    ControllerWidget* controllerWidget;
};
