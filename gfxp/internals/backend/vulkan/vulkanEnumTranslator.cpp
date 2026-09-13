
/**
 * @file vulkanEnumTranslator.cpp
 * Implements the gfxp::backend::EnumsTranslator class
*/

#include "vulkanEnumTranslator.hpp"

namespace gfxp::backend {


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given BufferUsageFlags to a vulkan VkBufferUsageFlags value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const gfxp::BufferUsageFlags from, VkBufferUsageFlags& to)
        {
                if(from == gfxp::BufferUsage::UNKNOWN_BUFFER_USAGE)
                        return false;

                bool result = false;
                to = 0;

                if(from & gfxp::BufferUsage::TRANSFER_SRC_BUFFER)
                {
                        to |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                        result = true;
                }

                if(from & gfxp::BufferUsage::TRANSFER_DST_BUFFER)
                {
                        to |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                        result = true;
                }

                if(from & gfxp::BufferUsage::VERTEX_BUFFER)
                {
                        to |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                        result = true;
                }

                if(from & gfxp::BufferUsage::INDEX_BUFFER)
                {
                        to |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
                        result = true;
                }

                if(from & gfxp::BufferUsage::STAGING_BUFFER)
                {
                        to |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                        to |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                        result = true;
                }

                return result;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given TextureType to a vulkan VkImageType value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const gfxp::TextureType from, VkImageType& to)
        {
                switch(from)
                {
                        case gfxp::TextureType::TEXTURE_1D:     to = VK_IMAGE_TYPE_1D;          break;
                        case gfxp::TextureType::TEXTURE_2D:     to = VK_IMAGE_TYPE_2D;          break;
                        case gfxp::TextureType::TEXTURE_3D:     to = VK_IMAGE_TYPE_3D;          break;

                        default:                                return false;                   break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given TextureUsage to a vulkan VkImageUsageFlags value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const gfxp::TextureUsage from, VkImageUsageFlags& to)
        {
                bool result = false;

                if(from & TextureUsage::UNKNOWN_TEXTURE_USAGE)
                        return false;

                if(from & gfxp::TextureUsage::TRANSFER_SRC_TEXTURE)
                {
                        to |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::TRANSFER_DST_TEXTURE)
                {
                        to |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::SAMPLED_TEXTURE)
                {
                        to |= VK_IMAGE_USAGE_SAMPLED_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::STORAGE_TEXTURE)
                {
                        to |= VK_IMAGE_USAGE_STORAGE_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::INPUT_ATTACHMENT)
                {
                        to |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::COLOR_ATTACHMENT)
                {
                        to |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
                        result = true;
                }

                if(from & gfxp::TextureUsage::DEPTH_STENCIL_ATTACHMENT)
                {
                        to |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                        result = true;
                }
        
                return result;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given TextureFormat to a vulkan VkFormat value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const gfxp::TextureFormat from, VkFormat& to)
        {
                switch(from)
                {
                        case gfxp::TextureFormat::R4G4_UNORM_PACK8:             to = VK_FORMAT_R4G4_UNORM_PACK8;                break;
                        case gfxp::TextureFormat::R4G4B4A4_UNORM_PACK16:        to = VK_FORMAT_R4G4B4A4_UNORM_PACK16;           break;
                        case gfxp::TextureFormat::B4G4R4A4_UNORM_PACK16:        to = VK_FORMAT_B4G4R4A4_UNORM_PACK16;           break;
                        case gfxp::TextureFormat::R5G6B5_UNORM_PACK16:          to = VK_FORMAT_R5G6B5_UNORM_PACK16;             break;
                        case gfxp::TextureFormat::B5G6R5_UNORM_PACK16:          to = VK_FORMAT_B5G6R5_UNORM_PACK16;             break;
                        case gfxp::TextureFormat::R5G5B5A1_UNORM_PACK16:        to = VK_FORMAT_R5G5B5A1_UNORM_PACK16;           break;
                        case gfxp::TextureFormat::B5G5R5A1_UNORM_PACK16:        to = VK_FORMAT_B5G5R5A1_UNORM_PACK16;           break;
                        case gfxp::TextureFormat::A1R5G5B5_UNORM_PACK16:        to = VK_FORMAT_A1R5G5B5_UNORM_PACK16;           break;
                        case gfxp::TextureFormat::R8_UNORM:                     to = VK_FORMAT_R8_UNORM;                        break;
                        case gfxp::TextureFormat::R8_SNORM:                     to = VK_FORMAT_R8_SNORM;                        break;
                        case gfxp::TextureFormat::R8_USCALED:                   to = VK_FORMAT_R8_USCALED;                      break;
                        case gfxp::TextureFormat::R8_SSCALED:                   to = VK_FORMAT_R8_SSCALED;                      break;
                        case gfxp::TextureFormat::R8_UINT:                      to = VK_FORMAT_R8_UINT;                         break;
                        case gfxp::TextureFormat::R8_SINT:                      to = VK_FORMAT_R8_SINT;                         break;
                        case gfxp::TextureFormat::R8_SRGB:                      to = VK_FORMAT_R8_SRGB;                         break;
                        case gfxp::TextureFormat::R8G8_UNORM:                   to = VK_FORMAT_R8G8_UNORM;                      break;
                        case gfxp::TextureFormat::R8G8_SNORM:                   to = VK_FORMAT_R8G8_SNORM;                      break;
                        case gfxp::TextureFormat::R8G8_USCALED:                 to = VK_FORMAT_R8G8_USCALED;                    break;
                        case gfxp::TextureFormat::R8G8_SSCALED:                 to = VK_FORMAT_R8G8_SSCALED;                    break;
                        case gfxp::TextureFormat::R8G8_UINT:                    to = VK_FORMAT_R8G8_UINT;                       break;
                        case gfxp::TextureFormat::R8G8_SINT:                    to = VK_FORMAT_R8G8_SINT;                       break;
                        case gfxp::TextureFormat::R8G8_SRGB:                    to = VK_FORMAT_R8G8_SRGB;                       break;
                        case gfxp::TextureFormat::R8G8B8_UNORM:                 to = VK_FORMAT_R8G8B8_UNORM;                    break;
                        case gfxp::TextureFormat::R8G8B8_SNORM:                 to = VK_FORMAT_R8G8B8_SNORM;                    break;
                        case gfxp::TextureFormat::R8G8B8_USCALED:               to = VK_FORMAT_R8G8B8_USCALED;                  break;
                        case gfxp::TextureFormat::R8G8B8_SSCALED:               to = VK_FORMAT_R8G8B8_SSCALED;                  break;
                        case gfxp::TextureFormat::R8G8B8_UINT:                  to = VK_FORMAT_R8G8B8_UINT;                     break;
                        case gfxp::TextureFormat::R8G8B8_SINT:                  to = VK_FORMAT_R8G8B8_SINT;                     break;
                        case gfxp::TextureFormat::R8G8B8_SRGB:                  to = VK_FORMAT_R8G8B8_SRGB;                     break;
                        case gfxp::TextureFormat::B8G8R8_UNORM:                 to = VK_FORMAT_B8G8R8_UNORM;                    break;
                        case gfxp::TextureFormat::B8G8R8_SNORM:                 to = VK_FORMAT_B8G8R8_SNORM;                    break;
                        case gfxp::TextureFormat::B8G8R8_USCALED:               to = VK_FORMAT_B8G8R8_USCALED;                  break;
                        case gfxp::TextureFormat::B8G8R8_SSCALED:               to = VK_FORMAT_B8G8R8_SSCALED;                  break;
                        case gfxp::TextureFormat::B8G8R8_UINT:                  to = VK_FORMAT_B8G8R8_UINT;                     break;
                        case gfxp::TextureFormat::B8G8R8_SINT:                  to = VK_FORMAT_B8G8R8_SINT;                     break;
                        case gfxp::TextureFormat::B8G8R8_SRGB:                  to = VK_FORMAT_B8G8R8_SRGB;                     break;
                        case gfxp::TextureFormat::R8G8B8A8_UNORM:               to = VK_FORMAT_R8G8B8A8_UNORM;                  break;
                        case gfxp::TextureFormat::R8G8B8A8_SNORM:               to = VK_FORMAT_R8G8B8A8_SNORM;                  break;
                        case gfxp::TextureFormat::R8G8B8A8_USCALED:             to = VK_FORMAT_R8G8B8A8_USCALED;                break;
                        case gfxp::TextureFormat::R8G8B8A8_SSCALED:             to = VK_FORMAT_R8G8B8A8_SSCALED;                break;
                        case gfxp::TextureFormat::R8G8B8A8_UINT:                to = VK_FORMAT_R8G8B8A8_UINT;                   break;
                        case gfxp::TextureFormat::R8G8B8A8_SINT:                to = VK_FORMAT_R8G8B8A8_SINT;                   break;
                        case gfxp::TextureFormat::R8G8B8A8_SRGB:                to = VK_FORMAT_R8G8B8A8_SRGB;                   break;
                        case gfxp::TextureFormat::B8G8R8A8_UNORM:               to = VK_FORMAT_B8G8R8A8_UNORM;                  break;
                        case gfxp::TextureFormat::B8G8R8A8_SNORM:               to = VK_FORMAT_B8G8R8A8_SNORM;                  break;
                        case gfxp::TextureFormat::B8G8R8A8_USCALED:             to = VK_FORMAT_B8G8R8A8_USCALED;                break;
                        case gfxp::TextureFormat::B8G8R8A8_SSCALED:             to = VK_FORMAT_B8G8R8A8_SSCALED;                break;
                        case gfxp::TextureFormat::B8G8R8A8_UINT:                to = VK_FORMAT_B8G8R8A8_UINT;                   break;
                        case gfxp::TextureFormat::B8G8R8A8_SINT:                to = VK_FORMAT_B8G8R8A8_SINT;                   break;
                        case gfxp::TextureFormat::B8G8R8A8_SRGB:                to = VK_FORMAT_B8G8R8A8_SRGB;                   break;
                        case gfxp::TextureFormat::A8B8G8R8_UNORM_PACK32:        to = VK_FORMAT_A8B8G8R8_UNORM_PACK32;           break;
                        case gfxp::TextureFormat::A8B8G8R8_SNORM_PACK32:        to = VK_FORMAT_A8B8G8R8_SNORM_PACK32;           break;
                        case gfxp::TextureFormat::A8B8G8R8_USCALED_PACK32:      to = VK_FORMAT_A8B8G8R8_USCALED_PACK32;         break;
                        case gfxp::TextureFormat::A8B8G8R8_SSCALED_PACK32:      to = VK_FORMAT_A8B8G8R8_SSCALED_PACK32;         break;
                        case gfxp::TextureFormat::A8B8G8R8_UINT_PACK32:         to = VK_FORMAT_A8B8G8R8_UINT_PACK32;            break;
                        case gfxp::TextureFormat::A8B8G8R8_SINT_PACK32:         to = VK_FORMAT_A8B8G8R8_SINT_PACK32;            break;
                        case gfxp::TextureFormat::A8B8G8R8_SRGB_PACK32:         to = VK_FORMAT_A8B8G8R8_SRGB_PACK32;            break;
                        case gfxp::TextureFormat::A2R10G10B10_UNORM_PACK32:     to = VK_FORMAT_A2R10G10B10_UNORM_PACK32;        break;
                        case gfxp::TextureFormat::A2R10G10B10_SNORM_PACK32:     to = VK_FORMAT_A2R10G10B10_SNORM_PACK32;        break;
                        case gfxp::TextureFormat::A2R10G10B10_USCALED_PACK32:   to = VK_FORMAT_A2R10G10B10_USCALED_PACK32;      break;
                        case gfxp::TextureFormat::A2R10G10B10_SSCALED_PACK32:   to = VK_FORMAT_A2R10G10B10_SSCALED_PACK32;      break;
                        case gfxp::TextureFormat::A2R10G10B10_UINT_PACK32:      to = VK_FORMAT_A2R10G10B10_UINT_PACK32;         break;
                        case gfxp::TextureFormat::A2R10G10B10_SINT_PACK32:      to = VK_FORMAT_A2R10G10B10_SINT_PACK32;         break;
                        case gfxp::TextureFormat::A2B10G10R10_UNORM_PACK32:     to = VK_FORMAT_A2B10G10R10_UNORM_PACK32;        break;
                        case gfxp::TextureFormat::A2B10G10R10_SNORM_PACK32:     to = VK_FORMAT_A2B10G10R10_SNORM_PACK32;        break;
                        case gfxp::TextureFormat::A2B10G10R10_USCALED_PACK32:   to = VK_FORMAT_A2B10G10R10_USCALED_PACK32;      break;
                        case gfxp::TextureFormat::A2B10G10R10_SSCALED_PACK32:   to = VK_FORMAT_A2B10G10R10_SSCALED_PACK32;      break;
                        case gfxp::TextureFormat::A2B10G10R10_UINT_PACK32:      to = VK_FORMAT_A2B10G10R10_UINT_PACK32;         break;
                        case gfxp::TextureFormat::A2B10G10R10_SINT_PACK32:      to = VK_FORMAT_A2B10G10R10_SINT_PACK32;         break;
                        case gfxp::TextureFormat::R16_UNORM:                    to = VK_FORMAT_R16_UNORM;                       break;
                        case gfxp::TextureFormat::R16_SNORM:                    to = VK_FORMAT_R16_SNORM;                       break;
                        case gfxp::TextureFormat::R16_USCALED:                  to = VK_FORMAT_R16_USCALED;                     break;
                        case gfxp::TextureFormat::R16_SSCALED:                  to = VK_FORMAT_R16_SSCALED;                     break;
                        case gfxp::TextureFormat::R16_UINT:                     to = VK_FORMAT_R16_UINT;                        break;
                        case gfxp::TextureFormat::R16_SINT:                     to = VK_FORMAT_R16_SINT;                        break;
                        case gfxp::TextureFormat::R16_SFLOAT:                   to = VK_FORMAT_R16_SFLOAT;                      break;
                        case gfxp::TextureFormat::R16G16_UNORM:                 to = VK_FORMAT_R16G16_UNORM;                    break;
                        case gfxp::TextureFormat::R16G16_SNORM:                 to = VK_FORMAT_R16G16_SNORM;                    break;
                        case gfxp::TextureFormat::R16G16_USCALED:               to = VK_FORMAT_R16G16_USCALED;                  break;
                        case gfxp::TextureFormat::R16G16_SSCALED:               to = VK_FORMAT_R16G16_SSCALED;                  break;
                        case gfxp::TextureFormat::R16G16_UINT:                  to = VK_FORMAT_R16G16_UINT;                     break;
                        case gfxp::TextureFormat::R16G16_SINT:                  to = VK_FORMAT_R16G16_SINT;                     break;
                        case gfxp::TextureFormat::R16G16_SFLOAT:                to = VK_FORMAT_R16G16_SFLOAT;                   break;
                        case gfxp::TextureFormat::R16G16B16_UNORM:              to = VK_FORMAT_R16G16B16_UNORM;                 break;
                        case gfxp::TextureFormat::R16G16B16_SNORM:              to = VK_FORMAT_R16G16B16_SNORM;                 break;
                        case gfxp::TextureFormat::R16G16B16_USCALED:            to = VK_FORMAT_R16G16B16_USCALED;               break;
                        case gfxp::TextureFormat::R16G16B16_SSCALED:            to = VK_FORMAT_R16G16B16_SSCALED;               break;
                        case gfxp::TextureFormat::R16G16B16_UINT:               to = VK_FORMAT_R16G16B16_UINT;                  break;
                        case gfxp::TextureFormat::R16G16B16_SINT:               to = VK_FORMAT_R16G16B16_SINT;                  break;
                        case gfxp::TextureFormat::R16G16B16_SFLOAT:             to = VK_FORMAT_R16G16B16_SFLOAT;                break;
                        case gfxp::TextureFormat::R16G16B16A16_UNORM:           to = VK_FORMAT_R16G16B16A16_UNORM;              break;
                        case gfxp::TextureFormat::R16G16B16A16_SNORM:           to = VK_FORMAT_R16G16B16A16_SNORM;              break;
                        case gfxp::TextureFormat::R16G16B16A16_USCALED:         to = VK_FORMAT_R16G16B16A16_USCALED;            break;
                        case gfxp::TextureFormat::R16G16B16A16_SSCALED:         to = VK_FORMAT_R16G16B16A16_SSCALED;            break;
                        case gfxp::TextureFormat::R16G16B16A16_UINT:            to = VK_FORMAT_R16G16B16A16_UINT;               break;
                        case gfxp::TextureFormat::R16G16B16A16_SINT:            to = VK_FORMAT_R16G16B16A16_SINT;               break;
                        case gfxp::TextureFormat::R16G16B16A16_SFLOAT:          to = VK_FORMAT_R16G16B16A16_SFLOAT;             break;
                        case gfxp::TextureFormat::R32_UINT:                     to = VK_FORMAT_R32_UINT;                        break;
                        case gfxp::TextureFormat::R32_SINT:                     to = VK_FORMAT_R32_SINT;                        break;
                        case gfxp::TextureFormat::R32_SFLOAT:                   to = VK_FORMAT_R32_SFLOAT;                      break;
                        case gfxp::TextureFormat::R32G32_UINT:                  to = VK_FORMAT_R32G32_UINT;                     break;
                        case gfxp::TextureFormat::R32G32_SINT:                  to = VK_FORMAT_R32G32_SINT;                     break;
                        case gfxp::TextureFormat::R32G32_SFLOAT:                to = VK_FORMAT_R32G32_SFLOAT;                   break;
                        case gfxp::TextureFormat::R32G32B32_UINT:               to = VK_FORMAT_R32G32B32_UINT;                  break;
                        case gfxp::TextureFormat::R32G32B32_SINT:               to = VK_FORMAT_R32G32B32_SINT;                  break;
                        case gfxp::TextureFormat::R32G32B32_SFLOAT:             to = VK_FORMAT_R32G32B32_SFLOAT;                break;
                        case gfxp::TextureFormat::R32G32B32A32_UINT:            to = VK_FORMAT_R32G32B32A32_UINT;               break;
                        case gfxp::TextureFormat::R32G32B32A32_SINT:            to = VK_FORMAT_R32G32B32A32_SINT;               break;
                        case gfxp::TextureFormat::R32G32B32A32_SFLOAT:          to = VK_FORMAT_R32G32B32A32_SFLOAT;             break;
                        case gfxp::TextureFormat::R64_UINT:                     to = VK_FORMAT_R64_UINT;                        break;
                        case gfxp::TextureFormat::R64_SINT:                     to = VK_FORMAT_R64_SINT;                        break;
                        case gfxp::TextureFormat::R64_SFLOAT:                   to = VK_FORMAT_R64_SFLOAT;                      break;
                        case gfxp::TextureFormat::R64G64_UINT:                  to = VK_FORMAT_R64G64_UINT;                     break;
                        case gfxp::TextureFormat::R64G64_SINT:                  to = VK_FORMAT_R64G64_SINT;                     break;
                        case gfxp::TextureFormat::R64G64_SFLOAT:                to = VK_FORMAT_R64G64_SFLOAT;                   break;
                        case gfxp::TextureFormat::R64G64B64_UINT:               to = VK_FORMAT_R64G64B64_UINT;                  break;
                        case gfxp::TextureFormat::R64G64B64_SINT:               to = VK_FORMAT_R64G64B64_SINT;                  break;
                        case gfxp::TextureFormat::R64G64B64_SFLOAT:             to = VK_FORMAT_R64G64B64_SFLOAT;                break;
                        case gfxp::TextureFormat::R64G64B64A64_UINT:            to = VK_FORMAT_R64G64B64A64_UINT;               break;
                        case gfxp::TextureFormat::R64G64B64A64_SINT:            to = VK_FORMAT_R64G64B64A64_SINT;               break;
                        case gfxp::TextureFormat::R64G64B64A64_SFLOAT:          to = VK_FORMAT_R64G64B64A64_SFLOAT;             break;
                        case gfxp::TextureFormat::B10G11R11_UFLOAT_PACK32:      to = VK_FORMAT_B10G11R11_UFLOAT_PACK32;         break;
                        case gfxp::TextureFormat::E5B9G9R9_UFLOAT_PACK32:       to = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;          break;
                        case gfxp::TextureFormat::D16_UNORM:                    to = VK_FORMAT_D16_UNORM;                       break;
                        case gfxp::TextureFormat::X8_D24_UNORM_PACK32:          to = VK_FORMAT_X8_D24_UNORM_PACK32;             break;
                        case gfxp::TextureFormat::D32_SFLOAT:                   to = VK_FORMAT_D32_SFLOAT;                      break;
                        case gfxp::TextureFormat::S8_UINT:                      to = VK_FORMAT_S8_UINT;                         break;
                        case gfxp::TextureFormat::D16_UNORM_S8_UINT:            to = VK_FORMAT_D16_UNORM_S8_UINT;               break;
                        case gfxp::TextureFormat::D24_UNORM_S8_UINT:            to = VK_FORMAT_D24_UNORM_S8_UINT;               break;
                        case gfxp::TextureFormat::D32_SFLOAT_S8_UINT:           to = VK_FORMAT_D32_SFLOAT_S8_UINT;              break;
                        case gfxp::TextureFormat::BC1_RGB_UNORM_BLOCK:          to = VK_FORMAT_BC1_RGB_UNORM_BLOCK;             break;
                        case gfxp::TextureFormat::BC1_RGB_SRGB_BLOCK:           to = VK_FORMAT_BC1_RGB_SRGB_BLOCK;              break;
                        case gfxp::TextureFormat::BC1_RGBA_UNORM_BLOCK:         to = VK_FORMAT_BC1_RGBA_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::BC1_RGBA_SRGB_BLOCK:          to = VK_FORMAT_BC1_RGBA_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::BC2_UNORM_BLOCK:              to = VK_FORMAT_BC2_UNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC2_SRGB_BLOCK:               to = VK_FORMAT_BC2_SRGB_BLOCK;                  break;
                        case gfxp::TextureFormat::BC3_UNORM_BLOCK:              to = VK_FORMAT_BC3_UNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC3_SRGB_BLOCK:               to = VK_FORMAT_BC3_SRGB_BLOCK;                  break;
                        case gfxp::TextureFormat::BC4_UNORM_BLOCK:              to = VK_FORMAT_BC4_UNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC4_SNORM_BLOCK:              to = VK_FORMAT_BC4_SNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC5_UNORM_BLOCK:              to = VK_FORMAT_BC5_UNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC5_SNORM_BLOCK:              to = VK_FORMAT_BC5_SNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC6H_UFLOAT_BLOCK:            to = VK_FORMAT_BC6H_UFLOAT_BLOCK;               break;
                        case gfxp::TextureFormat::BC6H_SFLOAT_BLOCK:            to = VK_FORMAT_BC6H_SFLOAT_BLOCK;               break;
                        case gfxp::TextureFormat::BC7_UNORM_BLOCK:              to = VK_FORMAT_BC7_UNORM_BLOCK;                 break;
                        case gfxp::TextureFormat::BC7_SRGB_BLOCK:               to = VK_FORMAT_BC7_SRGB_BLOCK;                  break;
                        case gfxp::TextureFormat::ETC2_R8G8B8_UNORM_BLOCK:      to = VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;         break;
                        case gfxp::TextureFormat::ETC2_R8G8B8_SRGB_BLOCK:       to = VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK;          break;
                        case gfxp::TextureFormat::ETC2_R8G8B8A1_UNORM_BLOCK:    to = VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;       break;
                        case gfxp::TextureFormat::ETC2_R8G8B8A1_SRGB_BLOCK:     to = VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK;        break;
                        case gfxp::TextureFormat::ETC2_R8G8B8A8_UNORM_BLOCK:    to = VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;       break;
                        case gfxp::TextureFormat::ETC2_R8G8B8A8_SRGB_BLOCK:     to = VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK;        break;
                        case gfxp::TextureFormat::EAC_R11_UNORM_BLOCK:          to = VK_FORMAT_EAC_R11_UNORM_BLOCK;             break;
                        case gfxp::TextureFormat::EAC_R11_SNORM_BLOCK:          to = VK_FORMAT_EAC_R11_SNORM_BLOCK;             break;
                        case gfxp::TextureFormat::EAC_R11G11_UNORM_BLOCK:       to = VK_FORMAT_EAC_R11G11_UNORM_BLOCK;          break;
                        case gfxp::TextureFormat::EAC_R11G11_SNORM_BLOCK:       to = VK_FORMAT_EAC_R11G11_SNORM_BLOCK;          break;
                        case gfxp::TextureFormat::ASTC_4x4_UNORM_BLOCK:         to = VK_FORMAT_ASTC_4x4_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_4x4_SRGB_BLOCK:          to = VK_FORMAT_ASTC_4x4_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_5x4_UNORM_BLOCK:         to = VK_FORMAT_ASTC_5x4_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_5x4_SRGB_BLOCK:          to = VK_FORMAT_ASTC_5x4_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_5x5_UNORM_BLOCK:         to = VK_FORMAT_ASTC_5x5_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_5x5_SRGB_BLOCK:          to = VK_FORMAT_ASTC_5x5_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_6x5_UNORM_BLOCK:         to = VK_FORMAT_ASTC_6x5_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_6x5_SRGB_BLOCK:          to = VK_FORMAT_ASTC_6x5_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_6x6_UNORM_BLOCK:         to = VK_FORMAT_ASTC_6x6_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_6x6_SRGB_BLOCK:          to = VK_FORMAT_ASTC_6x6_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_8x5_UNORM_BLOCK:         to = VK_FORMAT_ASTC_8x5_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_8x5_SRGB_BLOCK:          to = VK_FORMAT_ASTC_8x5_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_8x6_UNORM_BLOCK:         to = VK_FORMAT_ASTC_8x6_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_8x6_SRGB_BLOCK:          to = VK_FORMAT_ASTC_8x6_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_8x8_UNORM_BLOCK:         to = VK_FORMAT_ASTC_8x8_UNORM_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_8x8_SRGB_BLOCK:          to = VK_FORMAT_ASTC_8x8_SRGB_BLOCK;             break;
                        case gfxp::TextureFormat::ASTC_10x5_UNORM_BLOCK:        to = VK_FORMAT_ASTC_10x5_UNORM_BLOCK;           break;
                        case gfxp::TextureFormat::ASTC_10x5_SRGB_BLOCK:         to = VK_FORMAT_ASTC_10x5_SRGB_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_10x6_UNORM_BLOCK:        to = VK_FORMAT_ASTC_10x6_UNORM_BLOCK;           break;
                        case gfxp::TextureFormat::ASTC_10x6_SRGB_BLOCK:         to = VK_FORMAT_ASTC_10x6_SRGB_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_10x8_UNORM_BLOCK:        to = VK_FORMAT_ASTC_10x8_UNORM_BLOCK;           break;
                        case gfxp::TextureFormat::ASTC_10x8_SRGB_BLOCK:         to = VK_FORMAT_ASTC_10x8_SRGB_BLOCK;            break;
                        case gfxp::TextureFormat::ASTC_10x10_UNORM_BLOCK:       to = VK_FORMAT_ASTC_10x10_UNORM_BLOCK;          break;
                        case gfxp::TextureFormat::ASTC_10x10_SRGB_BLOCK:        to = VK_FORMAT_ASTC_10x10_SRGB_BLOCK;           break;
                        case gfxp::TextureFormat::ASTC_12x10_UNORM_BLOCK:       to = VK_FORMAT_ASTC_12x10_UNORM_BLOCK;          break;
                        case gfxp::TextureFormat::ASTC_12x10_SRGB_BLOCK:        to = VK_FORMAT_ASTC_12x10_SRGB_BLOCK;           break;
                        case gfxp::TextureFormat::ASTC_12x12_UNORM_BLOCK:       to = VK_FORMAT_ASTC_12x12_UNORM_BLOCK;          break;
                        case gfxp::TextureFormat::ASTC_12x12_SRGB_BLOCK:        to = VK_FORMAT_ASTC_12x12_SRGB_BLOCK;           break;

                        default:                                                return false;                                   break;
                }

                return true;
        }


        /*!
         * @brief VulkanEnumTranslator::translate
         * Translate the given TextureSampleCount to a vulkan VkSampleCountFlags value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const gfxp::TextureSampleCount from, VkSampleCountFlags& to)
        {
                to = 0;

                switch(from)
                {
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_1:  to = VK_SAMPLE_COUNT_1_BIT;     break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_2:  to = VK_SAMPLE_COUNT_2_BIT;     break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_4:  to = VK_SAMPLE_COUNT_4_BIT;     break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_8:  to = VK_SAMPLE_COUNT_8_BIT;     break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_16: to = VK_SAMPLE_COUNT_16_BIT;    break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_32: to = VK_SAMPLE_COUNT_32_BIT;    break;
                        case gfxp::TextureSampleCount::SAMPLE_COUNT_64: to = VK_SAMPLE_COUNT_64_BIT;    break;

                        default:                                        return false;   break;
                }

                return true;
        }


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
         * Translate the given PrimitiveType to a vulkan VkPrimitiveTopology value
         * @param from Value to be translated
         * @param to Output variable in which the translated value will be written
         * @return True if the translation is successfull, false otherwise
        */
        bool VulkanEnumTranslator::translate(const PrimitiveType from, VkPrimitiveTopology& to)
        {
                switch(from)
                {
                        case PrimitiveType::POINT_LIST:         to = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;          break;
                        case PrimitiveType::LINE_LIST:          to = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;           break;
                        case PrimitiveType::LINE_STRIP:         to = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;          break;
                        case PrimitiveType::TRIANGLE_LIST:      to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;       break;
                        case PrimitiveType::TRIANGLE_STRIP:     to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;      break;
                        case PrimitiveType::TRIANGLE_FAN:       to = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;        break;

                        default:                                return false; break;
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

