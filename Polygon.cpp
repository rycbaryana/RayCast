#include "Polygon.h"
#include <QDebug>

void Polygon::addVertex(const QPointF& vertex) {
    _vertices.push_back(vertex);
    updateIntersectPoints();
}

void Polygon::updateLastVertex(const QPointF& newVertex) {
    if (_vertices.empty()) {
        throw std::length_error("There is no _vertices");
    }
    _vertices.back() = newVertex;
    updateIntersectPoints();
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    double min = std::numeric_limits<double>::max();
    std::optional<QPointF> p = std::nullopt;
    for (int i = 0; i < _vertices.size(); ++i) {
        auto [t1, t2] =
            Ray::calcIntersect(ray, {_vertices[i], _vertices[(i + 1) % _vertices.size()]});
        if (t1 > 0 && t2 > 0 && t2 < 1 && t1 < min) {
            auto vec = ray.getEnd() - ray.getBegin();
            p = {ray.getBegin().x() + vec.x() * t1, ray.getBegin().y() + vec.y() * t1};
            min = t1;
        }
    }
    return p;
}

const std::vector<QPointF>& Polygon::getVertices() const {
    return _vertices;
}

void Polygon::deleteLastVertex() {
    if (!_vertices.empty()) {
        _vertices.pop_back();
        updateIntersectPoints();
    }
}

const std::vector<QPointF>& Polygon::getIntersectPoints() const {
    return _intersectPoints;
}

void Polygon::updateIntersectPoints() {
    _intersectPoints.clear();
    for (size_t i = 0; i < _vertices.size(); ++i) {
        Ray ray1 = {_vertices[i], _vertices[(i + 1) % _vertices.size()]};
        for (size_t j = 0; j < _vertices.size(); ++j) {
            if (qAbs(i - j) <= 1 || qAbs(i - j) == _vertices.size() - 1)
                continue;
            auto [t1, t2] =
                Ray::calcIntersect(ray1, {_vertices[j], _vertices[(j + 1) % _vertices.size()]});
            if (t1 > 0 && t1 < 1 && t2 > 0 && t2 < 1) {
                auto vec = ray1.getEnd() - ray1.getBegin();
                _intersectPoints.emplace_back(
                    ray1.getBegin().x() + vec.x() * t1, ray1.getBegin().y() + vec.y() * t1
                );
            }
        }
    }
}
