#include <gtest/gtest.h>
#include "spatialref/spatialrefdatabase.h"
#include "spatialref/spatialreference.h"

TEST(SpatialRef, WGS84Test)
{
    using namespace vrsa::spatialref;
    auto WGS84 = SpatialReference::fromEPSG(4326);
    ASSERT_TRUE(WGS84.isValid());
    ASSERT_NE(WGS84.GetOGRSpatialRef(), nullptr);
    ASSERT_TRUE(WGS84.isGeographic());
    ASSERT_FALSE(WGS84.isProjected());
    ASSERT_EQ(WGS84.getAuthorityCode(), std::to_string(4326));
}
TEST(SpatialRef, WGS84Test_with_DB)
{
    using namespace vrsa::spatialref;
    auto WGS84 = SpatialReference::fromEPSG(4326);
    auto WGS84_info = SpatialReferenceDatabase::instance().getCRSInfoByEPSG(4326);
    auto WGS84_DB = SpatialReferenceDatabase::instance().createFromName(WGS84_info.name);
    ASSERT_TRUE(WGS84_DB.isValid());
    ASSERT_NE(WGS84_DB.GetOGRSpatialRef(), nullptr);
    ASSERT_TRUE(WGS84_DB.isGeographic());
    ASSERT_FALSE(WGS84_DB.isProjected());
    ASSERT_EQ(WGS84_DB.getAuthorityCode(), std::to_string(4326));

    char* wkt1 = nullptr;
    char* wkt2 = nullptr;
    WGS84.GetOGRSpatialRef()->exportToWkt(&wkt1);
    WGS84_DB.GetOGRSpatialRef()->exportToWkt(&wkt2);
    qDebug() << "CRS1 WKT:" << wkt1;
    qDebug() << "CRS2 WKT:" << wkt2;
    CPLFree(wkt1);
    CPLFree(wkt2);

    qDebug() << "IsSame:" << WGS84.GetOGRSpatialRef()->IsSame(WGS84_DB.GetOGRSpatialRef());
    qDebug() << "IsSameGeogCS:" << WGS84.GetOGRSpatialRef()->IsSameGeogCS(WGS84_DB.GetOGRSpatialRef());

    ASSERT_TRUE(WGS84.isSame(WGS84_DB));
}
