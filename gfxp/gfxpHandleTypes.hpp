
/**
 * @file gfxpHandleTypes.hpp
 * Declares all types of handles to GPU resources
*/


#ifndef GFXP_HANDLE_TYPES_H
#define GFXP_HANDLE_TYPES_H

#include <cstddef>

namespace gfxp {


        /**
         * @brief GFXP_INVALID_HANDLE
         * Value used to mark that an handle, to any type of GPU resource, is invalid
        */
        constexpr std::nullptr_t GFXP_INVALID_HANDLE = nullptr;


        /**
         * @brief BufferHandle
         * Definition of handle to a GPU buffer resource
        */
        using BufferHandle = void*;


        /**
         * @brief ImageHandle
         * Definition of handle to a GPU texture resource
        */
        using TextureHandle = void*;


        /**
         * @brief ShaderHandle
         * Definition of handle to a GPU shader resource
        */
        using ShaderHandle = void*;


        /**
         * @brief PipelineHandle
         * Definition of handle to a GPU graphics/compute pipeline resource
        */
        using PipelineHandle = void*;

}

#endif // GFXP_HANDLE_TYPES_H
