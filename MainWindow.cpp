#include "MainWindow.h"
#include <QHBoxLayout>
#include <QToolBar>
#include <QActionGroup>

MainWindow::MainWindow() : controllerWidget(new ControllerWidget(this)), modes(new QToolBar(this))  {
    resize(800, 800);
    auto* layout = new QHBoxLayout();
    controllerWidget->setFocusPolicy(Qt::StrongFocus);
    controllerWidget->setFocus(Qt::ActiveWindowFocusReason);
    modes->setContextMenuPolicy(Qt::PreventContextMenu);
    modes->addAction(controllerWidget->lightMode);
    modes->addAction(controllerWidget->flashLightMode);
    modes->addSeparator();
    modes->addAction(controllerWidget->polyMode);
    modes->addAction(controllerWidget->staticMode);
    modes->addAction(controllerWidget->colorPick);
    modes->addSeparator();
    modes->addAction(controllerWidget->raysMode);
    modes->setToolButtonStyle(Qt::ToolButtonIconOnly);
    modes->setMovable(false);
    modes->setIconSize({32,32});
    addToolBar(modes);

    controllerWidget->setLayout(layout);
    setCentralWidget(controllerWidget);
}
