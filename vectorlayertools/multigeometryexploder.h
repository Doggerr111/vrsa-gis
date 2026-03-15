#ifndef MULTIGEOMETRYEXPLODER_H
#define MULTIGEOMETRYEXPLODER_H

#include <vector/vectorlayer.h>
#include <vector>
#include <memory>
#include "gdal/geometrytypeconverter.h"

namespace vrsa
{
namespace vectorlayertools
{


//class MultiGeometryExploder {
//public:
//    // структура для хранения результата
//    struct ExplodedFeature {
//        OGRGeometry* geometry;        // простая геометрия
//        OGRFeature* originalFeature;  // ссылка на оригинальный объект (для атрибутов)
//        int partIndex;                 // индекс части в мультигеометрии
//    };

//    // Основной метод: разбивает фичу с мультигеометрией на несколько простых фич
//    static std::vector<ExplodedFeature> explodeFeature(OGRFeature* poFeature) {
//        std::vector<ExplodedFeature> result;

//        if (!poFeature) return result;

//        OGRGeometry* poGeometry = poFeature->GetGeometryRef();
//        if (!poGeometry) return result;

//        OGRwkbGeometryType geomType = wkbFlatten(poGeometry->getGeometryType());

//        // Если это уже простая геометрия - возвращаем как есть
//        if (isSimpleGeometry(geomType)) {
//            ExplodedFeature ef;
//            ef.geometry = poGeometry->clone();
//            ef.originalFeature = poFeature;
//            ef.partIndex = 0;
//            result.push_back(ef);
//            return result;
//        }

//        // Обрабатываем разные типы мультигеометрий
//        switch(geomType) {
//            case wkbMultiPoint:
//                return explodeMultiPoint((OGRMultiPoint*)poGeometry, poFeature);
//            case wkbMultiLineString:
//                return explodeMultiLineString((OGRMultiLineString*)poGeometry, poFeature);
//            case wkbMultiPolygon:
//                return explodeMultiPolygon((OGRMultiPolygon*)poGeometry, poFeature);
//            case wkbGeometryCollection:
//                return explodeGeometryCollection((OGRGeometryCollection*)poGeometry, poFeature);
//            default:
//                // Неизвестный тип - возвращаем как есть
//                ExplodedFeature ef;
//                ef.geometry = poGeometry->clone();
//                ef.originalFeature = poFeature;
//                ef.partIndex = 0;
//                result.push_back(ef);
//                return result;
//        }
//    }

//    // Метод для создания новых фич в вашем слое
//    static void addExplodedFeaturesToLayer(OGRFeature* originalFeature, vrsa::vector::VectorLayer* targetLayer,
//                                           OGRLayer* sourceLayer = nullptr)
//    {

//        auto exploded = explodeFeature(originalFeature);

//        for (size_t i = 0; i < exploded.size(); ++i) {
//            auto& ef = exploded[i];

//            // Создаем новую фичу в вашем слое
//            auto yourFeature = vector::VectorFeature::createFeature(targetLayer);

//            // Конвертируем OGR геометрию в вашу геометрию
//            //auto yourGeom = convertOGRGeometryToYourGeometry(ef.geometry);
//            yourFeature->setGeometry(gdalwrapper::OgrGeometryPtr(ef.geometry));

//            // Копируем все атрибуты из оригинальной фичи
//            copyAttributes(originalFeature, yourFeature.get(), sourceLayer);

//            // Добавляем индекс части как дополнительный атрибут (опционально)
//            yourFeature->setAttribute("_part_index", static_cast<int>(i));
//            yourFeature->setAttribute("_original_fid", static_cast<int>(originalFeature->GetFID()));

//            // Сохраняем фичу
//            targetLayer->addFeature(std::move(yourFeature));

//            // Очищаем клонированную геометрию
//            //OGRGeometryFactory::destroyGeometry(ef.geometry);
//        }
//    }


//    static std::vector<ExplodedFeature> explodeMultiPoint(OGRMultiPoint* multiPoint, OGRFeature* originalFeature)
//    {

//        std::vector<ExplodedFeature> result;

//        for (int i = 0; i < multiPoint->getNumGeometries(); ++i) {
//            OGRPoint* point = (OGRPoint*)multiPoint->getGeometryRef(i);
//            if (point) {
//                ExplodedFeature ef;
//                ef.geometry = point->clone();
//                ef.originalFeature = originalFeature;
//                ef.partIndex = i;
//                result.push_back(ef);
//            }
//        }

//        return result;
//    }

//    static std::vector<ExplodedFeature> explodeMultiLineString(OGRMultiLineString* multiLine, OGRFeature* originalFeature)
//    {
//        std::vector<ExplodedFeature> result;

//        for (int i = 0; i < multiLine->getNumGeometries(); ++i) {
//            OGRLineString* line = (OGRLineString*)multiLine->getGeometryRef(i);
//            if (line) {
//                ExplodedFeature ef;
//                ef.geometry = line->clone();
//                ef.originalFeature = originalFeature;
//                ef.partIndex = i;
//                result.push_back(ef);
//            }
//        }

//        return result;
//    }

//    static std::vector<ExplodedFeature> explodeMultiPolygon(OGRMultiPolygon* multiPoly, OGRFeature* originalFeature)
//    {

//        std::vector<ExplodedFeature> result;

//        for (int i = 0; i < multiPoly->getNumGeometries(); ++i) {
//            OGRPolygon* poly = (OGRPolygon*)multiPoly->getGeometryRef(i);
//            if (poly) {
//                ExplodedFeature ef;
//                ef.geometry = poly->clone();
//                ef.originalFeature = originalFeature;
//                ef.partIndex = i;
//                result.push_back(ef);
//            }
//        }

//        return result;
//    }

//    static std::vector<ExplodedFeature> explodeGeometryCollection( OGRGeometryCollection* collection, OGRFeature* originalFeature)
//    {

//        std::vector<ExplodedFeature> result;

//        for (int i = 0; i < collection->getNumGeometries(); ++i) {
//            OGRGeometry* geom = collection->getGeometryRef(i);
//            if (geom) {
//                // Рекурсивно разбиваем каждый элемент коллекции
//                OGRFeature tempFeature(originalFeature->GetDefnRef());
//                tempFeature.SetGeometry(geom);

//                auto subResult = explodeFeature(&tempFeature);
//                for (auto& ef : subResult) {
//                    ef.originalFeature = originalFeature; // восстанавливаем оригинал
//                    result.push_back(ef);
//                }
//            }
//        }

//        return result;
//    }

//    // Вспомогательная функция для копирования атрибутов
//    static void copyAttributes(OGRFeature* srcFeature, vrsa::vector::VectorFeature* dstFeature, OGRLayer* layer = nullptr)
//    {

//        OGRFeatureDefn* poDefn = srcFeature->GetDefnRef();

//        for (int i = 0; i < poDefn->GetFieldCount(); ++i) {
//            OGRFieldDefn* poFieldDefn = poDefn->GetFieldDefn(i);
//            const char* fieldName = poFieldDefn->GetNameRef();

//            if (!srcFeature->IsFieldSet(i)) continue;

//            switch(poFieldDefn->GetType())
//            {
//                case OFTInteger:
//                    dstFeature->setAttribute(fieldName, srcFeature->GetFieldAsInteger(i));
//                    break;
//                case OFTInteger64: //ADD IT TO VARIANT FFS
//                    dstFeature->setAttribute(fieldName, static_cast<int>(srcFeature->GetFieldAsInteger64(i)));
//                    break;
//                case OFTReal:
//                    dstFeature->setAttribute(fieldName, srcFeature->GetFieldAsDouble(i));
//                    break;
//                case OFTString:
//                    dstFeature->setAttribute(fieldName, srcFeature->GetFieldAsString(i));
//                    break;
//                //ADD OTHERS
//                default:
//                    // Для неподдерживаемых типов сохраняем как строку
//                    dstFeature->setAttribute(fieldName, srcFeature->GetFieldAsString(i));
//                    break;
//            }
//        }
//    }

//    // Заглушка для конвертации OGR геометрии в вашу геометрию
//    // Реализуйте согласно вашей ГИС
//    static std::shared_ptr<vrsa::geometry::Geometry> convertOGRGeometryToYourGeometry(
//        OGRGeometry* ogrGeom) {
//        // Ваша реализация конвертации
//        // ...
//        return nullptr;
//    }



//    // 1. Создает новый GDALDataset из вашего слоя с разбитыми геометриями
//       static GDALDataset* createDatasetFromYourLayer(
//           vrsa::vector::VectorLayer* yourLayer,
//           const std::string& format = "ESRI Shapefile",
//           const std::string& outputPath = "")
//       {
//           GDALAllRegister();

//           // Получаем драйвер
//           GDALDriver* poDriver = GetGDALDriverManager()->GetDriverByName(format.c_str());
//           if (!poDriver) {
//               VRSA_DEBUG("ERROR", "Driver not found: " + format);
//               return nullptr;
//           }

//           // Создаем датасет
//           GDALDataset* poDS;
//           if (outputPath.empty()) {
//               poDS = poDriver->Create("/vsimem/exploded_layer", 0, 0, 0, GDT_Unknown, NULL);
//           } else {
//               poDS = poDriver->Create(outputPath.c_str(), 0, 0, 0, GDT_Unknown, NULL);
//           }

//           if (!poDS) {
//               VRSA_DEBUG("ERROR", "Failed to create dataset");
//               return nullptr;
//           }

//           // Определяем тип геометрии (простой, даже если исходный был мульти)
//           OGRwkbGeometryType targetGeomType = gdalwrapper::GeometryTypeConverter::getSimpleGeometryType(
//               gdalwrapper::GeometryTypeConverter::ToOGR(yourLayer->getGeomType())
//           );

//           // Создаем слой
//           OGRLayer* poLayer = poDS->CreateLayer(
//               yourLayer->getName(),
//               yourLayer->getOGRLayer()->GetSpatialRef(),  // УБРАТЬ!!!!пространственную привязку можно добавить позже
//               targetGeomType,
//               NULL
//           );

//           if (!poLayer)
//           {
//               VRSA_DEBUG("ERROR", "Failed to create layer");
//               GDALClose(poDS);
//               return nullptr;
//           }

//           // Создаем поля из атрибутов вашего слоя
//           createFieldsFromYourLayer(poLayer, yourLayer);

//           // Добавляем служебные поля
//           addHelperFields(poLayer);

//           // Заполняем данными с разбиением мультигеометрий
//           int explodedCount = 0;
//           int originalFid = 0;

//           for (int i = 0; i < yourLayer->getFeaturesCount(); ++i)
//           {
//               auto* yourFeature = yourLayer->getFeatureAt(i);
//               if (!yourFeature) continue;

//               auto ogrGeom = yourFeature->getOGRGeometry();
//               if (!ogrGeom) continue;

//               // Разбиваем мультигеометрию
//               auto explodedGeoms = explodeOGRGeometry(ogrGeom);

//               // Для каждой части создаем фичу
//               for (size_t partIdx = 0; partIdx < explodedGeoms.size(); partIdx++) {
//                   OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

//                   poFeature->SetGeometry(explodedGeoms[partIdx]);

//                   // Копируем атрибуты из вашей фичи
//                   copyAttributesFromYourFeature(yourFeature, poFeature, yourLayer);

//                   poFeature->SetField("_original_fid", originalFid);
//                   poFeature->SetField("_part_index", (int)partIdx);

//                   if (poLayer->CreateFeature(poFeature) == OGRERR_NONE) {
//                       explodedCount++;
//                   }

//                   OGRFeature::DestroyFeature(poFeature);
//               }

//               // Очищаем
//               OGRGeometryFactory::destroyGeometry(ogrGeom);
//               originalFid++;
//           }

//           VRSA_DEBUG("INFO", "Created dataset with " + std::to_string(explodedCount) +
//                     " features from " + std::to_string(originalFid) + " original features");

//           return poDS;
//       }

//       // 2. Модифицирует существующий GDALDataset, добавляя слой с разбитыми геометриями из вашего слоя
////       static bool addYourLayerToDataset(
////           GDALDataset* poDS,
////           vrsa::vector::VectorLayer* yourLayer,
////           const std::string& newLayerName = "")
////       {
////           if (!poDS || !yourLayer) return false;

////           std::string outputLayerName = newLayerName.empty() ?
////                                         yourLayer->name() + "_exploded" : newLayerName;

////           OGRwkbGeometryType targetGeomType = getSimpleGeometryType(
////               convertToOGRGeometryType(yourLayer->geometryType())
////           );

////           // Создаем новый слой в существующем датасете
////           OGRLayer* poLayer = poDS->CreateLayer(
////               outputLayerName.c_str(),
////               nullptr,
////               targetGeomType,
////               NULL
////           );

////           if (!poLayer) {
////               VRSA_DEBUG("ERROR", "Failed to create layer in dataset");
////               return false;
////           }

////           // Создаем поля
////           createFieldsFromYourLayer(poLayer, yourLayer);
////           addHelperFields(poLayer);

////           // Заполняем данными
////           int explodedCount = 0;
////           int originalFid = 0;

////           for (int i = 0; i < yourLayer->featureCount(); ++i) {
////               auto* yourFeature = yourLayer->featureAt(i);
////               if (!yourFeature) continue;

////               auto yourGeom = yourFeature->geometry();
////               if (!yourGeom) continue;

////               OGRGeometry* ogrGeom = convertYourGeometryToOGR(yourGeom);
////               if (!ogrGeom) continue;

////               auto explodedGeoms = explodeOGRGeometry(ogrGeom);

////               for (size_t partIdx = 0; partIdx < explodedGeoms.size(); partIdx++) {
////                   OGRFeature* poFeature = OGRFeature::CreateFeature(poLayer->GetLayerDefn());

////                   poFeature->SetGeometry(explodedGeoms[partIdx]);
////                   copyAttributesFromYourFeature(yourFeature, poFeature, yourLayer);

////                   poFeature->SetField("_original_fid", originalFid);
////                   poFeature->SetField("_part_index", (int)partIdx);

////                   if (poLayer->CreateFeature(poFeature) == OGRERR_NONE) {
////                       explodedCount++;
////                   }

////                   OGRFeature::DestroyFeature(poFeature);
////               }

////               OGRGeometryFactory::destroyGeometry(ogrGeom);
////               originalFid++;
////           }

////           VRSA_DEBUG("INFO", "Added layer '" + outputLayerName + "' with " +
////                     std::to_string(explodedCount) + " features");

////           return true;
////       }

////       // 3. Обновляет существующий слой в датасете, разбивая его геометрии
////       static bool explodeLayerInDataset(
////           GDALDataset* poDS,
////           const std::string& layerName,
////           const std::string& newLayerName = "")
////       {
////           if (!poDS) return false;

////           OGRLayer* poSrcLayer = poDS->GetLayerByName(layerName.c_str());
////           if (!poSrcLayer) {
////               VRSA_DEBUG("ERROR", "Layer not found: " + layerName);
////               return false;
////           }

////           std::string outputLayerName = newLayerName.empty() ?
////                                         layerName + "_exploded" : newLayerName;

////           OGRwkbGeometryType srcGeomType = poSrcLayer->GetGeomType();
////           OGRwkbGeometryType targetGeomType = getSimpleGeometryType(srcGeomType);

////           OGRLayer* poDstLayer = poDS->CreateLayer(
////               outputLayerName.c_str(),
////               poSrcLayer->GetSpatialRef(),
////               targetGeomType,
////               NULL
////           );

////           if (!poDstLayer) {
////               VRSA_DEBUG("ERROR", "Failed to create destination layer");
////               return false;
////           }

////           // Копируем схему полей
////           OGRFeatureDefn* poSrcDefn = poSrcLayer->GetLayerDefn();
////           for (int i = 0; i < poSrcDefn->GetFieldCount(); ++i) {
////               OGRFieldDefn* poFieldDefn = poSrcDefn->GetFieldDefn(i);
////               poDstLayer->CreateField(poFieldDefn);
////           }

////           addHelperFields(poDstLayer);

////           // Читаем и разбиваем фичи
////           poSrcLayer->ResetReading();
////           OGRFeature* poSrcFeature;

////           int originalFid = 0;
////           int explodedCount = 0;

////           while ((poSrcFeature = poSrcLayer->GetNextFeature()) != NULL) {
////               auto exploded = explodeFeature(poSrcFeature);

////               for (auto& ef : exploded) {
////                   OGRFeature* poDstFeature = OGRFeature::CreateFeature(poDstLayer->GetLayerDefn());

////                   poDstFeature->SetFrom(poSrcFeature, TRUE);
////                   poDstFeature->SetGeometry(ef.geometry);

////                   poDstFeature->SetField("_original_fid", originalFid);
////                   poDstFeature->SetField("_part_index", ef.partIndex);

////                   if (poDstLayer->CreateFeature(poDstFeature) == OGRERR_NONE) {
////                       explodedCount++;
////                   }

////                   OGRFeature::DestroyFeature(poDstFeature);
////                   OGRGeometryFactory::destroyGeometry(ef.geometry);
////               }

////               OGRFeature::DestroyFeature(poSrcFeature);
////               originalFid++;
////           }

////           VRSA_DEBUG("INFO", "Exploded " + std::to_string(originalFid) +
////                     " features into " + std::to_string(explodedCount) + " features");

////           return true;
////       }




//       // НОВЫЕ вспомогательные методы

//           static std::vector<OGRGeometry*> explodeOGRGeometry(OGRGeometry* geom) {
//               std::vector<OGRGeometry*> result;
//               if (!geom) return result;

//               OGRwkbGeometryType type = wkbFlatten(geom->getGeometryType());

//               if (type == wkbMultiPoint || type == wkbMultiLineString ||
//                   type == wkbMultiPolygon || type == wkbGeometryCollection) {

//                   auto* collection = (OGRGeometryCollection*)geom;
//                   for (int i = 0; i < collection->getNumGeometries(); ++i) {
//                       OGRGeometry* subGeom = collection->getGeometryRef(i);
//                       if (subGeom) {
//                           result.push_back(subGeom->clone());
//                       }
//                   }
//               } else {
//                   result.push_back(geom->clone());
//               }

//               return result;
//           }

//           static void createFieldsFromYourLayer(
//               OGRLayer* poLayer,
//               vrsa::vector::VectorLayer* yourLayer)
//           {
//               auto fieldNames = yourLayer->getAttributesNames();

//               for (const auto& fieldName : fieldNames)
//               {

//                   auto fieldType = yourLayer->fieldType(fieldName);

//                   OGRFieldDefn ogrField(fieldName.c_str(), convertToOGRFieldType(fieldType));
//                   poLayer->CreateField(&ogrField);
//               }
//           }

//           static OGRFieldType convertToOGRFieldType(vrsa::common::FieldType type) {
//               switch(type) {
//                   case vrsa::common::FieldType::Integer: return OFTInteger;
//                   case vrsa::common::FieldType::Integer64: return OFTInteger64;
//                   case vrsa::common::FieldType::Real: return OFTReal;
//                   case vrsa::common::FieldType::String: return OFTString;
//                   case vrsa::common::FieldType::Date: return OFTDate;
//                   case vrsa::common::FieldType::DateTime: return OFTDateTime;
//                   default: return OFTString;
//               }
//           }

//           static void addHelperFields(OGRLayer* poLayer) {
//               OGRFieldDefn originalFidField("_original_fid", OFTInteger);
//               poLayer->CreateField(&originalFidField);

//               OGRFieldDefn partIndexField("_part_index", OFTInteger);
//               poLayer->CreateField(&partIndexField);
//           }

//           static void copyAttributesFromYourFeature(
//               vrsa::vector::Feature* srcFeature,
//               OGRFeature* dstFeature,
//               vrsa::vector::VectorLayer* layer)
//           {
//               auto fieldNames = layer->fieldNames();

//               for (const auto& fieldName : fieldNames) {
//                   auto fieldType = layer->fieldType(fieldName);
//                   auto value = srcFeature->attribute(fieldName);

//                   int fieldIndex = dstFeature->GetFieldDefnRef()->GetFieldIndex(fieldName.c_str());
//                   if (fieldIndex < 0) continue;

//                   switch(fieldType) {
//                       case vrsa::common::FieldType::Integer:
//                           dstFeature->SetField(fieldIndex, value.toInt());
//                           break;
//                       case vrsa::common::FieldType::Real:
//                           dstFeature->SetField(fieldIndex, value.toDouble());
//                           break;
//                       case vrsa::common::FieldType::String:
//                           dstFeature->SetField(fieldIndex, value.toString().toStdString().c_str());
//                           break;
//                       default:
//                           dstFeature->SetField(fieldIndex, value.toString().toStdString().c_str());
//                           break;
//                   }
//               }
//           }
//};

}
}

#endif // MULTIGEOMETRYEXPLODER_H
