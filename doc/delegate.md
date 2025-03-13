# Delegate

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

## Implementation

- Add functors in delegate must have same function signature
- Arbitrary arguments supported
- Return is always void in functors

## Usage

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

## Other resources

- https://stackoverflow.com/questions/26497541/stdbind-and-stdweak-ptr/26497812
