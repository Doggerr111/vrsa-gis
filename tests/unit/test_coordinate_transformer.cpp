#include <gtest/gtest.h>
#include "spatialref/spatialrefdatabase.h"
#include "spatialref/coordinatetransformer.h"
#include "spatialref/spatialreference.h"


//TEST(CoordinateTransformer, GEOS)
//{
//    using namespace geos::geom;
//    using namespace geos::io;
//    try {
//          // Создаем фабрику геометрий
//          PrecisionModel* pm = new PrecisionModel();
//          GeometryFactory::Ptr factory = GeometryFactory::create(pm, 0);
//          std::cout << "✅ Geometry factory created" << std::endl;

//          // Создаем точку через координаты
//          Coordinate coord(10.5, 20.7);
//          auto point = factory->createPoint(coord);
//          qDebug() << "✅ Created point: " << QString::fromStdString(point->toString());

//          // Создаем линию через WKT
//          WKTReader reader(*factory);
//          std::unique_ptr<Geometry> line(reader.read("LINESTRING(0 0, 10 10, 20 5)"));
//          qDebug() << "✅ Created line from WKT: " << QString::fromStdString(line->toText());

//          // Проверяем пересечение
//          if (point->intersects(line.get())) {
//              qDebug()  << "✅ Point intersects line";
//          } else {
//              qDebug()  << "📌 Point does not intersect line";
//          }

////          // Буфер
////          std::unique_ptr<Geometry> buffer(line->buffer(2.0));
////          std::cout << "✅ Created buffer with " << buffer->getNumPoints() << " points" << std::endl;

////          // WKT вывод
////          WKTWriter writer;
////          writer.setTrim(true);
////          std::cout << "Buffer WKT: " << writer.write(buffer.get()) << std::endl;

////          // Простые операции
////          std::cout << "Point X: " << point->getX() << std::endl;
////          std::cout << "Point Y: " << point->getY() << std::endl;
////          std::cout << "Line length: " << line->getLength() << std::endl;
////          std::cout << "Buffer area: " << buffer->getArea() << std::endl;

////          // Очистка (умные указатели всё сделают сами)
////          std::cout << "🎉 C++ API test passed!" << std::endl;

//      } catch (const std::exception& e) {
//          std::cerr << "❌ Error: " << e.what() << std::endl;
//      }

//}
TEST(CoordinateTransformer, CreationTest)
{
    using namespace vrsa::spatialref;
    auto WGS84 = SpatialReference::fromEPSG(4326);
    auto GSK_2011_metres = SpatialReference::fromEPSG(7681);
    ASSERT_TRUE(WGS84.isValid());
    ASSERT_TRUE(GSK_2011_metres.isValid());

    ASSERT_TRUE(WGS84.isGeographic());
    ASSERT_FALSE(GSK_2011_metres.isGeographic());

    ASSERT_FALSE(WGS84.isProjected());
    ASSERT_TRUE(GSK_2011_metres.isGeoCentric());

    auto transformer_wgs84_to_gsk_2011 = WGS84.createTransformTo(GSK_2011_metres);
    ASSERT_NE(transformer_wgs84_to_gsk_2011, nullptr);

    double wgs_p_x = 55.7558;
    double wgs_p_y = 37.6176;
    double z = 0;
    transformer_wgs84_to_gsk_2011->transform(wgs_p_x, wgs_p_y, &z);
    qDebug()<<wgs_p_x;
    qDebug()<<wgs_p_y;
    qDebug()<<z;
}
