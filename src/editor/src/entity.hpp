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

#include "observable.hpp"
#include <array> // std::array
#include <atomic>
#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <memory> // std::unique_ptr

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

private:
  static std::unique_ptr<component_deleter_base> deleters[max_components];
};

template <typename T> struct component_deleter : public component_deleter_base {
  void delete_component(void *component) override {
    delete static_cast<T *>(component);
  }
};

template <typename T>
component_deleter_base *component_deleter_base::add_deleter() {
  if (deleters[T::family()] == nullptr) {
    deleters[T::family()] =
        std::make_unique<component_deleter<T>>();
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

class entity : public entity_ref_count {
public:
  friend class scene;
  using ptr = boost::intrusive_ptr<entity>;

  entity(scene &s_) : scene_{&s_} {}

  ~entity() { remove_all_components(); }

  void destroy() { deleted_ = true; }

  void remove_all_components() {
    for (auto i = 0u; i < max_components; ++i)
      if (comp_[i] != nullptr)
        remove_component(i);
  }

  template <typename T> void remove_component() {
	  static_assert(std::is_base_of<component_counter_base, T>::value, "stuff");
    remove_component(T::family());
  }

  void remove_component(int family) {
    if (comp_[family]) {
      auto d = component_deleter_base::get_deleter(family);
      d->delete_component(comp_[family]);
      comp_[family] = nullptr;
    }
  }

  template <typename T> T *add_component() { return add_component(T{}); }

  template <typename T, typename Derived = std::decay_t<T> >
  Derived* add_component(T &&init)
  {
	  using namespace boost;
	  static_assert(std::is_base_of<component_counter_base, Derived>::value, "stuff");
	  using Base = typename Derived::component_type;
	  component_deleter_base::add_deleter<Base>();
	  remove_component<Base>();
	  // move-construct
	  auto ptr = new Derived{ std::forward<Derived>(init) };
	  comp_[Base::family()] = ptr;
	  // call initialize() method if the component has one
	  auto has_initialize =
		  hana::is_valid([](auto &&x) -> decltype(x.initialize(entity::ptr{})) {});
	  hana::if_(has_initialize(*ptr),
		  [this](auto &&p) { p.initialize(entity::ptr{ this }); },
		  [](auto &&p) {})(*ptr);
	  // call start() method if the component has one
	  auto has_start = hana::is_valid([](auto &&x) -> decltype(x.start()) {});
	  hana::if_(has_start(*ptr), [this](auto &&p) { p.start(); },
		  [](auto &&p) {})(*ptr);
	  return ptr;
  }

  template <typename T> T *get_component() const {
	  static_assert(std::is_base_of<component_counter_base, T>::value, "stuff");
    return static_cast<T *>(comp_[T::family()]);
  }

  auto &get_scene() { return *scene_; }

private:
  scene *scene_;
  std::array<void *, max_components> comp_{{nullptr}};
  bool deleted_ = false;
};

//
// Base class for scripting components
template <typename T>
struct behaviour : public component<T> {
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

#endif /* end of include guard: ENTITY_HPP */