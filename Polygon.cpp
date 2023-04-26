#include "Polygon.h"
#include <QDebug>

void Polygon::addVertex(const QPointF& vertex) {
    _vertices.push_back(vertex);
}

void Polygon::updateLastVertex(const QPointF& newVertex) {
    if (_vertices.empty()) {
        throw std::length_error("There is no vertices");
    }
    _vertices.back() = newVertex;
}

std::optional<QPointF> Polygon::intersectRay(const Ray& ray) const {
    double min = std::numeric_limits<double>::max();
    std::optional<QPointF> p = std::nullopt;
    for (int i = 0; i < _vertices.size(); ++i) {
        auto vecSide = _vertices[(i + 1) % _vertices.size()] - _vertices[i];
        auto vecRay = ray.getEnd() - ray.getBegin();
        double rx = vecRay.x();
        double ry = vecRay.y();
        double sx = vecSide.x();
        double sy = vecSide.y();
        double x1 = ray.getBegin().x();
        double y1 = ray.getBegin().y();
        double x2 = _vertices[i].x();
        double y2 = _vertices[i].y();
        double t2 = (rx * (y2 - y1) + ry * (x1 - x2)) / (ry * sx - rx * sy);
        double t1 = (rx != 0 ? (x2 + sx * t2 - x1) / rx : (y2+sy*t2-y1) / ry);
        if (t1 > 0 && t2 > 0 && t2 < 1 && t1 < min) {
            p = {x1 + rx * t1, y1 + ry * t1};
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
    }
}
