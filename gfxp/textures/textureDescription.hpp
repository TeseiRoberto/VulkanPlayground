
/**
 * @file shaderDescription.hpp
 * Declares the gfxp::TextureDescription class
*/

#ifndef GFXP_TEXTURE_DESCRIPTION_H
#define GFXP_TEXTURE_DESCRIPTION_H

#include <cstdint>
#include <vector>
#include <string>
#include <filesystem>

#include "gfxpLog.hpp"
#include "gfxpEnums.hpp"
#include "gfxpHandleTypes.hpp"

namespace gfxp {


        /**
         * @class TextureDescription
         * Class used to describe the properties of a texture GPU resource
        */
        class TextureDescription {
        public:
                                                TextureDescription();
                                                ~TextureDescription() = default;

                TextureDescription&             setWidth(uint32_t width);
                TextureDescription&             setHeight(uint32_t height);
                TextureDescription&             setDepth(uint32_t depth);

                TextureDescription&             setSize(uint32_t width, uint32_t height, uint32_t depth = 0);

                TextureDescription&             setMipLevels(uint32_t levelsNum);
                TextureDescription&             setLayersNum(uint32_t layersNum);
                TextureDescription&             setType(TextureType type);
                TextureDescription&             setUsage(TextureUsageFlags usage);
                TextureDescription&             setFormat(TextureFormat format);
                TextureDescription&             setSampleCount(TextureSampleCount sampleCount);

                inline uint32_t                 getWidth() const                        { return m_width; }
                inline uint32_t                 geHeight() const                        { return m_height; }
                inline uint32_t                 getDepth() const                        { return m_depth; }

                void                            getSize(uint32_t& width, uint32_t& height, uint32_t& depth) const;

                inline uint32_t                 getMipLevels() const                    { return m_mipLevels; }
                inline uint32_t                 getLayersNum() const                    { return m_layersNum; }
                inline TextureType              getType() const                         { return m_type; }
                inline TetxureUsageFlags        getUsage() const                        { return m_usage; }
                inline TextureFormat            getFormat() const                       { return m_format; }
                inline TextureSampleCount       getSampleCount() const                  { return m_sampleCount; }

                void                            reset();

        private:
                uint32_t                m_width = 0;                                            ///< Width of the texture, expressed in pixels
                uint32_t                m_height = 0;                                           ///< Height of the texture, expressed in pixels
                uint32_t                m_depth = 0;                                            ///< Depth of the texture, expressed in pixels, used to create 3D textures

                uint32_t                m_mipLevels = 0;                                        ///< Number of mip-map levels to be created for the texture
                uint32_t                m_layersNum = 1;                                        ///< Number of layers for the texture

                TextureType             m_type = TextureType::UNKNOWN;                          ///< Type of the texture
                TextureUsageFlags       m_usage = TextureUsage::UNKNOWN;                        ///< Ways in which the texture will be used
                TextureFormat           m_format = TextureFormat::UNKNOWN;                      ///< Format of the pixels that will compose the texture
                TextureSampleCount      m_sampleCount = TextureSampleCount::SAMPLE_COUNT_1;     ///< Number of samples for each pixel of the texture (used only if the texture is sampled)
        };


} // namespace gfxp

#endif // GFXP_TEXTURE_DESCRIPTION_H
