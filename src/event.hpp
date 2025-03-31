#pragma once

#include <any>
#include <functional>
#include <string>
#include <unordered_map>

#include "util/utils.hpp"

#define BIT(x) (1 << (x))

namespace hep {
// https://github.com/TheCherno/Hazel/blob/1feb70572fa87fa1c4ba784a2cfeada5b4a500db/Hazel/src/Hazel/Events/Event.h

enum EventCategoryFlags : u32 {
  None = 0,
  EventCategoryApplication = BIT(0),
  EventCategoryInput = BIT(1),
  EventCategoryKeyboard = BIT(2),
  EventCategoryMouse = BIT(3),
  EventCategoryMouseButton = BIT(4)
};

class Event {
 public:
  virtual ~Event() = default;

  virtual const char* getName() const = 0;
  virtual std::string toString() const { return getName(); }

  virtual int getCategoryFlags() const { return EventCategoryFlags::None; };

  inline bool isInCategory(EventCategoryFlags category) {
    return getCategoryFlags() & category;
  }
};

template <typename DerivedEvent>
class EventDelegate : public Delegate<DerivedEvent&> {
 public:
  EventDelegate() {
    static_assert(std::is_base_of<Event, DerivedEvent>::value,
                  "DerivedEvent must derive from Event");
  }
};

inline std::ostream& operator<<(std::ostream& os, const Event& e) {
  return os << e.toString();
}

/**
 * Singleton class for managing event subscription and dispatching.
 *
 * TODO heavy documentation is needed
 *
 * @todo impl removing functors from delegate
 * @warning this class is in heavy development, use at your own risk
 * @warning NOT THREAD SAFE
 */
class EventSystem {
 public:
  using EventType = decltype(std::declval<std::type_info>().hash_code());

#define GET_EVENT_TYPE(E) typeid(E).hash_code()

  EventSystem(const EventSystem&) = delete;
  EventSystem& operator=(const EventSystem&) = delete;

  static EventSystem& get() {
    static EventSystem instance;
    return instance;
  }

  template <class E>
  void addListener(std::function<void(E&)> f) {
    static_assert(std::is_base_of<Event, E>::value, "E must derive from Event");

    EventType eventType = GET_EVENT_TYPE(E);

    // Ensures pointer to EventDelegate<E> is not null in unordered map
    if (!this->eventDelegates.count(eventType)) {
      this->eventDelegates[eventType] =
          std::make_unique<std::any>(EventDelegate<E>());

      log::trace("EventSystem creating new event type: ", eventType);
    }

    EventDelegate<E>& delegate =
        std::any_cast<EventDelegate<E>&>(*eventDelegates[eventType]);
    delegate.add(f);
  }

  template <class E, class O>
  void addMethodListener(void (O::*memberFn)(E&), std::shared_ptr<O> obj) {
    static_assert(std::is_base_of<Event, E>::value, "E must derive from Event");

    EventType eventType = GET_EVENT_TYPE(E);

    // Ensures pointer to EventDelegate<E> is not null in unordered map
    if (!this->eventDelegates.count(eventType)) {
      this->eventDelegates[eventType] =
          std::make_unique<std::any>(EventDelegate<E>());

      log::trace("EventSystem creating new event type: ", eventType);
    }

    EventDelegate<E>& delegate =
        std::any_cast<EventDelegate<E>&>(*eventDelegates[eventType]);
    delegate.addMethod(memberFn, obj);
  }

  template <class E>
  void dispatch(E& event) {
    static_assert(std::is_base_of<Event, E>::value, "E must derive from Event");
    EventType eventType = GET_EVENT_TYPE(E);

    if (!this->eventDelegates.count(eventType)) { return; }

    try {
      EventDelegate<E>& delegate =
          std::any_cast<EventDelegate<E>&>(*this->eventDelegates.at(eventType));
      delegate.invoke(event);
    } catch (const std::out_of_range& error) {
      log::error("event dispatch failed: ", event.toString());
    }
  }

 private:
  EventSystem() { log::info("EventSystem Initialized: ", this); }

  std::unordered_map<EventType, std::unique_ptr<std::any>> eventDelegates;
};

}  // namespace hep
