
/**
 * @file gpuResourceHandleManager.hpp
 * Declares the gfxp::internal::GpuResourceHandleManager class
*/

#ifndef GFXP_INTERNAL_GPU_RESOURCE_HANDLE_MANAGER_H
#define GFXP_INTERNAL_GPU_RESOURCE_HANDLE_MANAGER_H

#include <mutex>
#include <vector>
#include <algorithm>
#include <functional>

namespace gfxp::internal {


        /**
         * @class GpuResourceHandleManager
         * Manages a collection of handles (pointers) to generic GPU resources in a thread safe manner.
         * @param HandleType Type of handle to GPU resources that must be managed
         * @warning This class is not responsible for management of the resource lifetime (allocation and free)
        */
        template <typename HandleType>
        class GpuResourceHandleManager {
        public:
                                GpuResourceHandleManager() = default;
                                ~GpuResourceHandleManager() = default;

                void            add(const HandleType handle);
                bool            remove(const HandleType handle);
                bool            contains(const HandleType handle) const;

                void            applyToAll(std::function<void (HandleType&)> func);

                void            clear();

        private:
                std::mutex                      m_handlesMutex; ///< Mutex used to synch accesses to m_handles
                std::vector<HandleType>         m_handles;      ///< Pointers to GPU resorces instances
        };


        /**
         * @brief GpuResourceHandleManager::add
         * Inserts the given handle into the list of known handles
         * @param handle Handle to be added
        */
        template <typename HandleType>
        void GpuResourceHandleManager<HandleType>::add(const HandleType handle)
        {
                m_handlesMutex.lock();

                m_handles.push_back(handle);

                m_handlesMutex.unlock();
        }


        /**
         * @brief GpuResourceHandleManager::remove
         * Removes the given handle from the list of known handles
         * @param handle Handle to be removed
         * @return True if the handle is removed correctly, false otherwise
        */
        template <typename HandleType>
        bool GpuResourceHandleManager<HandleType>::remove(const HandleType handle)
        {
                std::lock_guard<std::mutex> lockGuard( m_handlesMutex );

                auto it = std::find(m_handles.begin(), m_handles.end(), handle);
                if(it == m_handles.end())
                        return false;

                m_handles.erase(it);
                return true;
        }


        /**
         * @brief GpuResourceHandleManager::contains
         * Checks if the given handle is stored in the list of known handles
         * @param handle Handle to be checked
         * @return True if the handle is known, false otherwise
        */
        template <typename HandleType>
        bool GpuResourceHandleManager<HandleType>::contains(const HandleType handle) const
        {
                std::lock_guard<std::mutex> lockGuard( m_handlesMutex );

                auto it = std::find(m_handles.begin(), m_handles.end(), handle);
                return (it != m_handles.end() );
        }


        /**
         * @brief GpuResourceHandleManager::applyToAll
         * Invokes the given function on all handles stored in the list of known handles
         * @warning The function must not modify the manager itself!
        */
        template <typename HandleType>
        void GpuResourceHandleManager<HandleType>::applyToAll(std::function<void (HandleType&)> func)
        {
                std::lock_guard<std::mutex> lockGuard( m_handlesMutex );

                for(auto& handle : m_handles)
                        func(handle);
        }


        /**
         * @brief GpuResourceHandleManager::clear
         * Clears the list of known handles
        */
        template <typename HandleType>
        void GpuResourceHandleManager<HandleType>::clear()
        {
                std::lock_guard<std::mutex> lockGuard( m_handlesMutex );
                m_handles.clear();
        }


} // namespace gfxp::internal

#endif // GFXP_INTERNAL_GPU_RESOURCE_HANDLE_MANAGER_H
