#ifndef ENTITY_HPP
#define ENTITY_HPP

// Yet another entity system
// inspired by https://github.com/alecthomas/entityx

// Goals: fast creation/deletion of entities and components
// fast retrieval of components through an entity pointer
// type safety
// deletion safety: a deleted entity should not be reclaimed until all
// references to
// the entity are released
// reference-counted

#include <array> // std::array
#include <atomic>	// std::atomic_int
#include <string>
#include <memory> // std::unique_ptr
#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp> 
#include "observable.hpp"
#include "reflect.hpp"

constexpr auto max_components = 16;

struct component_counter_base {
protected:
  static unsigned family_counter;
};

template <typename T> struct component : public component_counter_base {
  using component_type = T;
  static unsigned family() {
    static unsigned family_index = family_counter++;
    return family_index;
  }
};

class component_deleter_base {
public:
  virtual void delete_component(void *component) = 0;

  template <typename T> static component_deleter_base *add_deleter();

  static component_deleter_base *get_deleter(int family) {
    return deleters[family].get();
  }

  std::type_index typeindex;

private:
  static std::unique_ptr<component_deleter_base> deleters[max_components];
};

template <typename T> struct component_deleter : public component_deleter_base {
	component_deleter(std::type_index ti) : component_deleter_base{ ti }
	{}

  void delete_component(void *component) override {
    delete static_cast<T *>(component);
  }
};

template <typename T>
component_deleter_base *component_deleter_base::add_deleter() {
  if (deleters[T::family()] == nullptr) {
    deleters[T::family()] = std::make_unique<component_deleter<T>>(typeid(T));
  }
  return deleters[T::family()].get();
}

class entity_ref_count {
public:
  entity_ref_count() = default;
  int use_count() const { return use_count_; };

  friend void intrusive_ptr_add_ref(const entity_ref_count *p) noexcept {
    ++p->use_count_;
  }

  friend void intrusive_ptr_release(const entity_ref_count *p) noexcept {
    --p->use_count_;
    // do not delete here! cleanup is done in the game loop regularly
  }

private:
  mutable std::atomic_int use_count_{0};
};

template <typename T> class behaviour;
class script_base;

// Iterate on components:
//	 for (auto c : e->components())
// Iterate on components w/ associated type_index:
//   for (auto c : e->components_with_types())
// Issue: currently scripts are special cases
class REFLECT entity : public entity_ref_count {
public:
  friend class scene;
  using ptr = boost::intrusive_ptr<entity>;

  entity(scene &s_, std::string name_);
  ~entity();
  void destroy();
  template <typename T> T *get_component();
  template <typename T> void remove_component();
  void remove_all_components();
  void remove_component(int family);
  template <typename T, typename... Args> T *add_component(Args &&... args);
  template <typename T, typename... Args> T *add_script(Args &&... args);
  template <typename T> T *get_component() const;
  scene &get_scene();
  bool is_deleted() const;
  // entity name
  std::string name;

private:
  scene *scene_;
  // component storage
  std::array<void *, max_components> comp_{{nullptr}};
  // scripts
  std::vector<std::unique_ptr<script_base>> scripts_;
  bool deleted_ = false;
};

//======================================================
// Base class for scripting components
template <typename T> class behaviour : public component<T> {
public:
  template <typename T, typename F> void connect(observable<T> &obs, F &&fn) {
    obs.subscribe(sub, std::move(fn));
  }

  // Called by entity::add_component. Not meant to be called manually or
  // overriden.
  void initialize(entity::ptr ent_) { ent = std::move(ent_); }

  template <typename T, typename U, typename Derived>
  void connect_member(observable<T> &obs, void (Derived::*fn)(U)) {
    obs.subscribe(sub, [this, fn](T &&t) {
      (static_cast<Derived *>(this)->*fn)(std::move(t));
    });
  }

  template <typename T, typename Derived>
  void connect_member(observable<T> &obs, void (Derived::*fn)()) {
    obs.subscribe(sub, [this, fn]() { (static_cast<Derived *>(this)->*fn)(); });
  }

  auto &get_scene() { return ent->get_scene(); }
  template <typename T> T *get_component() { return ent->get_component<T>(); }

  entity::ptr ent;
  subscription sub;
};

//======================================================
// same as above, but with a virtual destructor
class script_base 
{
public:
  virtual ~script_base()
  {}
};

class script : public script_base, public behaviour<script> {
public:
  virtual ~script() 
  {}
};


//======================================================
// entity implementation
inline entity::entity(scene &s_, std::string name_) : name{ std::move(name_) }, scene_{ &s_ } {}

inline entity::~entity() { remove_all_components(); }

inline void entity::destroy() { deleted_ = true; }

template <typename T> T *entity::get_component() {
  static_assert(std::is_base_of<component<T>, T>::value,
                "Component type T must derive from component<T>");
  return static_cast<T *>(comp_[T::family()]);
}

template <typename T> void entity::remove_component() {
  static_assert(std::is_base_of<component<T>, T>::value,
                "Component type T must derive from component<T>");
  remove_component(T::family());
}

inline void entity::remove_all_components() {
  for (auto i = 0u; i < max_components; ++i)
    if (comp_[i] != nullptr)
      remove_component(i);
}

inline void entity::remove_component(int family) {
  if (comp_[family]) {
    auto d = component_deleter_base::get_deleter(family);
    d->delete_component(comp_[family]);
    comp_[family] = nullptr;
  }
}

template <typename T, typename... Args>
T *entity::add_component(Args &&... args) {
  static_assert(std::is_base_of<component_counter_base, T>::value,
                "Component type T must derive from component_counter_base");
  using Base = typename T::component_type;
  component_deleter_base::add_deleter<Base>();
  remove_component<Base>();
  auto ptr = new T{std::forward<Args>(args)...};
  comp_[Base::family()] = ptr;
  // call initialize() method if the component has one
  using namespace boost;
  auto has_initialize =
      hana::is_valid([](auto &&x) -> decltype(x.initialize(entity::ptr{})) {});
  hana::if_(has_initialize(*ptr),
            [this](auto &&p) { p.initialize(entity::ptr{ this }); },
            [](auto &&p) {})(*ptr);
  return ptr;
}

template <typename T, typename... Args> T *entity::add_script(Args &&... args) {
  static_assert(std::is_base_of<script_base, T>::value,
                "Script type T must derive from script_base");
  //
  scripts_.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
  auto ptr = scripts_.back().get();
  // call initialize() method if the component has one
  using namespace boost;
  auto has_initialize =
      hana::is_valid([](auto &&x) -> decltype(x.initialize(entity::ptr{})) {});
  hana::if_(has_initialize(*ptr),
            [this](auto &&p) { p.initialize(entity::ptr{this}); },
            [](auto &&p) {})(*ptr);
  return static_cast<T*>(ptr);
}

template <typename T> T *entity::get_component() const {
  static_assert(std::is_base_of<component<T>, T>::value,
                "Component type T must derive from component<T>");
  return static_cast<T *>(comp_[T::family()]);
}

inline scene &entity::get_scene() { return *scene_; }

inline bool entity::is_deleted() const { return deleted_; }

#endif /* end of include guard: ENTITY_HPP */