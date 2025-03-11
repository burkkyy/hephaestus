#pragma once

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>

#include "util/logger.hpp"

namespace hep {

class expired_weak_object : public std::runtime_error {
 public:
  explicit expired_weak_object(const std::string& message)
      : std::runtime_error(message) {}
};

/**
 * Creates a weak function that safely calls a method if the
 * object is alive.
 *
 * @tparam O Class owning the method
 * @tparam A Variadic template parameters for the method arguments
 *
 * @param method Pointer to the static method to be called
 * @param obj Shared pointer to the object instance
 * @returns std::function that calls method on obj if alive, or throws if
 * expired
 * @throws std::invalid_argument If obj is null.
 * @throws expired_weak_object If the object has expired when called
 *
 * @note Could investigate removing throwing exceptions, and instead return a
 * bool if the weak pointer is alive. This may improve performance
 */
template <class O, typename... A>
std::function<void(A...)> createWeakFunction(void (O::*method)(A...),
                                             std::shared_ptr<O> obj) {
  if (!obj) { throw std::invalid_argument("method and obj must not be null"); }

  std::weak_ptr<O> weakObj = obj;

  return std::function<void(A...)>([method = std::move(method),
                                    weakObj = std::move(weakObj)](A... args) {
    if (auto lockedObj = weakObj.lock()) {
      (lockedObj.get()->*method)(std::forward<A>(args)...);
    } else {
      throw expired_weak_object("Attempting to call function of a dead class");
    }
  });
}

template <typename... A>
class Delegate {
 public:
  using Functor = std::function<void(A...)>;

  Delegate() { this->functors.reserve(10); }

  void add(const Functor& func) { functors.push_back(func); }

  template <class O>
  void addMethod(void (O::*memberFn)(A...), std::shared_ptr<O> obj) {
    functors.push_back(createWeakFunction(memberFn, obj));
  }

  void clear() { this->functors.clear(); }

  void invoke(A... args) {
    if (this->functors.empty()) {
      log::warning("invoking empty delegate");
      return;
    }

    std::vector<size_t> deadFunctorIndexs;

    for (size_t i = 0; i < functors.size(); i++) {
      if (this->functors[i]) {
        try {
          this->functors[i](std::forward<A>(args)...);
        } catch (const expired_weak_object& e) {
          log::error("delegate invoke error: " << e.what());
          deadFunctorIndexs.push_back(i);
        }
      }
    }

    for (auto it = deadFunctorIndexs.rbegin(); it != deadFunctorIndexs.rend();
         ++it) {
      functors.erase(functors.begin() + *it);
    }
  }

  void operator=(Functor c) {
    clear();

    if (c != nullptr) { add(c); }
  }

  void operator+=(Functor c) { add(c); }

  void operator()() { invoke(); }

 private:
  std::vector<Functor> functors;
};

}  // namespace hep
