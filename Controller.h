#pragma once
#include "Ray.h"
#include "Polygon.h"

class Controller {
public:
    Controller();
    void addPolygon(const Polygon&);
    void addVertexToLastPolygon(const QPoint& newVertex);
    void updateLastPolygon(const QPoint& newVertex);
    void deleteVertexFromLastPolygon();
    void clearPolygons();
    void replacePolygon(size_t i, const Polygon& polygon);

    const std::vector<Polygon>& getPolygons() const;
    const std::vector<QPointF>& getLightSources() const;
    void setLightSources(const std::vector<QPointF>& newSources);
    const std::vector<QPointF>& getStaticSources() const;
    void setStaticSources(const std::vector<QPointF>& staticSources);
    void addStaticSources(const QPointF& newSource);
    void clearStaticSources();
    std::vector<Ray> castRays(const QPointF& source) const;
    std::vector<Ray> castRays(const QPointF& source, double angleStart, double angleEnd) const;
    void intersectRays(std::vector<Ray>& rays) const;
    std::vector<Polygon> createLightArea(std::vector<Ray>&) const;
    std::vector<Polygon> createLightArea(std::vector<Ray>&, double angleStart, double angleEnd) const;
    std::vector<Polygon> createStaticLightArea() const;
    void removeAdjacentRays(std::vector<Ray>& rays) const;

private:
    std::vector<QPointF> _lightSources;
    std::vector<QPointF> _staticSources;
    std::vector<Polygon> _polygons;
};
