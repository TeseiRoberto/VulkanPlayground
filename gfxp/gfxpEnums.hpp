
/**
 * @file gfxpEnums.hpp
 * Declares common enum types used all around gfxp code
*/

#ifndef GFXP_ENUMS_H
#define GFXP_ENUMS_H

#include <cstdint>

namespace gfxp {


        /**
         * @enum GraphicApi
         * Enumerates the graphics API backends supported by gfxp
        */
        enum class GraphicApi : uint8_t {
                VULKAN = 0                      ///< Vulkan API backend
        };


        /**
         * @enum BufferUsage
         * Enumerates all the possible usages of a buffer resource,
         * those values can be or-ed together to create a bitmask 
        */
        enum BufferUsage : uint32_t {
                UNKNOWN_BUFFER_USAGE    = 0,                                                    ///< Default dummy value
                TRANSFER_SRC_BUFFER     = 1 << 0,                                               ///< Buffer is used as source for generic data transfer commands
                TRANSFER_DST_BUFFER     = 1 << 1,                                               ///< Buffer is used as destination for generic data transfer commands
                VERTEX_BUFFER           = (1 << 2) | TRANSFER_DST_BUFFER,                       ///< Buffer is used to store vertex data
                INDEX_BUFFER            = (1 << 3) | TRANSFER_DST_BUFFER,                       ///< Buffer is used to store index data
                STAGING_BUFFER          = (1 << 4) | TRANSFER_SRC_BUFFER | TRANSFER_DST_BUFFER, ///< Buffer is used as temporary storage for CPU to GPU, and GPU to CPU data transfer
        };

        using BufferUsageFlags = uint32_t;


        /**
         * @enum TextureType
         * Enumerates the types of texture supported by gfxp
        */
        enum class TextureType : uint8_t {
                UNKNOWN = 0,                    ///< Default dummy value
                TEXTURE_1D,                     ///< A one dimensional texture
                TEXTURE_2D,                     ///< A two dimensional texture
                TEXTURE_3D,                     ///< A three dimensional texture
        };


        /**
         * @enum TextureUsage
         * Enumerates all the possible usages of a texture resource,
         * those values can be or-ed together to create a bitmask 
        */
        enum TextureUsage : uint32_t {
                UNKNOWN_TEXTURE_USAGE           = 0,            ///< Default dummy value
                TRANSFER_SRC_TEXTURE            = 1 << 0,       ///< Texture is used as source for generic data transfer commands
                TRANSFER_DST_TEXTURE            = 1 << 1,       ///< Texture is used as destination for generic data transfer commands
                SAMPLED_TEXTURE                 = 1 << 2,       ///< Texture is used as source for sampling operations
                STORAGE_TEXTURE                 = 1 << 3,       ///< Texture is used for reading/writing generic data
                INPUT_ATTACHMENT                = 1 << 4,       ///< Texture is used as input for a fragment shader
                COLOR_ATTACHMENT                = 1 << 5,       ///< Texture is used as a color attachment (render target) for a graphics pipeline
                DEPTH_STENCIL_ATTACHMENT        = 1 << 6,       ///< Texture is used as a depth/stencil attachment (render target) for a graphics pipeline
        };

        using TextureUsageFlags = uint32_t;


        /**
         * @enum TextureFormat
         * Enumerates the texture format supported by gfxp
        */
        enum class TextureFormat {
                UNKNOWN = 0,
                R4G4_UNORM_PACK8,
                R4G4B4A4_UNORM_PACK16,
                B4G4R4A4_UNORM_PACK16,
                R5G6B5_UNORM_PACK16,
                B5G6R5_UNORM_PACK16,
                R5G5B5A1_UNORM_PACK16,
                B5G5R5A1_UNORM_PACK16,
                A1R5G5B5_UNORM_PACK16,
                R8_UNORM,
                R8_SNORM,
                R8_USCALED,
                R8_SSCALED,
                R8_UINT,
                R8_SINT,
                R8_SRGB,
                R8G8_UNORM,
                R8G8_SNORM,
                R8G8_USCALED,
                R8G8_SSCALED,
                R8G8_UINT,
                R8G8_SINT,
                R8G8_SRGB,
                R8G8B8_UNORM,
                R8G8B8_SNORM,
                R8G8B8_USCALED,
                R8G8B8_SSCALED,
                R8G8B8_UINT,
                R8G8B8_SINT,
                R8G8B8_SRGB,
                B8G8R8_UNORM,
                B8G8R8_SNORM,
                B8G8R8_USCALED,
                B8G8R8_SSCALED,
                B8G8R8_UINT,
                B8G8R8_SINT,
                B8G8R8_SRGB,
                R8G8B8A8_UNORM,
                R8G8B8A8_SNORM,
                R8G8B8A8_USCALED,
                R8G8B8A8_SSCALED,
                R8G8B8A8_UINT,
                R8G8B8A8_SINT,
                R8G8B8A8_SRGB,
                B8G8R8A8_UNORM,
                B8G8R8A8_SNORM,
                B8G8R8A8_USCALED,
                B8G8R8A8_SSCALED,
                B8G8R8A8_UINT,
                B8G8R8A8_SINT,
                B8G8R8A8_SRGB,
                A8B8G8R8_UNORM_PACK32,
                A8B8G8R8_SNORM_PACK32,
                A8B8G8R8_USCALED_PACK32,
                A8B8G8R8_SSCALED_PACK32,
                A8B8G8R8_UINT_PACK32,
                A8B8G8R8_SINT_PACK32,
                A8B8G8R8_SRGB_PACK32,
                A2R10G10B10_UNORM_PACK32,
                A2R10G10B10_SNORM_PACK32,
                A2R10G10B10_USCALED_PACK32,
                A2R10G10B10_SSCALED_PACK32,
                A2R10G10B10_UINT_PACK32,
                A2R10G10B10_SINT_PACK32,
                A2B10G10R10_UNORM_PACK32,
                A2B10G10R10_SNORM_PACK32,
                A2B10G10R10_USCALED_PACK32,
                A2B10G10R10_SSCALED_PACK32,
                A2B10G10R10_UINT_PACK32,
                A2B10G10R10_SINT_PACK32,
                R16_UNORM,
                R16_SNORM,
                R16_USCALED,
                R16_SSCALED,
                R16_UINT,
                R16_SINT,
                R16_SFLOAT,
                R16G16_UNORM,
                R16G16_SNORM,
                R16G16_USCALED,
                R16G16_SSCALED,
                R16G16_UINT,
                R16G16_SINT,
                R16G16_SFLOAT,
                R16G16B16_UNORM,
                R16G16B16_SNORM,
                R16G16B16_USCALED,
                R16G16B16_SSCALED,
                R16G16B16_UINT,
                R16G16B16_SINT,
                R16G16B16_SFLOAT,
                R16G16B16A16_UNORM,
                R16G16B16A16_SNORM,
                R16G16B16A16_USCALED,
                R16G16B16A16_SSCALED,
                R16G16B16A16_UINT,
                R16G16B16A16_SINT,
                R16G16B16A16_SFLOAT,
                R32_UINT,
                R32_SINT,
                R32_SFLOAT,
                R32G32_UINT,
                R32G32_SINT,
                R32G32_SFLOAT,
                R32G32B32_UINT,
                R32G32B32_SINT,
                R32G32B32_SFLOAT,
                R32G32B32A32_UINT,
                R32G32B32A32_SINT,
                R32G32B32A32_SFLOAT,
                R64_UINT,
                R64_SINT,
                R64_SFLOAT,
                R64G64_UINT,
                R64G64_SINT,
                R64G64_SFLOAT,
                R64G64B64_UINT,
                R64G64B64_SINT,
                R64G64B64_SFLOAT,
                R64G64B64A64_UINT,
                R64G64B64A64_SINT,
                R64G64B64A64_SFLOAT,
                B10G11R11_UFLOAT_PACK32,
                E5B9G9R9_UFLOAT_PACK32,
                D16_UNORM,
                X8_D24_UNORM_PACK32,
                D32_SFLOAT,
                S8_UINT,
                D16_UNORM_S8_UINT,
                D24_UNORM_S8_UINT,
                D32_SFLOAT_S8_UINT,
                BC1_RGB_UNORM_BLOCK,
                BC1_RGB_SRGB_BLOCK,
                BC1_RGBA_UNORM_BLOCK,
                BC1_RGBA_SRGB_BLOCK,
                BC2_UNORM_BLOCK,
                BC2_SRGB_BLOCK,
                BC3_UNORM_BLOCK,
                BC3_SRGB_BLOCK,
                BC4_UNORM_BLOCK,
                BC4_SNORM_BLOCK,
                BC5_UNORM_BLOCK,
                BC5_SNORM_BLOCK,
                BC6H_UFLOAT_BLOCK,
                BC6H_SFLOAT_BLOCK,
                BC7_UNORM_BLOCK,
                BC7_SRGB_BLOCK,
                ETC2_R8G8B8_UNORM_BLOCK,
                ETC2_R8G8B8_SRGB_BLOCK,
                ETC2_R8G8B8A1_UNORM_BLOCK,
                ETC2_R8G8B8A1_SRGB_BLOCK,
                ETC2_R8G8B8A8_UNORM_BLOCK,
                ETC2_R8G8B8A8_SRGB_BLOCK,
                EAC_R11_UNORM_BLOCK,
                EAC_R11_SNORM_BLOCK,
                EAC_R11G11_UNORM_BLOCK,
                EAC_R11G11_SNORM_BLOCK,
                ASTC_4x4_UNORM_BLOCK,
                ASTC_4x4_SRGB_BLOCK,
                ASTC_5x4_UNORM_BLOCK,
                ASTC_5x4_SRGB_BLOCK,
                ASTC_5x5_UNORM_BLOCK,
                ASTC_5x5_SRGB_BLOCK,
                ASTC_6x5_UNORM_BLOCK,
                ASTC_6x5_SRGB_BLOCK,
                ASTC_6x6_UNORM_BLOCK,
                ASTC_6x6_SRGB_BLOCK,
                ASTC_8x5_UNORM_BLOCK,
                ASTC_8x5_SRGB_BLOCK,
                ASTC_8x6_UNORM_BLOCK,
                ASTC_8x6_SRGB_BLOCK,
                ASTC_8x8_UNORM_BLOCK,
                ASTC_8x8_SRGB_BLOCK,
                ASTC_10x5_UNORM_BLOCK,
                ASTC_10x5_SRGB_BLOCK,
                ASTC_10x6_UNORM_BLOCK,
                ASTC_10x6_SRGB_BLOCK,
                ASTC_10x8_UNORM_BLOCK,
                ASTC_10x8_SRGB_BLOCK,
                ASTC_10x10_UNORM_BLOCK,
                ASTC_10x10_SRGB_BLOCK,
                ASTC_12x10_UNORM_BLOCK,
                ASTC_12x10_SRGB_BLOCK,
                ASTC_12x12_UNORM_BLOCK,
                ASTC_12x12_SRGB_BLOCK,
        };


        /**
         * @enum TextureSampleCount
         * Enumerates the available number of samples to sample a texture
        */
        enum class TextureSampleCount {
                SAMPLE_COUNT_1 = 0,             ///< 1sample per pixel will be used
                SAMPLE_COUNT_2,                 ///< 2 samples per pixel will be used
                SAMPLE_COUNT_4,                 ///< 4 samples per pixel will be used
                SAMPLE_COUNT_8,                 ///< 8 samples per pixel will be used
                SAMPLE_COUNT_16,                ///< 16 samples per pixel will be used
                SAMPLE_COUNT_32,                ///< 32 samples per pixel will be used
                SAMPLE_COUNT_64,                ///< 64 samples per pixel will be used
        };


        /**
         * @enum ShaderType
         * Enumerates the types of shaders supported by gfxp
        */
        enum class ShaderType : uint8_t {
                UNKNOWN = 0,                    ///< Unknown shader type, shader is not valid
                VERTEX_SHADER,                  ///< Vertex shader
                FRAGMENT_SHADER,                ///< Fragment shader
        };


        /**
         * @enum VertexAttributeType
         * Enumerates the data types that can be used as vertex shader input attributes
        */
        enum class VertexAttributeType : uint8_t {
                UNKNOWN = 0,            ///< Unknown attribute type

                INT_8,                  ///< Signed int, 8 bit
                UINT_8,                 ///< Unsigned int, 8 bit

                INT_16,                 ///< Signed int, 16 bit
                UINT_16,                ///< Signed int, 16 bit

                INT_32,                 ///< Signed int, 32 bit
                UINT_32,                ///< Unsigned int, 32 bit
                FLOAT,                  ///< Floating point, 32 bit
                
                INT_64,                 ///< Signed int, 64 bit
                UINT_64,                ///< Unsigned int, 64 bit
                DOUBLE,                 ///< Double precision floating point, 64 bit

                VEC2_INT_8,             ///< Two signed integers, 8 bit each
                VEC2_INT_16,            ///< Two signed integers, 16 bit each
                VEC2_INT_32,            ///< Two signed integers, 32 bit each
                VEC2_INT_64,            ///< Two signed integers, 64 bit each
                VEC2_UINT_8,            ///< Two unsigned integers, 8 bit each
                VEC2_UINT_16,           ///< Two unsigned integers, 16 bit each
                VEC2_UINT_32,           ///< Two unsigned integers, 32 bit each
                VEC2_UINT_64,           ///< Two unigned integers, 64 bit each
                VEC2_FLOAT,             ///< Two floating point, 32 bit each

                VEC3_INT_8,             ///< Three signed integers, 8 bit each
                VEC3_INT_16,            ///< Three signed integers, 16 bit each
                VEC3_INT_32,            ///< Three signed integers, 32 bit each
                VEC3_INT_64,            ///< Three signed integers, 64 bit each
                VEC3_UINT_8,            ///< Three unsigned integers, 8 bit each
                VEC3_UINT_16,           ///< Three unsigned integers, 16 bit each
                VEC3_UINT_32,           ///< Three unsigned integers, 32 bit each
                VEC3_UINT_64,           ///< Three unsigned integers, 64 bit each
                VEC3_FLOAT,             ///< Three floating point, 32 bit each
        
                VEC4_INT_8,             ///< Four signed integers, 8 bit each
                VEC4_INT_16,            ///< Four signed integers, 16 bit each
                VEC4_INT_32,            ///< Four signed integers, 32 bit each
                VEC4_INT_64,            ///< Four signed integers, 64 bit each
                VEC4_UINT_8,            ///< Four unsigned integers, 8 bit each
                VEC4_UINT_16,           ///< Four unsigned integers, 16 bit each
                VEC4_UINT_32,           ///< Four unsigned integers, 32 bit each
                VEC4_UINT_64,           ///< Four unsigned integers, 64 bit each
                VEC4_FLOAT,             ///< Four floating point, 32 bit each
        };


        /**
         * @enum VertexInputRate
         * Enumerates the ways in which vertex attributes can be fetched from a vertex buffer
        */
        enum class VertexInputRate : uint8_t {
                PER_VERTEX = 0,         ///< Attribute is fetched for each vertex
                PER_INSTANCE,           ///< Attribute is fetched for each instance (used in instanced rendering)
        };


        /**
         * @enum PrimitiveType
         * Enumerates the types of primitive topologies that can be used to group vertices
         * passed as input to a graphics pipeline 
        */
        enum class PrimitiveType : uint8_t {
                POINT_LIST,             ///< The input vertices defines a series of separate points
                LINE_LIST,              ///< The input vertices defines a series of separate line segments
                LINE_STRIP,             ///< The input vertices defines a series of connected line segments with consecutive lines sharing a vertex
                TRIANGLE_LIST,          ///< The input vertices defines a series of separate triangles
                TRIANGLE_STRIP,         ///< The input vertices defines a series of connected triangles with consecutive triangles sharing an edge
                TRIANGLE_FAN,           ///< The input vertices defines a series of connected triangles with consecutive triangles sharing one vertex
        };


        /**
         * @enum CullMode
         * Enumerates the types of culling that can be performed on rendered triangles
        */
        enum class CullMode : uint8_t {
                NO_CULLING,             ///< No triangles are discarded
                CULL_FRONT_FACE,        ///< Only front facing triangles are discarded
                CULL_BACK_FACE,         ///< Only back facing triangles are discarded
                CULL_FRONT_AND_BACK     ///< All triangles are discarded
        };


        /**
         * @enum TriangleFrontFace
         * Enumerates the ways to determine wich face of a triangle shall be considered the visible/front face
        */
        enum class TriangleFrontFace : uint8_t {
                FRONT_FACE_CLOCKWISE,           ///< Triangles with a clockwise winding order are considered front facing
                FRONT_FACE_COUTER_CLOCKWISE     ///< Triangles with a counter clockwise winding order are considered front facing
        };


        /**
         * @enum CompareOperatorType
         * Enumerates the types of compare operators that can be used for depth/stencil and sampler operations
        */
        enum class CompareOperatorType : uint8_t {
                ALWAYS_FALSE,           ///< The comparison will always fail (evaluate to false)
                LESS_THAN,              ///< The comparison will evaluate "reference < test"
                EQUAL_TO,               ///< The comparison will evaluate "reference == test"
                LESS_OR_EQUAL_TO,       ///< The comparison will evaluate "reference <= test"
                GREATER_THAN,           ///< The comparison will evaluate "reference > test"
                GREATER_OR_EQUAL_TO,    ///< The comparison will evaluate "reference >= test"
                NOT_EQUAL_TO,           ///< The comparison will evaluate "reference != test"
                ALWAYS_TRUE,            ///< The comparison will always pass (evaluate to true)
        };



} // namespace gfxp

#endif // GFXP_ENUMS_H
