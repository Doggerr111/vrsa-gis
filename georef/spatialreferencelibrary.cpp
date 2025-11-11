#include "spatialreferencelibrary.h"



void vrsa::georef::SpatialReferenceLibrary::AddCoordinateSystem(const std::string &name, const CoordinateSystemInfo &info)
{
    emit CoordinateSystemAdded(name);
}

std::optional<std::string> vrsa::georef::SpatialReferenceLibrary::GetWKTByName(std::string name)
{
    auto it = mCRSMap.find(name);
    if (it != mCRSMap.end())
        return it->second.wkt;
    return std::nullopt;
}

std::optional<std::string> vrsa::georef::SpatialReferenceLibrary::GetProjByName(std::string name)
{
    auto it = mCRSMap.find(name);
    if (it != mCRSMap.end())
        return it->second.proj;
    return std::nullopt;
}

std::optional<int> vrsa::georef::SpatialReferenceLibrary::GetEPSGByName(std::string name)
{
    auto it = mCRSMap.find(name);
    if (it != mCRSMap.end())
        return it->second.epsgCode;
    return std::nullopt;
}

std::vector<std::string> vrsa::georef::SpatialReferenceLibrary::GetAvaliableSystems()
{
    std::vector<std::string> names;
    names.reserve(mCRSMap.size());
    for (const auto &it: mCRSMap)
        names.push_back(it.first);
    return names;
}
