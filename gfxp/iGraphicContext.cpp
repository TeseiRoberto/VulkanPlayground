
/**
 * @file iGraphicContext.hpp
 * Implements the gfxp::IGraphicContext::create static method
*/

#include <cstdlib>
#include "iGraphicContext.hpp"
#include "internals/backend/vulkan/vulkanContext.hpp"

namespace gfxp {


        /**
         * @brief IGraphicContext::create
         * @param backendApi Graphics API to be used as backend
         * @return An uninitialized, API specific, graphic context instance
        */
        std::unique_ptr<IGraphicContext> IGraphicContext::create(const GraphicApi backendApi)
        {
                IGraphicContext* context = nullptr;

                switch(backendApi)
                {
                        case GraphicApi::VULKAN:
                                context = new gfxp::backend::VulkanContext();
                                break;

                        default:
                                // This shall never happen!
                                LOG_ERROR("IGraphicContext::create() failed: an invalid backendApi is specified!");
                                std::exit(-1);
                                break;
                }

                return std::unique_ptr<IGraphicContext>(context);
        }


} // namespace gfxp
