#ifndef GEOMETRYFACTORY_H
#define GEOMETRYFACTORY_H
#include <random>
#include <ogrsf_frmts.h>
#include <QPointF>
#include <vector>


#ifdef VRSA_ENABLE_TEST_UTILS

namespace vrsa
{
namespace test_utils
{

class RandomGenerator {
private:
    std::mt19937 rng;
    std::uniform_real_distribution<double> coord_dist;
    std::uniform_int_distribution<int> count_dist;

public:
    RandomGenerator(double minCoord = -180, double maxCoord = 180, int minCount = 1, int maxCount = 10)
        : rng(std::random_device{}())
        , coord_dist(minCoord, maxCoord)
        , count_dist(minCount, maxCount)
    {}

    double randomCoord() { return coord_dist(rng); }
    int randomCount() { return count_dist(rng); }

    QPointF randomPoint() {
        return QPointF(randomCoord(), randomCoord());
    }
};

// Генерация случайной точки
inline OGRPoint* createRandomPoint(RandomGenerator& gen) {
    return new OGRPoint(gen.randomCoord(), gen.randomCoord());
}

// Генерация случайной линии
inline OGRLineString* createRandomLineString(RandomGenerator& gen, int numPoints = -1) {
    if (numPoints < 0) numPoints = gen.randomCount() + 2; // минимум 2 точки

    auto* line = new OGRLineString();
    for (int i = 0; i < numPoints; ++i) {
        line->addPoint(gen.randomCoord(), gen.randomCoord());
    }
    return line;
}

// Генерация случайного полигона (простой, без отверстий)
inline OGRPolygon* createRandomPolygon(RandomGenerator& gen, int numPoints = -1) {
    if (numPoints < 0) numPoints = gen.randomCount() + 3; // минимум 3 точки

    auto* ring = new OGRLinearRing();

    // Генерируем точки
    std::vector<QPointF> points;
    for (int i = 0; i < numPoints; ++i) {
        points.push_back(gen.randomPoint());
    }

    // Вычисляем центроид
    double centerX = 0, centerY = 0;
    for (const auto& p : points) {
        centerX += p.x();
        centerY += p.y();
    }
    centerX /= numPoints;
    centerY /= numPoints;

    // Сортируем точки по углу вокруг центроида (чтобы получить простой полигон)
    std::sort(points.begin(), points.end(), [centerX, centerY](const QPointF& a, const QPointF& b) {
        return atan2(a.y() - centerY, a.x() - centerX) < atan2(b.y() - centerY, b.x() - centerX);
    });

    // Добавляем точки в кольцо
    for (const auto& p : points) {
        ring->addPoint(p.x(), p.y());
    }
    // Замыкаем полигон
    ring->addPoint(points[0].x(), points[0].y());

    auto* poly = new OGRPolygon();
    poly->addRing(ring);
    return poly;
}

// Генерация мультиточки
inline OGRMultiPoint* createRandomMultiPoint(RandomGenerator& gen, int numPoints = -1) {
    if (numPoints < 0) numPoints = gen.randomCount();

    auto* multiPoint = new OGRMultiPoint();
    for (int i = 0; i < numPoints; ++i) {
        multiPoint->addGeometry(createRandomPoint(gen));
    }
    return multiPoint;
}

// Генерация мультилинии
inline OGRMultiLineString* createRandomMultiLineString(RandomGenerator& gen, int numLines = -1) {
    if (numLines < 0) numLines = gen.randomCount();

    auto* multiLine = new OGRMultiLineString();
    for (int i = 0; i < numLines; ++i) {
        multiLine->addGeometry(createRandomLineString(gen, gen.randomCount() + 2));
    }
    return multiLine;
}

// Генерация мультиполигона
inline OGRMultiPolygon* createRandomMultiPolygon(RandomGenerator& gen, int numPolys = -1) {
    if (numPolys < 0) numPolys = gen.randomCount();

    auto* multiPoly = new OGRMultiPolygon();
    for (int i = 0; i < numPolys; ++i) {
        multiPoly->addGeometry(createRandomPolygon(gen, gen.randomCount() + 3));
    }
    return multiPoly;
}

// Генерация геометрии по типу
inline OGRGeometry* createRandomGeometry(OGRwkbGeometryType type, RandomGenerator& gen) {
    switch(wkbFlatten(type)) {
        case wkbPoint:
            return createRandomPoint(gen);
        case wkbLineString:
            return createRandomLineString(gen);
        case wkbPolygon:
            return createRandomPolygon(gen);
        case wkbMultiPoint:
            return createRandomMultiPoint(gen);
        case wkbMultiLineString:
            return createRandomMultiLineString(gen);
        case wkbMultiPolygon:
            return createRandomMultiPolygon(gen);
        default:
            return nullptr;
    }
}

}
}
#endif

#endif // GEOMETRYFACTORY_H
