// https://www.cnblogs.com/pointer-smq/p/12900444.html

#include <functional>
#include <iostream>
#include <memory>
#include <vector>

template <class T>
class Future;

template <class T>
class Promise;

class Schedular;

class SharedStateBase : public std::enable_shared_from_this<SharedStateBase> {
  friend class Schedular;

 public:
  virtual ~SharedStateBase() = default;

 private:
  virtual void invoke_all_callback() = 0;
};

template <class T>
class SharedState : public SharedStateBase {
  friend class Future<T>;
  friend class Promise<T>;

 public:
  SharedState(Schedular& schedular) : schedular(&schedular) {}
  SharedState(const SharedState&) = delete;
  SharedState(SharedState&&) = delete;
  SharedState& operator=(const SharedState&) = delete;
  SharedState& operator=(SharedState&&) = delete;

 private:
  template <class U>
  void set(U&& v) {
    if (settled) {
      return;
    }
    settled = true;
    value = std::forward<U>(v);
    post_all_callbacks();
  }

  T& get() { return value; }

  void add_finish_callback(std::function<void(T&)> callback) {
    finish_callbacks.push_back(std::move(callback));
    post_all_callbacks();
  }

  void post_all_callbacks();

  virtual void invoke_all_callback() override {
    callback_posted = false;
    size_t sz = finish_callbacks.size();
    for (size_t i = 0; i != sz; i++) {
      auto v = std::move(finish_callbacks[i]);
      v(value);
    }
    finish_callbacks.erase(finish_callbacks.begin(),
                           finish_callbacks.begin() + sz);
  }

  bool has_owner = false;
  bool settled = false;
  bool callback_posted = false;
  Schedular* schedular = nullptr;
  T value;
  std::vector<std::function<void(T&)>> finish_callbacks;
};

template <class T>
class Promise {
 public:
  Promise(Schedular& schedular)
      : _schedular(&schedular),
        _state(std::make_shared<SharedState<T>>(*_schedular)) {}

  Future<T> get_future();

  template <class U>
  void set_result(U&& value) {
    if (_state->settled) {
      throw std::invalid_argument("already set result");
    }
    _state->set(std::forward<U>(value));
  }

 private:
  Schedular* _schedular;
  std::shared_ptr<SharedState<T>> _state;
};

template <class T>
class Future {
  friend class Promise<T>;

 private:
  Future(std::shared_ptr<SharedState<T>> state) : _state(std::move(state)) {}

 public:
  void add_finish_callback(std::function<void(T&)> callback) {
    _state->add_finish_callback(std::move(callback));
  }

 private:
  std::shared_ptr<SharedState<T>> _state;
};

template <class T>
Future<T> Promise<T>::get_future() {
  return Future<T>(_state);
}

class Schedular {
  template <class T>
  friend class SharedState;

 public:
  Schedular() = default;
  Schedular(Schedular&&) = delete;
  Schedular(const Schedular&) = delete;
  Schedular& operator=(Schedular&&) = delete;
  Schedular& operator=(const Schedular&) = delete;

  void poll() {
    size_t sz = pending_states.size();
    for (size_t i = 0; i != sz; i++) {
      auto state = std::move(pending_states[i]);
      state->invoke_all_callback();
    }
    pending_states.erase(pending_states.begin(), pending_states.begin() + sz);
  }

 private:
  void post_call_state(std::shared_ptr<SharedStateBase> state) {
    pending_states.push_back(std::move(state));
  }

  std::vector<std::shared_ptr<SharedStateBase>> pending_states;
};

template <class T>
void SharedState<T>::post_all_callbacks() {
  if (callback_posted) {
    return;
  }
  callback_posted = true;
  schedular->post_call_state(shared_from_this());
}

int main() {
  Schedular schedular;
  Promise<int> promise(schedular);
  Future<int> future = promise.get_future();
  std::cout << "future get\n";
  promise.set_result(10);
  std::cout << "promise result set\n";
  future.add_finish_callback(
      [](int v) { std::cout << "callback 1 got result " << v << "\n"; });
  std::cout << "future callback add\n";
  std::cout << "tick 1\n";
  schedular.poll();
  std::cout << "tick 2\n";
  future.add_finish_callback(
      [](int v) { std::cout << "callback 2 got result " << v << "\n"; });
  std::cout << "future callback 2 add\n";
  schedular.poll();

  std::cout << "\n";

  Promise<double> promise2(schedular);
  promise2.set_result(12.34);
  std::cout << "promise result2 set\n";
  Future<double> future2 = promise2.get_future();
  std::cout << "future2 get\n";
  future2.add_finish_callback([&](double v) {
    std::cout << "future2 callback 1 got result" << v << "\n";
    future2.add_finish_callback([](double v) {
      std::cout << "future2 callback 2 got result" << v << "\n";
    });
    std::cout << "future2 callback 2 add inside callback\n";
  });
  std::cout << "future2 callback add\n";
  std::cout << "tick 3\n";
  schedular.poll();
  std::cout << "tick 4\n";
  schedular.poll();
}