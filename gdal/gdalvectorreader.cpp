#include "gdalvectorreader.h"

vrsa::gdalwrapper::GDALVectorReader::GDALVectorReader()
{

}

std::unique_ptr<VectorLayer> vrsa::gdalwrapper::GDALVectorReader::readLayer(const std::string &source)
{
    auto dataset = vrsa::gdalwrapper::createDataset(source, GDAL_OF_VECTOR | GDAL_OF_READONLY);
        if (!dataset) {
            throw std::runtime_error("Failed to open data source: " + source);
        }

        int layerCount = dataset->GetLayerCount();
        if (layerCount == 0) {
            throw std::runtime_error("No layers found in: " + source);
        }

        std::vector<std::unique_ptr<VectorLayer>> layers;
        layers.reserve(layerCount);

        // Обрабатываем все слои
        for (int i = 0; i < layerCount; ++i)
        {
            OGRLayer* layer = dataset->GetLayer(i);
            if (!layer)
            {
                VRSA_ERROR("GDAL", std::string("Warning: Failed to get layer ") + std::to_string(i) + " from: " + source);
                continue;
            }

//            try
//            {
//                auto vector_layer = convertOGRLayerToVectorLayer(dataset, layer, source, i);
//                if (vector_layer)
//                    layers.push_back(std::move(vector_layer));
//            }
//            catch (const std::exception& e)
//            {
//                std::cerr << "Warning: Failed to convert layer " << i << ": " << e.what() << std::endl;
//            }
        }

        if (layers.empty()) {
            throw std::runtime_error("No valid layers could be read from: " + source);
        }

        //return layers;
}


