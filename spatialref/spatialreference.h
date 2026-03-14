#ifndef SPATIALREFERENCE_H
#define SPATIALREFERENCE_H

#include "gdal/gdalresourcehandles.h"
namespace vrsa
{
namespace common
{
enum class CRSDesctiptionFormat: int;
}
namespace spatialref
{

class SpatialReference
{
public:
    SpatialReference() = default;
    SpatialReference(gdalwrapper::OgrSpatialRefPtr ogrSpatialRef);
    SpatialReference(OGRSpatialReference* ogrSpatialRef);
    SpatialReference(std::string str, common::CRSDesctiptionFormat format);
    SpatialReference(int epsgCode);
    SpatialReference(const SpatialReference& other);
    SpatialReference& operator=(const SpatialReference& other);
    SpatialReference(SpatialReference&& other) noexcept = default;
    SpatialReference& operator=(SpatialReference&& other) noexcept = default;


    static SpatialReference fromWKT(const std::string& wkt);
    static SpatialReference fromEPSG(int epsgCode);
    static SpatialReference fromProj(const std::string& projString);
    static SpatialReference fromUTM(int zone, bool northernHemisphere = true);

    inline OGRSpatialReference* GetOGRSpatialRef() const noexcept { return mCrs.get(); }

    //методы для получения информации
    bool isValid() const;
    inline bool isGeographic() const { return mCrs ? mCrs->IsGeographic() : false; };  // градусы
    inline bool isProjected() const { return mCrs ? mCrs->IsProjected() : false; };   // метры
    double getLinearUnits(std::string& unitName) const; // метры/футы и т.д.
    std::string getAuthorityCode() const; // например "4326"
    std::string getAuthorityName() const; // например "EPSG"

    //экспорт
    std::string toWKT() const;
    std::string toProj() const;
    std::string toPrettyWKT() const; // с отступами для отладки
    /**
     * @english @brief Returns EPSG code of spatial reference
     * @return EPSG code or -1 if not available/error
     * @russian @brief Возвращает EPSG код системы координат
     * @return EPSG код или -1 если недоступен/ошибка
     */
    int toEPSG() const;

    //    //работа с пользовательскими CRS
    //    bool saveUserCRS(const std::string& name); // сохранить в пользовательскую БД
    //    static SpatialReference fromUserCRS(const std::string& name); // загрузить

    //вспомогательные методы
    bool isSame(const SpatialReference& other) const;
    /**
     * @english @brief Creates transformation from this CRS to target CRS
     * @param target Target spatial reference
     * @return Transformation smart pointer or nullptr on error
     *
     * @russian @brief Создает трансформацию из этой CRS в целевую CRS
     * @param target Целевая система координат
     * @return Умный указатель на трансформацию или nullptr при ошибке
     */
    gdalwrapper::OgrCoordinateTransformationRefPtr createTransformTo(const SpatialReference& target) const;


private:
    vrsa::gdalwrapper::OgrSpatialRefPtr mCrs;

};

}
}


#endif // SPATIALREFERENCE_H
