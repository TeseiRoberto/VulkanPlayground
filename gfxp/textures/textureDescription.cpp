
/**
 * @file shaderDescription.cpp
 * Implements the gfxp::TextureDescription class
*/

#include "textureDescription.hpp"

namespace gfxp {


        /**
         * @brief TextureDescription::setWidth
         * Sets the width of the texture
         * @param width Width to be used for the texture, expressed in pixels
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setWidth(uint32_t width)
        {
                m_width = width;
                return *this;
        }


        /**
         * @brief TextureDescription::setHeight
         * Sets the height of the texture
         * @param height Height to be used for the texture, expressed in pixels
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setHeight(uint32_t height)
        {
                m_height = height;
                return *this;
        }


        /**
         * @brief TextureDescription::setDepth
         * Sets the depth value of the texture, such value is required to create a 3D texture
         * @param depth Depth value for the texture
         * @return Reference to this description class instance, so that calls to methods can be concatenated
         * @note This method will fail if the texture type is not TEXTURE_3D
        */
        TextureDescription& TextureDescription::setDepth(uint32_t depth)
        {
                if(m_type != TextureType::TEXTURE_3D)
                {
                        LOG_ERROR("TextureDescription::setDepth() failed: texture type is not TEXTURE_3D!");
                        return *this;
                }

                m_depth = depth;
                return *this;
        }


        /**
         * @brief TextureDescription::setSize
         * Sets the size of the texture
         * @param width @param height @param depth Size of the texture
         * @return Reference to this description class instance, so that calls to methods can be concatenated
         * @note The depth value will be ignored if the texture type is not TEXTURE_3D
        */
        TextureDescription& TextureDescription::setSize(uint32_t width, uint32_t height, uint32_t depth)
        {
                m_width = width;
                m_height = height;

                if(m_type == TextureType::TEXTURE_3D)
                        m_depth = depth;

                return *this;
        }


        /**
         * @brief TextureDescription::setMipLevels
         * Sets the number of mip-maps that must be generated for the texture
         * @param levelsNum Number of mip-maps to be generated for the texture
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setMipLevels(uint32_t levelsNum)
        {
                m_levelsNum = levelsNum;
                return *this;
        }


        /**
         * @brief TextureDescription::setLayersNum
         * Sets the number of layers for the texture, such value is required to create a texture array
         * @param layersNum Number of layers for the texture
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setLayersNum(uint32_t layersNum)
        {
                m_layersNum = layersNum;
                return *this;
        }


        /**
         * @brief TextureDescription::setType
         * Sets the type of the texture
         * @param type Type of texture
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setType(TextureType type)
        {
                m_type = type;

                // Reset depth value if the texture is not 3D
                if(m_type != TextureType::TEXTURE_3D)
                        m_depth = 0;

                return *this;
        }


        /**
         * @brief TextureDescription::setUsage
         * Sets the ways in which the texture will be used
         * @param usage Bitmask of flags which describes how the texture will be used
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setUsage(TextureUsageFlags usage)
        {
                m_usage = usage;
                return *this;
        }


        /**
         * @brief TextureDescription::setFormat
         * Sets the format of the texture
         * @param format Format that the pixels of the texture must have
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setFormat(TextureFormat format)
        {
                m_format = format;
                return *this;
        }


        /**
         * @brief TextureDescription::setSampleCount
         * Sets the number of samples that must be used when sampling the texture
         * @param sampleCount Number of samples that must be used
         * @return Reference to this description class instance, so that calls to methods can be concatenated
        */
        TextureDescription& TextureDescription::setSampleCount(TextureSampleCount sampleCount)
        {
                m_sampleCount = sampleCount;
                return *this;
        }


        /**
         * @brief TextureDescription::getSize
         * @param width Output variable in which the width of the texture will be stored
         * @param height Output variable in which the height of the texture will be stored
         * @param depth Output variable in which the depth of the texture will be stored
        */
        void TextureDescription::getSize(uint32_t& width, uint32_t& height, uint32_t& depth) const
        {
                width   = m_width;
                height  = m_height;
                depth   = m_depth;
        }


        /**
         * @brief TextureDescription::reset
         * Resets all the fields of the description class instance to default values
        */
        void TextureDescription::reset()
        {
                m_width = 0;
                m_height = 0;
                m_depth = 0;

                m_mipLevels = 0;
                m_layersNum = 1;

                m_type = TextureType::UNKNOWN;
                m_usage = TextureUsage::UNKNOWN;
                m_format = TextureFormat::UNKNOWN;
                m_sampleCount = TextureSampleCount::SAMPLE_COUNT_1;
        }


} // namespace gfxp
