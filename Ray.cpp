#include "Ray.h"

Ray Ray::rotate(double angle) const {
    return Ray(_begin, angle + _angle);
}

Ray::Ray(const QPointF& begin, const QPointF& end) : _begin(begin), _end(end), normalized(false) {
    angleRecalc();
}

const QPointF& Ray::getBegin() const {
    return _begin;
}

const QPointF& Ray::getEnd() const {
    return _end;
}

double Ray::getAngle() const {
    return _angle;
}

void Ray::setBegin(const QPointF& begin) {
    _begin = begin;
    angleRecalc();
}

void Ray::setEnd(const QPointF& end) {
    _end = end;
    angleRecalc();
}

void Ray::setAngle(double angle) {
    _angle = angle;
}

void Ray::angleRecalc() {
    auto vec = _end - _begin;
    auto length = sqrt(vec.x() * vec.x() + vec.y() * vec.y());
    if (length == 0) {
        _angle = 0;
        return;
    }
    _angle = acos(vec.x() / length);
    if (vec.x() <= 0 && vec.y() <= 0) _angle = 2 * M_PI - _angle;
    if (vec.x() > 0 && vec.y() < 0) _angle = 2 * M_PI - _angle;
    if (_angle >= 2 * M_PI) _angle -= 2 * M_PI;
}

Ray::Ray(const QPointF& begin, double angle) : _begin(begin), _angle(angle), normalized(true) {
    _end = _begin + QPointF(cos(angle), sin(angle));
}

bool Ray::isNormalized() const {
    return normalized;
}

void Ray::setNormalized(bool normalized) {
    this->normalized = normalized;
}

std::pair<double, double> Ray::calcIntersect(const Ray& ray1, const Ray& ray2) {
    auto vec1 = ray1.getEnd() - ray1.getBegin();
    auto vec2 = ray2.getEnd() - ray2.getBegin();
    double rx = vec1.x();
    double ry = vec1.y();
    double sx = vec2.x();
    double sy = vec2.y();
    double x1 = ray1.getBegin().x();
    double y1 = ray1.getBegin().y();
    double x2 = ray2.getBegin().x();
    double y2 = ray2.getBegin().y();
    double t2 = (rx * (y2 - y1) + ry * (x1 - x2)) / (ry * sx - rx * sy);
    double t1 = (rx != 0 ? (x2 + sx * t2 - x1) / rx : (y2 + sy * t2 - y1) / ry);
    return {t1, t2};
}

bool Ray::operator==(const Ray& other) const {
    return _begin == other._begin && _angle == other._angle;
}
