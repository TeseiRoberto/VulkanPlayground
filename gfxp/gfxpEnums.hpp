
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
         * @enum BufferType
         * Enumerates the types of buffers supported by gfxp
        */
        enum class BufferType : uint8_t {
                STAGING_BUFFER = 0,             ///< Buffer as temporary storing location for CPU to GPU data transfer 
                VERTEX_BUFFER,                  ///< GPU local buffer used to store vertex data
                INDEX_BUFFER,                   ///< GPU local buffer used to store index data
        };


        /**
         * @enum ShaderType
         * Enumerates the supported types of shader
        */
        enum class ShaderType : uint8_t {
                UNKNOWN_SHADER = 0,             ///< Unknown shader type, shader is not valid
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
         * @enum PrimitiveTopologyType
         * Enumerates the types of primitive topologies that can be used to group vertices
         * passed as input to a graphics pipeline 
        */
        enum class PrimitiveTopologyType : uint8_t {
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
