#include "spatialoperation.h"


vrsa::vector::SpatialOperation::SpatialOperation(const common::SpatialOperationDTO &dto, VectorLayerCreator *creator)
    : mParams{dto},
      mCreator{creator}
{

}
