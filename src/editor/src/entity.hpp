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

#include <algorithm> // std::remove_if
#include <atomic>
#include <array>     // std::array
#include <boost/hana.hpp>
#include <boost/hana/ext/std/tuple.hpp>
#include <boost/smart_ptr/intrusive_ptr.hpp>
#include <memory> // std::unique_ptr
#include <vector> // std::vector

constexpr auto max_components = 16;

struct component_counter_base {
protected:
  static unsigned family_counter;
};

template <typename T> struct component_counter : public component_counter_base {
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
    if (deleters[component_counter<T>::family()] == nullptr) {
      deleters[component_counter<T>::family()] =
          std::make_unique<component_deleter<T>>();
    }
    return deleters[component_counter<T>::family()].get();
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

  entity() = default;

  ~entity() { remove_all_components(); }

  void destroy() { deleted_ = true; }

  void remove_all_components() {
    for (auto i = 0u; i < max_components; ++i)
      if (comp_[i] != nullptr)
        remove_component(i);
  }

  template <typename T> void remove_component() {
    remove_component(component_counter<T>::family());
  }

  void remove_component(int family) {
    if (comp_[family]) {
      auto d = component_deleter_base::get_deleter(family);
      d->delete_component(comp_[family]);
      comp_[family] = nullptr;
    }
  }

  template <typename T> T *add_component() { return add_component(T{}); }

  template <typename T> T *add_component(T &&init) {
    component_deleter_base::add_deleter<T>();
    remove_component<T>();
    // move-construct
    auto ptr = new T{std::forward<T>(init)};
    comp_[component_counter<T>::family()] = ptr;
    return ptr;
  }

  template <typename T> T *get_component() const {
    return static_cast<T *>(comp_[component_counter<T>::family()]);
  }

private:
  std::array<void *, max_components> comp_{{nullptr}};
  bool deleted_ = false;
};

// a container for entities
class scene {
public:
  entity::ptr create_entity() {
    ent_.push_back(std::make_unique<entity>());
    return entity::ptr{ent_.back().get()};
  }

  template <typename... Components>
  entity::ptr create_entity(Components &&... comp) {
    auto p = create_entity();
    using namespace boost;
    hana::for_each(std::forward_as_tuple(comp...), [&p](auto &&c) {
      using CompT = std::decay_t<decltype(c)>;
      p->add_component<CompT>(std::forward<CompT>(c));
    });
    return p;
  }

  // collect garbage
  void collect() {
    ent_.erase(std::remove_if(ent_.begin(), ent_.end(), [](auto &up) {
      return up->deleted_ && (up->use_count() == 1);
    }));
  }

  size_t size() const { return ent_.size(); }

  template <typename T> auto find(bool include_deleted = false) const {
    /*using namespace ranges;
    // range-v3 is cool, but incredibly slow to compile
    return ent_ | view::remove_if([include_deleted](auto &&u) {
             return (!u->template get_component<T>()) ||
                    (!include_deleted && u->deleted_);
           }) |
           view::transform([](auto &&e) { return e.get(); });*/
    return entity_view{*this, component_counter<T>::family()};
  }

private:
  std::vector<std::unique_ptr<entity>> ent_;

  class entity_iterator {
  public:
    entity_iterator(const scene &s, unsigned family, size_t current = 0)
        : s_{s}, family_{family}, cur_{current} {
      skip();
    }

    entity_iterator &operator++() {
      ++cur_;
      skip();
	  return *this;
    }

    entity *operator*() { return s_.ent_[cur_].get(); }

	friend inline bool operator==(const scene::entity_iterator &it1,
		const scene::entity_iterator &it2) {
		return (it1.family_ == it2.family_) && (it1.cur_ == it2.cur_);
	}
	
	friend inline bool operator!=(const scene::entity_iterator &it1,
		const scene::entity_iterator &it2) {
		return !(it1 == it2);
	}

  private:
    void skip() {
      while (cur_ < s_.size() && s_.ent_[cur_]->comp_[family_] == nullptr)
        ++cur_;
    }


	const scene &s_;
	unsigned family_;
	size_t cur_;
  };

  struct entity_view {
    const scene &s_;
    unsigned family_;
    entity_view(const scene &scene, unsigned family) : s_{scene}, family_{family} {}
    entity_iterator begin() const { return entity_iterator{s_, family_, 0}; }
    entity_iterator end() const {
      return entity_iterator{s_, family_, s_.size()};
    }
  };
};



#endif /* end of include guard: ENTITY_HPP */