
/**
 * @file handleManager.hpp
 * Declares the gfxp::internal::HandleManager class
*/

#ifndef GFXP_INTERNAL_HANDLE_MANAGER_H
#define GFXP_INTERNAL_HANDLE_MANAGER_H

#include <unordered_map>
#include <functional>
#include "handleTypes.hpp"

namespace gfxp::internal {


        /**
         * @class HandleManager
         * Manages a collection of generic objects and keeps track of their association to numeric handles;
         * such handles are computed and assigned by this class itself.
         * @param Object Type of objects managed by this class
         * @param HandleType Type of handle to be associated to Object instances
        */
        template <typename Object, typename HandleType>
        class HandleManager {
        public:
                                HandleManager() = default;
                                ~HandleManager() = default;

                HandleType      addObject(const Object& obj);
                bool            removeObject(const HandleType handle);
                bool            findObject(const HandleType handle, Object& obj);

                void            applyToAll(std::function<void (Object&)> func);

                void            reset();

        private:
                uint32_t                                m_lastHandleAssigned = VP_INVALID_HANDLE;       ///< Last handle value returned by this manager
                std::unordered_map<HandleType, Object>  m_objectsMap;                                   ///< Maps handle values to object instances
        };


        /**
         * @brief HandleManager::addObject
         * Inserts the given object into the collection managed by the handler and computes an handle for it
         * @param obj Object to be stored
         * @return An handle to uniquely identify the object
        */
        template <typename Object, typename HandleType>
        HandleType HandleManager<Object, HandleType>::addObject(const Object& obj)
        {
                ++m_lastHandleAssigned;
                m_objectMap.insert( { m_lastHandleAssigned, obj } );

                return static_cast<HandleType>(m_lastHandleAssigned);
        }


        /**
         * @brief HandleManager::removeObject
         * Removes the object associated to the given handle from the collection managed by the manager
         * @param handle Handle to the object that shall be removed
         * @return True if the object gets removed correctly, false otherwise
        */
        template <typename Object, typename HandleType>
        bool HandleManager<Object, HandleType>::removeObject(const HandleType handle)
        {
                auto it = m_objectsMap.find(handle);
                if(it == m_objectsMap.end())
                        return false;

                m_objectMap.remove(it);
                return true;
        }


        /**
         * @brief HandleManager::findObject
         * Searches the object associated to the given handle into the collection managed by the manager
         * @param handle Handle to the object that shall be retrieved
         * @param obj Output variable in which the object will be copied
         * @return True if the object if found and copied correctly, false otherwise
        */
        template <typename Object, typename HandleType>
        bool HandleManager<Object, HandleType>::findObject(const HandleType handle, Object& obj)
        {
                auto it = m_objectsMap.find(handle);
                if(it == m_objectsMap.end())
                        return false;

                obj = it->second;
                return true;
        }


        /**
         * @brief HandleManager::applyToAll
         * Invokes the given function on all objects stored in the collection managed by the manager
         * @warning The function shall not modify the manager itself!
        */
        template <typename Object, typename HandleType>
        void HandleManager<Object, HandleType>::applyToAll(std::function<void (Object&)> func)
        {
                for(auto& pair : m_objectsMap)
                        func(pair.second);
        }


        /**
         * @brief HandleManager::reset
         * Clears the collection of objects managed by the manager and resets the handle counter
         * (so that handles will restart from the beginning and will be reused)
        */
        template <typename Object, typename HandleType>
        void HandleManager<Object, HandleType>::reset()
        {
                m_objectsMap.clear();
                m_lastHandleAssigned = VP_INVALID_HANDLE;
        }


} // namespace gfxp::internal

#endif // GFXP_INTERNAL_HANDLE_MANAGER_H
