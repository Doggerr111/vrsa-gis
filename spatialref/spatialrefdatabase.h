#ifndef SPATIALREFDATABASE_H
#define SPATIALREFDATABASE_H

#include <algorithm>
#include <string>
#include <QDebug>
#include <cmath>
namespace vrsa
{
namespace spatialref
{
class SpatialReference;

/**
 * @english
 * @brief Singleton database of predefined coordinate reference systems and ellipsoids
 *
 * Provides access to collection of CRS including:
 * - Global standards (WGS84, Pseudo-Mercator)
 * - Russian local systems (GSK-2011, PZ-90.11)
 *
 * Use instance() to get the singleton object.
 *
 * @russian
 * @brief База данных предопределенных систем координат (singleton) и эллипсоидов
 *
 * Предоставляет доступ к коллекции CRS, включая:
 * - Глобальные стандарты (WGS84, Pseudo-Mercator)
 * - Российские системы (ГСК-2011, ПЗ-90.11)
 *
 * Для доступа используйте instance().
 */
class SpatialReferenceDatabase {

public:
    /**
     * @english
     * @brief Coordinate Reference System information structure
     * @russian
     * @brief Структура с информацией о системе координат
     */
    struct CRSInfo
    {
        std::string name;       ///< @english Local name for UI @russian Локальное имя для интерфейса
        std::string authName;   ///< @english Authority name (e.g. "EPSG") @russian Название авторитетной организации (напр. "EPSG")
        int authCode;           ///< @english Authority code (e.g. 4326) @russian Код авторитетной организации (напр. 4326)
        std::string projString; ///< @english PROJ string representation @russian PROJ строка
        std::string wktString;  ///< @english WKT string representation @russian WKT строка
        bool isUserDefined;     ///< @english True if user-defined CRS @russian true если пользовательская система

        CRSInfo(const std::string& n, const std::string& aName, int aCode,
                    const std::string& wkt, bool userDef)
                : name(n), authName(aName), authCode(aCode), wktString(wkt),
                  isUserDefined(userDef) {}

        CRSInfo(const char* n, const char* aName, int aCode,
                    const char* proj, const char* wkt, bool userDef)
                : name(n), authName(aName), authCode(aCode),
                  projString(proj), wktString(wkt), isUserDefined(userDef) {}
    };

    struct Ellipsoid {
        std::string name;
        double a;               // большая полуось (м)
        double inv_f;           // обратное сжатие (1/f)

        Ellipsoid(const std::string& name, double semi_major_axis, double inv_flattening)
            : name{name}, a{semi_major_axis}, inv_f{inv_flattening}{}
        QString getName() const { return QString::fromStdString(name); };
        double getFlattening() const { return 1.0 / inv_f; }
        double getEccentricitySq() const {
            double f = getFlattening();
            return 2 * f - f * f;
        }
        double getEccentricity() const { return sqrt(getEccentricitySq()); }

        QString toString() const {
            return QString("%1 (a=%2, 1/f=%3)").arg(QString::fromStdString(name)).arg(a).arg(inv_f);
        }
    };

    static SpatialReferenceDatabase& instance()
    {
        static SpatialReferenceDatabase db;
        return db;
    }


    std::vector<CRSInfo> getPredefinedCRSs() const noexcept { return mCrsDB; };
    std::vector<Ellipsoid> getPredefinedEllipsoids() const noexcept { return mEllipsoids; };
    CRSInfo getCRSInfoByEPSG(int code);
    //cоздание SpatialReference из CRSInfo по индексу
    SpatialReference createFromIndex(size_t index) const;
    //cоздание SpatialReference по EPSG коду из CRSInfo
    SpatialReference createFromEPSG(int code) const;
    //cоздание SpatialReference по имени
    SpatialReference createFromName(const std::string& name) const;
    //cоздание всех CRS (возвращает вектор)
    std::vector<SpatialReference> createAll() const;

    //добавить проекции в базу
    void addCRS(const CRSInfo& info);
    void addUserCRS(CRSInfo &info); //userdefinedflag будет изменен
    void addUserCRS(const std::string& name, const std::string& authName = "", const int authCode = 0,
                    const std::string& wkt = "");

private:
    /**
    * @english
    * @brief Creates SpatialReference from CRSInfo using best available format
    *
    * Tries to create CRS in following order:
    * 1. WKT string (most complete)
    * 2. PROJ string
    * 3. EPSG code
    *
    * @param info CRS information structure
    * @return Valid SpatialReference or empty on failure
    *
    * @russian
    * @brief Создает SpatialReference из CRSInfo, используя лучший доступный формат
    *
    * Пытается создать CRS в следующем порядке:
    * 1. WKT строка (наиболее полная)
    * 2. PROJ строка
    * 3. EPSG код
    *
    * @param info Структура с информацией о CRS
    * @return Валидная SpatialReference или пустая при ошибке
    */
    SpatialReference createFromCRSInfo(const CRSInfo& info) const;

private:
    SpatialReferenceDatabase(): mCrsDB {loadCRSList()}, mEllipsoids {loadEllipsoids()}, mDBCapacity{10} {};
    SpatialReferenceDatabase(const SpatialReferenceDatabase&) = delete;
    SpatialReferenceDatabase& operator=(const SpatialReferenceDatabase&) = delete;
    std::vector<CRSInfo> loadCRSList();
    std::vector<Ellipsoid> loadEllipsoids();
//        auto list = loadCRSList();
//            mCrsDB.reserve(list.size());
//            mCrsDB = list;


private:
    std::vector<CRSInfo> mCrsDB;
    std::vector<Ellipsoid> mEllipsoids;
    size_t mDBCapacity;

};
inline QDebug operator<<(QDebug debug, const vrsa::spatialref::SpatialReferenceDatabase::CRSInfo& crsInfo)
{
    QDebugStateSaver saver(debug);
    debug.nospace() << "CRSInfo("
                    << "name: " << QString::fromStdString(crsInfo.name) << ", "
                    << "authName: " << QString::fromStdString(crsInfo.authName) << ", "
                    << "authCode: " << crsInfo.authCode << ", "
                    << "projString: " << (crsInfo.projString.empty() ? "(empty)" : QString::fromStdString(crsInfo.projString)) << ", "
                    << "wktString: " << (crsInfo.wktString.empty() ? "(empty)" : "(wkt content)") << ", "
                    << "isUserDefined: " << (crsInfo.isUserDefined ? "true" : "false")
                    << ")";
    return debug;
}
}
}

#endif // SPATIALREFDATABASE_H
