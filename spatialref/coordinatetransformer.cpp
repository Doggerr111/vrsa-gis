#include "coordinatetransformer.h"

vrsa::spatialref::CoordinateTransformer::CoordinateTransformer(const SpatialReference& src, const SpatialReference& dst)
    : m_src(src)
    , m_dst(dst)
    , m_transform(nullptr, OGRCoordinateTransformation::DestroyCT)
{
    auto* rawTransform = OGRCreateCoordinateTransformation(
        src.GetOGRSpatialRef(),
        dst.GetOGRSpatialRef()
    );

    if (!rawTransform) {
        VRSA_LOG_GDAL_ERROR("CRS", "Failed to create coordinate transformation from "
                            + src.getAuthorityCode() + " to " + dst.getAuthorityCode());
        return;
    }

    m_transform.reset(rawTransform);
}

vrsa::spatialref::CoordinateTransformer::~CoordinateTransformer() = default;

vrsa::spatialref::CoordinateTransformer::CoordinateTransformer(CoordinateTransformer&& other) noexcept
    : m_transform(std::move(other.m_transform))
    , m_src(std::move(other.m_src))
    , m_dst(std::move(other.m_dst))
{}

vrsa::spatialref::CoordinateTransformer& vrsa::spatialref::CoordinateTransformer::operator=(CoordinateTransformer&& other) noexcept
{
    if (this != &other) {
        m_transform = std::move(other.m_transform);
        m_src = std::move(other.m_src);
        m_dst = std::move(other.m_dst);
    }
    return *this;
}

bool vrsa::spatialref::CoordinateTransformer::transform(double& x, double& y) const
{
    if (!m_transform) return false;
    return m_transform->Transform(1, &x, &y);
}

bool vrsa::spatialref::CoordinateTransformer::transform(double& x, double& y, double* z) const
{
    if (!m_transform) return false;
    return m_transform->Transform(1, &x, &y, z);
}


OGRGeometry* vrsa::spatialref::CoordinateTransformer::transformGeometry(const OGRGeometry* geom) const
{
    if (!m_transform || !geom) return nullptr;

    // Клонируем геометрию, чтобы не менять оригинал
    OGRGeometry* clone = geom->clone();
    if (!clone) return nullptr;

    // Трансформируем клон
    if (clone->transform(m_transform.get()) != OGRERR_NONE) {
        VRSA_LOG_GDAL_ERROR("CRS", "Failed to transform geometry");
        delete clone;
        return nullptr;
    }

    return clone;
}

std::unique_ptr<vrsa::spatialref::CoordinateTransformer> vrsa::spatialref::CoordinateTransformer::createInverse() const
{
    if (!isValid()) return nullptr;
    return std::make_unique<CoordinateTransformer>(m_dst, m_src);
}

std::string vrsa::spatialref::CoordinateTransformer::getInfo() const
{
    if (!isValid()) return "Invalid transformation";
    return "Transform from " + m_src.getAuthorityCode() + " to " + m_dst.getAuthorityCode();
}

