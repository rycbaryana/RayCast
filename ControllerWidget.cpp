#include "ControllerWidget.h"
#include <QActionGroup>
#include <QColorDialog>
#include <QMouseEvent>
#include <QPainter>
#include <QPushButton>
#include <QPainterPath>

ControllerWidget::ControllerWidget(QWidget* parent)
    : QWidget(parent), controller(new Controller()) {
    controller->addPolygon({});
    setMouseTracking(true);
    QPalette pal;
    pal.setColor(QPalette::Window, Qt::black);
    setAutoFillBackground(true);
    setPalette(pal);

    initModes();
}

void ControllerWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    drawPolygons(&painter);
    if (lightMode->isChecked() || flashLightMode->isChecked()) {
        drawLightArea(&painter);
        drawLightSource(&painter);
    }
    drawStaticSources(&painter);
}

void ControllerWidget::drawPolygon(QPainter* painter, const Polygon& polygon) const {
    auto vertices = polygon.getVertices();
    if (vertices.empty()) {
        return;
    }
    if (vertices.size() == 1) {
        painter->drawPoint(vertices[0]);
        return;
    }
    QPainterPath poly(vertices[0]);
    for (size_t i = 1; i < vertices.size(); ++i) {
        poly.lineTo(vertices[i]);
    }
//    painter->save();
//    for (auto& self : polygon.getIntersectPoints()) {
//        painter->setPen({Qt::red, 10});
//        painter->drawPoint(self);
//    }
//    painter->restore();
    poly.closeSubpath();
    painter->drawPath(poly);
}

void ControllerWidget::drawPolygons(QPainter* painter) const {
    painter->save();
    painter->setPen({Qt::white, 2});
    for (const auto& polygon : controller->getPolygons()) {
        drawPolygon(painter, polygon);
    }
    painter->restore();
}

void ControllerWidget::drawLightSource(QPainter* painter) const {
    painter->save();
    painter->setRenderHint(QPainter::SmoothPixmapTransform);
    auto pos = controller->getLightSources().front();
    if (lightMode->isChecked()) {
        QPixmap img(":/img/brightness.png");
        painter->drawPixmap(
            pos.x() - img.rect().width() / 2, pos.y() - img.rect().height() / 2, img.rect().width(),
            img.rect().height(), img
        );
    } else if (flashLightMode->isChecked()) {
        QPixmap img(":/img/flashlight_obj.png");
        QTransform transform;
        double rotateAngle =
            angleStart > angleEnd ? angleStart - 2 * M_PI + angleEnd : angleStart + angleEnd;
        rotateAngle *= 90 / M_PI;
        img = img.transformed(transform.rotate(rotateAngle));
        painter->translate(pos.x(), pos.y());
        painter->drawPixmap(
            -img.rect().width() / 2, -img.rect().height() / 2, img.rect().width(),
            img.rect().height(), img
        );
    }

    painter->restore();
}

void ControllerWidget::drawStaticSources(QPainter* painter) const {
    painter->save();
    for (const auto& source : controller->getStaticSources()) {
        QPixmap staticLight(":/img/static.png");
        staticLight = staticLight.scaled(16, 16);
        QPainter staticPainter(&staticLight);
        staticPainter.setCompositionMode(QPainter::CompositionMode_SourceIn);
        staticPainter.fillRect(staticLight.rect(), staticColor);
        staticPainter.end();
        painter->drawPixmap(
            source.x() - staticLight.rect().width() / 2, source.y() - staticLight.rect().height() / 2, staticLight.rect().width(),
            staticLight.rect().height(), staticLight
        );
    }
    painter->restore();
}

void ControllerWidget::drawLightArea(QPainter* painter) const {
    painter->save();
    painter->setPen(Qt::NoPen);
    std::vector<Ray> rays;
    auto lightArea =
        (flashLightMode->isChecked() ? controller->createLightArea(rays, angleStart, angleEnd)
                                     : controller->createLightArea(rays));
    for (size_t i = 0; i < lightArea.size(); ++i) {
        const Polygon& polygon = lightArea[i];
        QRadialGradient grad(controller->getLightSources().at(i), width());
        grad.setStops({{0, {230, 0, 0, 255 * 2 / maxSources}}, {0.75, Qt::transparent}});
        painter->setBrush({grad});
        drawPolygon(painter, polygon);
    }
    auto staticLightArea = controller->createStaticLightArea();
    for (size_t i = 0; i < staticLightArea.size(); ++i) {
        const Polygon& polygon = staticLightArea[i];
        QRadialGradient grad(controller->getStaticSources().at(i), 300);
        grad.setStops({{0, {staticColor.red(), staticColor.green(), staticColor.blue(), 128}}, {1, Qt::transparent}});
        QBrush brush(grad);
        painter->setBrush(brush);
        drawPolygon(painter, polygon);
    }

    if (raysMode->isChecked())
        drawRays(painter, rays);
    painter->restore();
}

void ControllerWidget::drawRays(QPainter* painter, const std::vector<Ray>& rays) const {
    painter->save();
    for (auto ray : rays) {
        painter->setPen({Qt::red, 2});
        painter->drawLine(ray.getBegin(), ray.getEnd());
        QPen pen(Qt::red, 7);
        pen.setCapStyle(Qt::RoundCap);
        painter->setPen(pen);
        //                painter->drawPoint(ray.getEnd());
        //                        painter->setPen({Qt::white, 3});
        //                        painter->drawText(ray.getEnd() + QPoint(10, 10),
        //                        QString::number(ray.getAngle() * 180 / M_PI));
    }
    painter->restore();
}

void ControllerWidget::mouseMoveEvent(QMouseEvent* event) {
    QPoint center = event->pos();
    constexpr double radius = 7.5;
    double angle = 0;
    std::vector<QPointF> newSources{center};
    while (angle <= 2 * M_PI && maxSources > 1) {
        newSources.emplace_back(center.x() + radius * cos(angle), center.y() - radius * sin(angle));
        angle += 2 * M_PI / (maxSources - 1);
    }
    controller->setLightSources(newSources);
    update();
}

void ControllerWidget::mousePressEvent(QMouseEvent* event) {
    if (!polyMode->isChecked() && !staticMode->isChecked())
        return;
    if (staticMode->isChecked()) {
        if (event->button() == Qt::LeftButton) {
            controller->addStaticSources(event->pos());
        }
        update();
        return;
    }
    if (event->button() == Qt::LeftButton) {
        if (newPolygon) {
            controller->addPolygon(Polygon({event->pos()}));
            newPolygon = false;
        } else {
            controller->addVertexToLastPolygon({event->pos()});
            controller->updateLastPolygonIntersectPoints();
        }
        update();
    }
    if (event->button() == Qt::RightButton) {
        newPolygon = true;
    }
}

void ControllerWidget::keyPressEvent(QKeyEvent* event) {
    if (polyMode->isChecked() && event->key() == Qt::Key_Z
        && event->modifiers() & Qt::ControlModifier && controller->getPolygons().size() > 1) {
        controller->deleteVertexFromLastPolygon();
        controller->updateLastPolygonIntersectPoints();
        update();
    }
    if (polyMode->isChecked() && event->key() == Qt::Key_R
        && event->modifiers() & Qt::ControlModifier) {
        controller->clearPolygons();
        controller->addPolygon(
            {{-10, -10},
             {qreal(width()) + 10, -10},
             {qreal(width()) + 10, qreal(height()) + 10},
             {-10, qreal(height()) + 10}}
        );
        newPolygon = true;
        update();
    }
    if (staticMode->isChecked() && event->key() == Qt::Key_R
        && event->modifiers() & Qt::ControlModifier) {
        controller->clearStaticSources();
        update();
    }
}

void ControllerWidget::resizeEvent(QResizeEvent* event) {
    controller->replacePolygon(
        0,
        {{-10, -10},
         {qreal(width()) + 10, -10},
         {qreal(width()) + 10, qreal(height()) + 10},
         {-10, qreal(height()) + 10}}
    );
}

void ControllerWidget::wheelEvent(QWheelEvent* event) {
    double delta = event->angleDelta().y() / 720.0;
    angleStart += delta;
    angleEnd += delta;
    if (angleStart >= 2 * M_PI)
        angleStart -= 2 * M_PI;
    if (angleStart < 0)
        angleStart += 2 * M_PI;
    if (angleEnd >= 2 * M_PI)
        angleEnd -= 2 * M_PI;
    if (angleEnd < 0)
        angleEnd += 2 * M_PI;
    update();
}

void ControllerWidget::initModes() {
    auto group = new QActionGroup(this);

    polyMode = new QAction(QIcon(":/img/polygon.png"), "Polygon", group);
    polyMode->setCheckable(true);
    polyMode->setShortcut(Qt::Key_P);
    newPolygon = true;


    lightMode = new QAction(QIcon(":/img/light.png"), "Light", group);
    lightMode->setCheckable(true);
    lightMode->setChecked(true);
    lightMode->setShortcut(Qt::Key_L);

    raysMode = new QAction(QIcon(":/img/rays.png"), "Rays", this);
    raysMode->setCheckable(true);
    raysMode->setShortcut(Qt::Key_R);

    flashLightMode = new QAction(QIcon(":/img/flashlight.png"), "Flashlight", group);
    flashLightMode->setCheckable(true);
    flashLightMode->setShortcut(Qt::Key_F);
    angleStart = 0;
    angleEnd = M_PI / 4;

    staticMode = new QAction(QIcon(":/img/static.png"), "Static Lights Mode", group);
    staticMode->setCheckable(true);
    staticMode->setShortcut(Qt::Key_S);

    colorPick = new QWidgetAction(this);
    auto* button = new QPushButton(this);
    button->setAutoFillBackground(true);
    button->setFlat(true);

    colorPick->setDefaultWidget(button);
    colorPick->defaultWidget()->setAutoFillBackground(true);
    QPalette pal;
    pal.setBrush(QPalette::Button, staticColor);
    colorPick->defaultWidget()->setPalette(pal);
    colorPick->setShortcut(Qt::Key_C);

    connect(lightMode, &QAction::changed, this, [&]() { update(); });
    connect(polyMode, &QAction::changed, this, [&]() { update(); });
    connect(raysMode, &QAction::changed, this, [&]() { update(); });
    connect(flashLightMode, &QAction::changed, this, [&]() { update(); });
    connect(staticMode, &QAction::changed, this, [&]() { update(); });
    connect(button, &QPushButton::pressed, this, [&]() {
        staticColor = QColorDialog::getColor();
        QPalette pal;
        pal.setBrush(QPalette::Button, staticColor);
        colorPick->defaultWidget()->setPalette(pal);
        setFocus();
        update();
    });
}
