#pragma once
#include "Ray.h"
#include <QPoint>
#include <vector>

class Polygon {
public:
    explicit Polygon(const std::vector<QPointF>& vertices) : _vertices(vertices){};
    Polygon(const std::initializer_list<QPointF>& vertices) : _vertices(vertices){};
    const std::vector<QPointF>& getVertices() const;
    const std::vector<QPointF>& getIntersectPoints() const;
    void addVertex(const QPointF& vertex);
    void updateLastVertex(const QPointF& newVertex);
    void deleteLastVertex();
    void updateIntersectPoints();
    std::optional<QPointF> intersectRay(const Ray& ray) const;

private:
    std::vector<QPointF> _vertices;
    std::vector<QPointF> _intersectPoints;
};
