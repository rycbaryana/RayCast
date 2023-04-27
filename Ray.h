#pragma once
#include <QPoint>

class Ray {
public:
    Ray(const QPointF& begin, const QPointF& end);
    Ray(const QPointF& begin, double angle);

    const QPointF& getBegin() const;
    const QPointF& getEnd() const;
    double getAngle() const;
    bool isNormalized() const;
    void setNormalized(bool normalized);
    void setBegin(const QPointF& begin);
    void setEnd(const QPointF& end);
    void setAngle(double angle);
    void angleRecalc();
    static std::pair<double, double> calcIntersect(const Ray& vec1, const Ray& vec2);

    Ray rotate(double angle) const;

    bool operator==(const Ray& other) const;

private:
    QPointF _begin;
    QPointF _end;
    double _angle;
    bool normalized;
};
