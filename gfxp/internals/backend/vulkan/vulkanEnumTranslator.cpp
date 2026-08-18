
/**
 * @file vulkanEnumTranslator.cpp
 * Implements the gfxp::backend::EnumsTranslator class
*/

#include "vulkanEnumTranslator.hpp"

namespace gfxp::backend {



        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given ShaderType to a vulkan VkShaderStageFlagBits value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const ShaderType from, VkShaderStageFlagBits& to)
        {
                switch(from)
                {
                        case ShaderType::VERTEX_SHADER:         to = VK_SHADER_STAGE_VERTEX_BIT;        break;
                        case ShaderType::FRAGMENT_SHADER:       to = VK_SHADER_STAGE_FRAGMENT_BIT;      break;

                        default:                                return false; break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given VertexAttributeType to a vulkan VkFormat value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const VertexAttributeType from, VkFormat& to)
        {
                switch(from)
                {
                        case VertexAttributeType::UNKNOWN:      to = VK_FORMAT_UNDEFINED;               break;

                        case VertexAttributeType::INT_8:        to = VK_FORMAT_R8_SINT;                 break;
                        case VertexAttributeType::UINT_8:       to = VK_FORMAT_R8_UINT;                 break;

                        case VertexAttributeType::INT_16:       to = VK_FORMAT_R16_SINT;                break;
                        case VertexAttributeType::UINT_16:      to = VK_FORMAT_R16_UINT;                break;

                        case VertexAttributeType::INT_32:       to = VK_FORMAT_R32_SINT;                break;
                        case VertexAttributeType::UINT_32:      to = VK_FORMAT_R32_UINT;                break;
                        case VertexAttributeType::FLOAT:        to = VK_FORMAT_R32_SFLOAT;              break;

                        case VertexAttributeType::INT_64:       to = VK_FORMAT_R64_SINT;                break;
                        case VertexAttributeType::UINT_64:      to = VK_FORMAT_R64_UINT;                break;
                        case VertexAttributeType::DOUBLE:       to = VK_FORMAT_R64_SFLOAT;              break;

                        case VertexAttributeType::VEC2_INT_8:   to = VK_FORMAT_R8G8_SINT;               break;
                        case VertexAttributeType::VEC2_INT_16:  to = VK_FORMAT_R16G16_SINT;             break;
                        case VertexAttributeType::VEC2_INT_32:  to = VK_FORMAT_R32G32_SINT;             break;
                        case VertexAttributeType::VEC2_INT_64:  to = VK_FORMAT_R64G64_SINT;             break;
                        case VertexAttributeType::VEC2_UINT_8:  to = VK_FORMAT_R8G8_UINT;               break;
                        case VertexAttributeType::VEC2_UINT_16: to = VK_FORMAT_R16G16_UINT;             break;
                        case VertexAttributeType::VEC2_UINT_32: to = VK_FORMAT_R32G32_UINT;             break;
                        case VertexAttributeType::VEC2_UINT_64: to = VK_FORMAT_R64G64_UINT;             break;
                        case VertexAttributeType::VEC2_FLOAT:   to = VK_FORMAT_R32G32_SFLOAT;           break;

                        case VertexAttributeType::VEC3_INT_8:   to = VK_FORMAT_R8G8B8_SINT;             break;
                        case VertexAttributeType::VEC3_INT_16:  to = VK_FORMAT_R16G16B16_SINT;          break;
                        case VertexAttributeType::VEC3_INT_32:  to = VK_FORMAT_R32G32B32_SINT;          break;
                        case VertexAttributeType::VEC3_INT_64:  to = VK_FORMAT_R64G64B64_SINT;          break;
                        case VertexAttributeType::VEC3_UINT_8:  to = VK_FORMAT_R8G8B8_UINT;             break;
                        case VertexAttributeType::VEC3_UINT_16: to = VK_FORMAT_R16G16B16_UINT;          break;
                        case VertexAttributeType::VEC3_UINT_32: to = VK_FORMAT_R32G32B32_UINT;          break;
                        case VertexAttributeType::VEC3_UINT_64: to = VK_FORMAT_R64G64B64_UINT;          break;
                        case VertexAttributeType::VEC3_FLOAT:   to = VK_FORMAT_R32G32B32_SFLOAT;        break;
        
                        case VertexAttributeType::VEC4_INT_8:   to = VK_FORMAT_R8G8B8A8_SINT;           break;
                        case VertexAttributeType::VEC4_INT_16:  to = VK_FORMAT_R16G16B16A16_SINT;       break;
                        case VertexAttributeType::VEC4_INT_32:  to = VK_FORMAT_R32G32B32A32_SINT;       break;
                        case VertexAttributeType::VEC4_INT_64:  to = VK_FORMAT_R64G64B64A64_SINT;       break;
                        case VertexAttributeType::VEC4_UINT_8:  to = VK_FORMAT_R8G8B8A8_UINT;           break;
                        case VertexAttributeType::VEC4_UINT_16: to = VK_FORMAT_R16G16B16A16_UINT;       break;
                        case VertexAttributeType::VEC4_UINT_32: to = VK_FORMAT_R32G32B32A32_UINT;       break;
                        case VertexAttributeType::VEC4_UINT_64: to = VK_FORMAT_R64G64B64A64_UINT;       break;
                        case VertexAttributeType::VEC4_FLOAT:   to = VK_FORMAT_R32G32B32A32_SFLOAT;     break;

                        default:                                return false; break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translateToSize
         * Translate the given VertexAttributeType to the size, expressed in bytes, necessary
         * to store a value of such type
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translateToSize(const VertexAttributeType from, size_t& to)
        {
                switch(from)
                {
                        case VertexAttributeType::UNKNOWN:      to = 0;         break;

                        case VertexAttributeType::INT_8:        to = 1;         break;
                        case VertexAttributeType::UINT_8:       to = 1;         break;

                        case VertexAttributeType::INT_16:       to = 2;         break;
                        case VertexAttributeType::UINT_16:      to = 2;         break;

                        case VertexAttributeType::INT_32:       to = 4;         break;
                        case VertexAttributeType::UINT_32:      to = 4;         break;
                        case VertexAttributeType::FLOAT:        to = 4;         break;

                        case VertexAttributeType::INT_64:       to = 8;         break;
                        case VertexAttributeType::UINT_64:      to = 8;         break;
                        case VertexAttributeType::DOUBLE:       to = 8;         break;

                        case VertexAttributeType::VEC2_INT_8:   to = 2;         break;
                        case VertexAttributeType::VEC2_INT_16:  to = 4;         break;
                        case VertexAttributeType::VEC2_INT_32:  to = 8;         break;
                        case VertexAttributeType::VEC2_INT_64:  to = 16;        break;
                        case VertexAttributeType::VEC2_UINT_8:  to = 2;         break;
                        case VertexAttributeType::VEC2_UINT_16: to = 4;         break;
                        case VertexAttributeType::VEC2_UINT_32: to = 8;         break;
                        case VertexAttributeType::VEC2_UINT_64: to = 16;        break;
                        case VertexAttributeType::VEC2_FLOAT:   to = 8;         break;

                        case VertexAttributeType::VEC3_INT_8:   to = 3;         break;
                        case VertexAttributeType::VEC3_INT_16:  to = 6;         break;
                        case VertexAttributeType::VEC3_INT_32:  to = 12;        break;
                        case VertexAttributeType::VEC3_INT_64:  to = 24;        break;
                        case VertexAttributeType::VEC3_UINT_8:  to = 3;         break;
                        case VertexAttributeType::VEC3_UINT_16: to = 6;         break;
                        case VertexAttributeType::VEC3_UINT_32: to = 12;        break;
                        case VertexAttributeType::VEC3_UINT_64: to = 24;        break;
                        case VertexAttributeType::VEC3_FLOAT:   to = 12;        break;
        
                        case VertexAttributeType::VEC4_INT_8:   to = 4;         break;
                        case VertexAttributeType::VEC4_INT_16:  to = 8;         break;
                        case VertexAttributeType::VEC4_INT_32:  to = 16;        break;
                        case VertexAttributeType::VEC4_INT_64:  to = 32;        break;
                        case VertexAttributeType::VEC4_UINT_8:  to = 4;         break;
                        case VertexAttributeType::VEC4_UINT_16: to = 8;         break;
                        case VertexAttributeType::VEC4_UINT_32: to = 16;        break;
                        case VertexAttributeType::VEC4_UINT_64: to = 32;        break;
                        case VertexAttributeType::VEC4_FLOAT:   to = 16;        break;

                        default:                                return false;   break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given VertexInputRate to a vulkan VkVertexInputRate value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const VertexInputRate from, VkVertexInputRate& to)
        {
                switch(from)
                {
                        case VertexInputRate::PER_VERTEX:       to = VK_VERTEX_INPUT_RATE_VERTEX;       break;
                        case VertexInputRate::PER_INSTANCE:     to = VK_VERTEX_INPUT_RATE_INSTANCE;     break;

                        default:                                        return false; break;
                }

                return true;
        }

        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given PrimitiveTopologyTye to a vulkan VkPrimitiveTopology value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const PrimitiveTopologyType from, VkPrimitiveTopology& to)
        {
                switch(from)
                {
                        case PrimitiveTopologyType::POINT_LIST:         to = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;          break;
                        case PrimitiveTopologyType::LINE_LIST:          to = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;           break;
                        case PrimitiveTopologyType::LINE_STRIP:         to = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;          break;
                        case PrimitiveTopologyType::TRIANGLE_LIST:      to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;       break;
                        case PrimitiveTopologyType::TRIANGLE_STRIP:     to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;      break;
                        case PrimitiveTopologyType::TRIANGLE_FAN:       to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;        break;

                        default:                                        return false; break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given CullMode to a vulkan VkCullModeFlagBits value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const CullMode from, VkCullModeFlagBits& to)
        {
                switch(from)
                {
                        case CullMode::NO_CULLING:              to = VK_CULL_MODE_NONE;                 break;
                        case CullMode::CULL_FRONT_FACE:         to = VK_CULL_MODE_FRONT_BIT;            break;
                        case CullMode::CULL_BACK_FACE:          to = VK_CULL_MODE_BACK_BIT;             break;
                        case CullMode::CULL_FRONT_AND_BACK:     to = VK_CULL_MODE_FRONT_AND_BACK;       break;

                        default:                                return false; break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given TriangleFrontFace to a vulkan VkFrontFace value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const TriangleFrontFace from, VkFrontFace& to)
        {
                switch(from)
                {
                        case TriangleFrontFace::FRONT_FACE_CLOCKWISE:           to = VK_FRONT_FACE_CLOCKWISE;           break;
                        case TriangleFrontFace::FRONT_FACE_COUTER_CLOCKWISE:    to = VK_FRONT_FACE_COUNTER_CLOCKWISE;   break;

                        default:                                                return false; break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given ShaderType to a vulkan VkShaderStage value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const CompareOperatorType from, VkCompareOp& to)
        {
                switch(from)
                {
                        case CompareOperatorType::ALWAYS_FALSE:         to = VK_COMPARE_OP_NEVER;               break;
                        case CompareOperatorType::LESS_THAN:            to = VK_COMPARE_OP_LESS;                break;
                        case CompareOperatorType::EQUAL_TO:             to = VK_COMPARE_OP_EQUAL;               break;
                        case CompareOperatorType::LESS_OR_EQUAL_TO:     to = VK_COMPARE_OP_LESS_OR_EQUAL;       break;
                        case CompareOperatorType::GREATER_THAN:         to = VK_COMPARE_OP_GREATER;             break;
                        case CompareOperatorType::GREATER_OR_EQUAL_TO:  to = VK_COMPARE_OP_GREATER_OR_EQUAL;    break;
                        case CompareOperatorType::NOT_EQUAL_TO:         to = VK_COMPARE_OP_NOT_EQUAL;           break;
                        case CompareOperatorType::ALWAYS_TRUE:          to = VK_COMPARE_OP_ALWAYS;              break;

                        default:                                        return false; break;
                }

                return true;
        }


} // namespace gfxp::backend

