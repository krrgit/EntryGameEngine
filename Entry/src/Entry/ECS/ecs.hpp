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

        // special null entity value
        struct null_t {
            constexpr operator Entity() const { return invalid; }
            static constexpr Entity invalid = (Entity)(-1); // 0xFFFFFFFF
        };

        constexpr null_t null{};

        // Utility: index_sequence for C++11
        template<std::size_t... Ints>
        struct index_sequence
        {
            typedef index_sequence type;
        };

        template<std::size_t N, std::size_t... Ints>
        struct make_index_sequence : make_index_sequence<N - 1, N - 1, Ints...> {};

        template<std::size_t... Ints>
        struct make_index_sequence<0, Ints...> : index_sequence<Ints...> {};

        template<typename... Ts>
        using index_sequence_for = make_index_sequence<sizeof...(Ts)>;


		class Registry {
		private:
            Entity nextEntity = 0;
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

            template<typename Func>
            void each(Func func)
            {
                for (auto e : entities)
                {
                    func(e);
                }
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
            T* try_get(Entity e) {
                auto storage = getStorage<T>();
                if (!storage) return nullptr;
                auto it = storage->data.find(e);
                if (it != storage->data.end())
                    return &it->second;
                return nullptr;
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

            // --- View Class ---
            template<typename... Components>
            class View {
                Registry& registry;
                std::vector<Entity> entities;

            public:
                View(Registry& reg, std::vector<Entity> ents)
                    : registry(reg), entities(std::move(ents)) {}

                std::vector<Entity>::iterator begin() { return entities.begin(); }
                std::vector<Entity>::iterator end() { return entities.end(); }

                template<typename T>
                T& get(Entity e) {
                    return registry.get<T>(e);
                }

                template<typename T>
                T* try_get(Entity e) {
                    return registry.try_get<T>(e);
                }

                template<typename Func>
                void each(Func&& func)
                {
                    for (Entity e : entities)
                    {
                        callFunc(e, func, index_sequence_for<Components...>{});
                    }
                }
            private:
                template<typename Func, std::size_t... Is>
                void callFunc(Entity e, Func& func, index_sequence<Is...>)
                {
                    // Expand Components... pack, fetch each component, and call func
                    func(e, registry.get<Components>(e)...);
                }
            };

            // --- Create a View ---
            template<typename T, typename... Ts>
            View<T, Ts...> view() {
                std::vector<Entity> result;

                auto mainStorage = getStorage<T>();
                if (!mainStorage) return View<T, Ts...>(*this, result);

                for (auto& pair : mainStorage->data) {
                    Entity e = pair.first;
                    if (hasComponents<Ts...>(e))
                        result.push_back(e);
                }

                return View<T, Ts...>(*this, result);
            }

        private:

            // Helper: get storage
            template<typename T>
            ComponentStorage<T>* getStorage() {
                auto type = typeId<T>();
                if (!components.count(type)) return nullptr;
                return static_cast<ComponentStorage<T>*>(components[type].get());
            }

            // Base case: no components to check
            template<typename... Ts>
            typename std::enable_if<sizeof...(Ts) == 0, bool>::type
            hasComponents(Entity) {
                return true;
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