#pragma once
#include <QWidget>
#include "Controller.h"
#include <QWidgetAction>

class ControllerWidget : public QWidget {
    Q_OBJECT
public:
    explicit ControllerWidget(QWidget* parent = nullptr);
    void initModes();

    void paintEvent(QPaintEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

    void drawPolygons(QPainter* painter) const;
    void drawPolygon(QPainter* painter, const Polygon& polygon) const;
    void drawLightSource(QPainter* painter) const ;
    void drawRays(QPainter* painter, const std::vector<Ray>& rays) const;
    void drawLightArea(QPainter* painter) const ;
    void drawStaticSources(QPainter* painter) const;


    ~ControllerWidget() override {delete controller;};

    Controller* controller;
    QAction* polyMode;
    QAction* lightMode;
    QAction* raysMode;
    QAction* flashLightMode;
    QAction* staticMode;
    QWidgetAction* colorPick;
    QColor staticColor = Qt::red;
    bool newPolygon;
    double angleStart;
    double angleEnd;
    const int maxSources = 11;
};
