#ifndef GISTYPES_H
#define GISTYPES_H

#include <QPointF>
#include <QRectF>
#include <QSizeF>
#include <QColor>
#include <string>
#include <memory>
namespace vrsa
{
namespace common
{
/**
 * @file GisTypes.h
 *
 * @english
 * @brief Type aliases and smart pointers for GIS system
 *
 * Provides type aliases for Qt geometry types and smart pointers
 * used throughout the VRSA GIS. These aliases ensure consistency
 * and easy replacement if needed.
 * @endenglish
 *
 * @russian
 * @brief Псевдонимы типов и умные указатели для ГИС системы
 *
 * Предоставляет псевдонимы для геометрических типов Qt и умных указателей,
 * используемых в VRSA GIS. Эти псевдонимы обеспечивают единообразие
 * и легкую замену при необходимости.
 * @endrussian
 */

using Point = QPointF;           // 2D точка с координатами double
using BoundingBox = QRectF;      // Ограничивающий прямоугольник
using Size = QSizeF;             // Размер в double
using Color = QColor;            // Цвет


using String = std::string;      // Строка UTF-8
using AttributeValue = std::string; //<Значение атрибута

template<typename T>
using Ptr = std::unique_ptr<T>;  //<Уникальный указатель

template<typename T>
using SharedPtr = std::shared_ptr<T>; // Разделяемый указатель

}
}
#endif // GISTYPES_H
