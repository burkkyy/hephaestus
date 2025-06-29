# Documentation

Most of info here will be for quick look up while developing, or notes to reference to get a quick refresher.

# 1. Disclaimer

The project is in a very early dev phase, so a good amount of the documentation will be out of date.

# 2. Event System

The event system is a flexible and type safe mechanism for managing and dispatching events within your a project. It allows functions to register as listeners (alias for callbacks) for specific events.

The system is designed to be extensible, enabling users to define custom events without needing to recompile the entire engine, which would require downloading build dependencies etc.

> As of Mar 12 2025 the project does not build to a shared library, project is in early dev phase so mentioned include paths may also be incorrect

## 2.1 Overiew

The engine provides a list of events you can create and dispatch.

> builtin events found in `events/`, when I get to writing engine api doc include list of builin events?

The `EventSystem` is a [singleton class](https://www.geeksforgeeks.org/singleton-class-java/) `hep::EventSystem` found in `#include "events/event.hpp`.

```cpp
class EventSystem {
  /**
   * @brief Retrieves singleton instance
   * @return ref to singleton
   */
  static EventSystem& get();

  /**
   * @brief Adds a function as a listener for events of type E
   * @tparam E Event type to listen for; must derive from hep::Event
   * @param f listener function for event E
   */
  template <class E>
  void addListener(std::function<void(E&)> f);

  /**
   * @brief Adds a member function from class O as a listener for events of type E
   * @tparam E Event type to listen for; must derive from hep::Event
   * @tparam O class defining memberFn (ignore since implicitly compiled)
   * @param memberFn pointer to a member function of O taking an E&
   * @param obj shared pointer to the instance of O owning the member function
   * @note works but experimental feature
   */
  template <class E, class O> // ignore O, its compiler implicit
  void addMethodListener(void (O::*memberFn)(E&), std::shared_ptr<O> obj);

  /**
   * @brief dispatches an event of type E to all registered listeners.
   * @tparam E Event type to dispatch; must derive from hep::Event
   * @param event reference to the event that will be passed to all listeners
   */
  template <class E>
  void dispatch(E& event);
};
```

## 2.2 Usage

For a usage example I will use the builtin event `KeyPressEvent`. Given by the name, if we add a listener to this event we can call a print function any time we press a key:

```cpp
#include "events/key_event.hpp"

void onKeyPressEvent(KeyPressedEvent& event){
  std::cout << "some key was pressed" << std::endl;

  if(event.getKeyCode() == Key::Space){
    std::cout << "space bar was pressed" << std::endl;
  }
}

EventSystem::addListener<KeyPressedEvent>(onKeyPressEvent);
```

> **All listener to an event must be of the signature:** `void(EventName&)`

## 2.3 Custom user defined events

Lets go through how you would create your own event.

For the sake of an example, lets say a user wanted to define an event for when a player dies in their game and we want to capture the players name and the level they died on.

This event can be defined by creating a class that inherits from `Event`. Lets call this event `OnPlayerDeathEvent`.

To pass our the player name, floor level etc onto the observers of `OnPlayerDeathEvent`, we can define these as attributes in `OnPlayerDeathEvent`.

```cpp
#include "events/event.hpp"

class OnPlayerDeathEvent : public Event {
 public:
  OnPlayerDeathEvent(std::string playerName, int floorLevel)
      : playerName{playerName}, floorLevel{floorLevel} {}

  std::string getPlayerName() const { return this->playerName; }
  int getFloorLevel() const { return this->floorLevel; }

 private:
  std::string playerName;
  int floorLevel;
};
```

> **Attributes should be private.** This is not required, but recommend as there could be multiple listeners to this event, so these attributes not being private means their values would be different for each listener.

With our `OnPlayerDeathEvent` event defined, we can create listeners to it and dispatch this event:

```cpp
void onPlayerDeath(OnPlayerDeathEvent& event){
  std::string name = event.getPlayerName();
  int level = event.getPlayerLevel();

  std::cout << "Player " << name << " died on level " << level << std::endl;
}

EventSystem::get().addListener<OnPlayerDeathEvent>(onPlayerDeath);

OnPlayerDeathEvent event{"Example", 54};
EventSystem::get().dispatch(event); // listeners of OnPlayerDeathEvent get called
```

## 2.4 Experimental Features

### 2.4.1 Adding memory safe member functions as listeners

Currently this "works" but could be improved.

Current example usage:

```cpp
class Foo {
public:
  Foo(std::string name) : name{name} {}

  void OnEvent(SomeEvent& event){
    std::cout << "Hello from " << this->name << std::endl;
  }

private:
  std::string name;
};

auto foo1 = std::make_shared<Foo>("foo1");
auto foo2 = std::make_shared<Foo>("foo2");

// adding OnEvent method of each instance to event system
EventSystem::get().addMethodListener<SomeEvent>(&Foo::OnEvent, foo1);
EventSystem::get().addMethodListener<SomeEvent>(&Foo::OnEvent, foo2);

// dispatch SomeEvent
SomeEvent event;
EventSystem::get().dispatch(event);

// delete foo1
foo1.reset();

// dispatch SomeEvent again
EventSystem::get().dispatch(event);
```

Example output:

```
Hello from foo1
Hello from foo2
Hello from foo2
```

Notice after freeing `foo1` the dispatch didnt cause any errors, this is because in the background a weakptr is created for `foo1` and if `foo1`'s shared pointer is reset/freed then the observer listener (`Foo::OnEvent`) is safely removed from the event system.

Current limitation is the inability to add a member function within your class using `addMethodListener`. Current work around is just using `std::bind()` with `addListener`.

Investigate some method of safely adding memeber functions within your own class.

## 2.5 Notes / Design choices

- Event system loosey follows an Observer design pattern

## 2.6 Other resources

- https://www.geeksforgeeks.org/observer-pattern-set-1-introduction/

# 3. Delegate

Stores one or more functors, calls all stored functors on delegate invoke. Supports standalone functions and member functions with weak-pointer safety.

```cpp
template <typename... A>
class Delegate {
  using Functor = std::function<void(A...)>;

  void add(const Functor& func);

  template <class O>
  void addMethod(void (O::*memberFn)(A...), std::shared_ptr<O> obj);

  void clear();

  void invoke(A... args);

  void operator=(Functor c);

  void operator+=(Functor c);

  void operator()();
};
```

## 3.1 Implementation

- Add functors in delegate must have same function signature
- Arbitrary arguments supported
- Return is always void in functors

## 3.2 Usage

```cpp
void hello(){
  std::cout << "hello" << std::endl;
}

class Foo {
public:
  void print();

  void add(int x, int y);
};

{
  Delegate d;

  d += hello;

  d.invoke(); // calls hello

  {
    Foo foo;

    d.addMethod(&Foo::hello, foo);

    d.invoke(); // calls hello and foo.print
  }

  d.invoke(); // calls hello, removes foo.print from delegate
}

Delegate<int, int> d;
d.addMethod(&Foo::add, foo);
d.invoke(12, 24);  // calls foo.add, note return is not supported
```

## 3.3 Other resources

- https://stackoverflow.com/questions/26497541/stdbind-and-stdweak-ptr/26497812
