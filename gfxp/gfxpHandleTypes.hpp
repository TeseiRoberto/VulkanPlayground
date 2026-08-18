
/**
 * @file gfxpHandleTypes.hpp
 * Declares all types of handles to GPU resources
*/


#ifndef GFXP_HANDLE_TYPES_H
#define GFXP_HANDLE_TYPES_H

#include <cstdint>
#include <limits>

namespace gfxp {


        /**
         * @brief GFXP_INVALID_HANDLE
         * Value used to mark that an handle, to any type of GPU resource, is invalid
        */
        using GFXP_INVALID_HANDLE = std::numeric_limits<uint32_t>::max();


        /**
         * @brief BufferHandle
         * Definition of handle to a GPU buffer resource
        */
        using BufferHandle = uint32_t;


        /**
         * @brief ImageHandle
         * Definition of handle to a GPU image resource
        */
        using ImageHandle = uint32_t;


        /**
         * @brief ShaderHandle
         * Definition of handle to a GPU shader resource
        */
        using ShaderHandle = uint32_t;


        /**
         * @brief PipelineHandle
         * Definition of handle to a GPU graphics/compute pipeline resource
        */
        using PipelineHandle = uint32_t;

}

#endif // GFXP_HANDLE_TYPES_H
