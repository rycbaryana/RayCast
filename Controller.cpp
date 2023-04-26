#include "Controller.h"
#include <QDebug>

const std::vector<QPointF>& Controller::getLightSources() const {
    return _lightSources;
}

void Controller::setLightSources(const std::vector<QPointF>& newSources) {
    _lightSources = newSources;
}

const std::vector<Polygon>& Controller::getPolygons() const {
    return _polygons;
}

Controller::Controller() : _polygons(), _lightSources({{0, 0}}), _staticSources() { }

void Controller::addPolygon(const Polygon& polygon) {
    _polygons.push_back(polygon);
}

void Controller::addVertexToLastPolygon(const QPoint& newVertex) {
    if (_polygons.empty()) {
        throw std::length_error("Polygons are empty");
    }
    _polygons.back().addVertex(newVertex);
}

void Controller::updateLastPolygon(const QPoint& newVertex) {
    if (_polygons.empty()) {
        throw std::length_error("Polygons are empty");
    }
    _polygons.back().updateLastVertex(newVertex);
}

std::vector<Ray> Controller::castRays(const QPointF& source) const {
    std::vector<Ray> rays;
    for (const auto& polygon : _polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            Ray ray = {source, vertex};
            double delta = 0.00001;
            rays.push_back(ray);
            rays.push_back(ray.rotate(delta));
            rays.push_back(ray.rotate(-delta));
        }
    }

    return rays;
}

void Controller::intersectRays(std::vector<Ray>& rays) const {
    for (const auto& polygon : _polygons) {
        for (auto& ray : rays) {
            if (auto p = polygon.intersectRay(ray)) {
                auto v1 = ray.getEnd() - ray.getBegin();
                auto v2 = *p - ray.getBegin();
                if (QPointF::dotProduct(v1, v1) > QPointF::dotProduct(v2, v2)
                    || ray.isNormalized()) {
                    ray.setNormalized(false);
                    ray.setEnd(*p);
                }
            };
        }
    }
}

void Controller::deleteVertexFromLastPolygon() {
    if (!_polygons.empty()) {
        _polygons.back().deleteLastVertex();
    }
}

std::vector<Polygon> Controller::createLightArea(std::vector<Ray>& raysOut) const {
    std::vector<Polygon> light;
    for (const auto& source : _lightSources) {
        auto rays = castRays(source);
        intersectRays(rays);
        std::sort(rays.begin(), rays.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.getAngle() < rhs.getAngle();
        });
        removeAdjacentRays(rays);
        Polygon lightArea({});
        for (const auto& ray : rays) {
            lightArea.addVertex(ray.getEnd());
            raysOut.push_back(ray);
        }
        light.push_back(lightArea);
    }
    return light;
}

std::vector<Ray>
Controller::castRays(const QPointF& source, double angleStart, double angleEnd) const {
    std::vector<Ray> rays;
    Ray ox = {source, 0};
    rays.push_back(ox.rotate(angleStart));
    rays.push_back(ox.rotate(angleEnd));
    for (const auto& polygon : _polygons) {
        for (const auto& vertex : polygon.getVertices()) {
            Ray ray = {source, vertex};
            double delta = 0.00001;
            if ((ray.getAngle() >= angleStart && ray.getAngle() <= angleEnd)
                || (angleStart >= angleEnd
                    && (ray.getAngle() >= angleStart || ray.getAngle() <= angleEnd))) {
                rays.push_back(ray);
                rays.push_back(ray.rotate(delta));
                rays.push_back(ray.rotate(-delta));
            }
        }
    }
    return rays;
}

std::vector<Polygon>
Controller::createLightArea(std::vector<Ray>& raysOut, double angleStart, double angleEnd) const {
    std::vector<Polygon> light;
    for (const auto& source : _lightSources) {
        auto rays = castRays(source, angleStart, angleEnd);
        intersectRays(rays);
        if (angleStart < angleEnd) {
            std::sort(rays.begin(), rays.end(), [](const auto& lhs, const auto& rhs) {
                return lhs.getAngle() < rhs.getAngle();
            });
        } else {
            std::sort(rays.begin(), rays.end(), [=](const auto& lhs, const auto& rhs) {
                double langle = lhs.getAngle();
                double rangle = rhs.getAngle();

                if (langle >= angleStart || qFuzzyCompare(langle, angleStart))
                    langle -= 2 * M_PI;
                if (rangle >= angleStart || qFuzzyCompare(rangle, angleStart))
                    rangle -= 2 * M_PI;
                return langle < rangle;
            }   );
        }
        Polygon lightArea({source});
        for (const auto& ray : rays) {
            lightArea.addVertex(ray.getEnd());
            raysOut.push_back(ray);
        }
        light.push_back(lightArea);
    }
    return light;
}

std::vector<Polygon> Controller::createStaticLightArea() const {
    std::vector<Polygon> light;
    for (const auto& source : _staticSources) {
        auto rays = castRays(source);
        intersectRays(rays);
        std::sort(rays.begin(), rays.end(), [](const auto& lhs, const auto& rhs) {
            return lhs.getAngle() < rhs.getAngle();
        });
        Polygon lightArea({});
        for (const auto& ray : rays) {
            lightArea.addVertex(ray.getEnd());
        }
        light.push_back(lightArea);
    }
    return light;
}

void Controller::removeAdjacentRays(std::vector<Ray>& rays) const {
    double eps = 1;
    bool done = false;
    while (!done) {
        for (int i = 0; i < rays.size() - 1; ++i) {
            done = true;
            auto vec = rays[i].getEnd() - rays[i + 1].getEnd();
            if (sqrt(QPointF::dotProduct(vec, vec)) <= eps) {
                rays.erase(rays.begin() + i + 1);
                done = false;
            }
        }
    }
}

void Controller::clearPolygons() {
    _polygons.clear();
}

void Controller::replacePolygon(size_t i, const Polygon& polygon) {
    _polygons[i] = polygon;
}

const std::vector<QPointF>& Controller::getStaticSources() const {
    return _staticSources;
}

void Controller::setStaticSources(const std::vector<QPointF>& staticSources) {
    _staticSources = staticSources;
}

void Controller::addStaticSources(const QPointF& newSource) {
    _staticSources.push_back(newSource);
}

void Controller::clearStaticSources() {
    _staticSources.clear();
}
