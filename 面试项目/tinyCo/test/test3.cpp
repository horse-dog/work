// https://www.cnblogs.com/pointer-smq/p/12950668.html

#include <coroutine>
#include <iostream>
using namespace std;

template <class T>
struct Generator {
  struct Promise {
    suspend_never initial_suspend() { return {}; }
    suspend_always final_suspend() noexcept { return {}; }
    Generator<T> get_return_object() {
      return {std::coroutine_handle<Generator<T>::promise_type>::from_promise(
          *this)};
    }
    void unhandled_exception() { std::terminate(); }
    suspend_always yield_value(T v) {
      _current = std::move(v);
      return {};
    }
    void return_void() {}
    T _current;
  };

  using promise_type = Promise;
  struct iterator_end_sentinel {};
  struct iterator {
    template <class>
    using iterator_category = std::input_iterator_tag;
    using value_type = T;
    T operator*() { return _promise->_current; }
    void operator++() {
      coroutine_handle<Promise>::from_promise(*_promise).resume();
    }
    bool operator!=(iterator_end_sentinel) {
      return !coroutine_handle<Promise>::from_promise(*_promise).done();
    }

    iterator(Promise* promise) : _promise(promise) {}
    Promise* _promise;
  };

  Generator(std::coroutine_handle<Promise> hdr) : _handle(hdr) {}
  ~Generator() { _handle.destroy(); }
  Generator(const Generator& other) = delete;
  Generator& operator=(const Generator& other) = delete;
  Generator& operator=(Generator&& other) noexcept = delete;

  iterator begin() { return {&(_handle.promise())}; }
  iterator_end_sentinel end() { return {}; }

  std::coroutine_handle<Promise> _handle;
};

Generator<int> func() {
  for (int i = 0; i < 10; i++) {
    co_yield i;
  }
}

int main() {
  for (auto i : func()) {
    std::cout << i << " ";
  }
  std::cout << std::endl;
  return 0;
}