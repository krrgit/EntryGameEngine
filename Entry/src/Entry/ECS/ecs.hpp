#pragma once
#include <unordered_map>
#include <vector>
#include <cstdint>
#include <algorithm>
#include <typeindex>
#include <memory>

namespace Entry {

	namespace ECS
	{
		// -------- Entity --------
		using Entity = std::uint32_t;


		class Registry {
		private:
            Entity nextEntity = 1;
			std::vector <Entity> entities;
            std::size_t typeCounter = 0;

			struct IComponentStorage {
				virtual ~IComponentStorage() = default;
				virtual void remove(Entity e) = 0;
                virtual bool contains(Entity e) const = 0;
            };

			template<typename T>
			struct ComponentStorage : IComponentStorage {
				std::unordered_map<Entity, T> data;
				void remove(Entity e) override { data.erase(e); }
                bool contains(Entity e) const override { return data.find(e) != data.end(); }
            };

            // Unique type ID generator without RTTI
            template<typename T>
            std::size_t typeId() {
                static std::size_t id = typeCounter++;
                return id;
            }

			std::unordered_map<std::size_t, std::unique_ptr<IComponentStorage>> components;

        public:
            // --- Entities ---
            Entity create() {
                Entity e = nextEntity++;
                entities.push_back(e);
                return e;
            }

            void destroy(Entity e) {
                entities.erase(std::remove(entities.begin(), entities.end(), e), entities.end());
                // remove all components of this entity
                for (auto& pair : components) pair.second->remove(e);
            }

            // --- Components ---
            template<typename T, typename... Args>
            T& emplace(Entity e, Args&&... args) {
                auto type = typeId<T>();
                if (!components.count(type))
                    components[type] = std::unique_ptr<IComponentStorage>(new ComponentStorage<T>());
                auto storage = static_cast<ComponentStorage<T>*>(components[type].get());
                auto& component = storage->data[e] = T(std::forward<Args>(args)...);
                return component;
            }

            template<typename T>
            T& get(Entity e) {
                auto type = typeId<T>();
                auto storage = static_cast<ComponentStorage<T>*>(components[type].get());
                auto it = storage->data.find(e);
                if (!storage || it == storage->data.end())
                    ET_CORE_ERROR("Component not found for entity");
                return it->second;
            }

            template<typename T>
            void remove(Entity e) {
                auto type = typeId<T>();
                if (!components.count(type)) return;
                auto storage = static_cast<ComponentStorage<T>*>(components[type].get());
                storage->remove(e);
            }

            template<typename T>
            bool has(Entity e) {
                auto type = typeId<T>();
                return components.count(type);
            }

            // --- Views ---
            // Single-component view
            template<typename T>
            std::vector<Entity> view() {
                std::vector<Entity> result;
                auto type = typeId<T>();
                if (!components.count(type)) return result;
                auto storage = static_cast<ComponentStorage<T>*>(components[type].get());
                for (auto& pair : storage->data) result.push_back(pair.first);
                return result;
            }

            // Multi-component view
            template<typename T, typename... Ts>
            std::vector<Entity> view() {
                std::vector<Entity> result;

                // Find the smallest storage to iterate over
                ComponentStorage<T>* mainStorage = getStorage<T>();
                if (!mainStorage) return result;

                for (auto& pair : mainStorage->data) {
                    Entity e = pair.first;
                    if (hasComponents<Ts...>(e)) {
                        result.push_back(e);
                    }
                }

                return result;
            }
        private:

            // Helper: get storage
            template<typename T>
            ComponentStorage<T>* getStorage() {
                auto type = typeId<T>();
                if (!components.count(type)) return nullptr;
                return static_cast<ComponentStorage<T>*>(components[type].get());
            }

            // Helper: check if entity has all components
            template<typename T>
            bool hasComponents(Entity e) {
                auto storage = getStorage<T>();
                return storage && storage->contains(e);
            }

            template<typename T, typename U, typename... Ts>
            bool hasComponents(Entity e) {
                return hasComponents<T>(e) && hasComponents<U, Ts...>(e);
            }
		};
	}
}