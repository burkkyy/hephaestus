#pragma once

#include <sstream>

#include "event.hpp"
#include "key_codes.hpp"

namespace alp {

class KeyEvent : public Event {
 public:
  KeyCode getKeyCode() const { return this->keycode; }

  virtual int getCategoryFlags() const override {
    return EventCategoryKeyboard | EventCategoryInput;
  }

 protected:
  KeyEvent(const KeyCode keycode) : keycode{keycode} {}

  KeyCode keycode;
};

class KeyPressedEvent : public KeyEvent {
 public:
  KeyPressedEvent(const KeyCode keycode, bool repeated = false)
      : KeyEvent{keycode}, repeated{repeated} {}

  bool isRepeat() const { return this->repeated; }

  std::string toString() const override {
    std::stringstream ss;
    ss << "KeyPressedEvent: " << this->keycode
       << " (repeat = " << this->repeated << ")";
    return ss.str();
  }

  virtual const char* getName() const override { return "KeyPressed"; }

 private:
  bool repeated;
};

class KeyReleasedEvent : public KeyEvent {
 public:
  KeyReleasedEvent(const KeyCode keycode) : KeyEvent{keycode} {}

  std::string toString() const override {
    std::stringstream ss;
    ss << "KeyReleasedEvent: " << this->keycode;
    return ss.str();
  }

  virtual const char* getName() const override { return "KeyReleased"; }
};

class KeyTypedEvent : public KeyEvent {
 public:
  KeyTypedEvent(const KeyCode keycode) : KeyEvent{keycode} {}

  std::string toString() const override {
    std::stringstream ss;
    ss << "KeyTypedEvent: " << this->keycode;
    return ss.str();
  }

  virtual const char* getName() const override { return "KeyTyped"; }
};

}  // namespace alp
