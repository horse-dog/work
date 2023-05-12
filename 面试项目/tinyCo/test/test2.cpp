// https://www.cnblogs.com/pointer-smq/p/12940360.html

#include <algorithm>
#include <chrono>
#include <coroutine>
#include <deque>
#include <functional>
#include <iostream>
#include <memory>
#include <thread>
#include <vector>
using namespace std;
using namespace chrono;

template <class T>
class Future;

template <class T>
class Promise;

class Schedular;

class SharedStateBase : public enable_shared_from_this<SharedStateBase> {
  friend class Schedular;

 public:
  virtual ~SharedStateBase() = default;

 private:
  /**
   * @brief 执行所有回调函数
   */
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
  /**
   * @brief 设置value
   *
   * @tparam U
   * @param v value
   */
  template <class U>
  void set(U&& v) {
    if (settled) {
      return;
    }
    settled = true;
    value = forward<U>(v);
    post_all_callbacks();
  }

  /**
   * @brief 获取value
   *
   * @return T&
   */
  T& get() { return value; }

  /**
   * @brief 向完成设置后的回调队列添加一个回调函数
   *
   * @param callback 回调函数
   */
  void add_finish_callback(function<void(T&)> callback) {
    finish_callbacks.push_back(move(callback));
    if (settled) {
      post_all_callbacks();
    }
  }

  /**
   * @brief 将自己提交给调度器
   * 观察者模式
   */
  void post_all_callbacks();

  /**
   * @brief 执行所有回调函数
   */
  virtual void invoke_all_callback() override {
    callback_posted = false;
    size_t sz = finish_callbacks.size();
    for (size_t i = 0; i != sz; i++) {
      auto v = move(finish_callbacks[i]);
      v(value);
    }
    finish_callbacks.erase(finish_callbacks.begin(),
                           finish_callbacks.begin() + sz);
  }

  bool settled = false;           /*> 是否已经设置值 */
  bool callback_posted = false;   /*> 是否已经在观察调度器 */
  Schedular* schedular = nullptr; /*> 观察的调度器 */
  T value;                        /*> 值 */
  vector<function<void(T&)>> finish_callbacks; /* 完成值的设置后的回调队列 */
};

template <class T>
class Promise {
 public:
  Promise(Schedular& schedular)
      : _schedular(&schedular),
        _state(make_shared<SharedState<T>>(*_schedular)) {}

  /**
   * @brief 获取future对象
   *
   * @return Future<T>
   */
  Future<T> get_future() { return Future<T>(_state); }

  // 协程接口
  Future<T> get_return_object() { return get_future(); }
  suspend_never initial_suspend() { return {}; }
  suspend_never final_suspend() noexcept { return {}; }
  void return_value(T v) { set_result(v); }
  void unhandled_exception() { terminate(); }

  /**
   * @brief 设置共享状态的值
   *
   * @tparam U
   * @param value 值
   */
  template <class U>
  void set_result(U&& value) {
    if (_state->settled) {
      throw invalid_argument("already set result");
    }
    _state->set(forward<U>(value));
  }

 private:
  Schedular* _schedular;             /*> 调度器 */
  shared_ptr<SharedState<T>> _state; /* 共享状态 */
};

template <class T>
class Future {
 public:
  using result_type = T;
  using promise_type = Promise<T>;
  friend class Promise<T>;

 private:
  Future(shared_ptr<SharedState<T>> state) : _state(move(state)) {}

 public:
  // 协程接口
  bool await_ready() { return _state->settled; }
  void await_suspend(coroutine_handle<> handle) {
    add_finish_callback([=](T&) mutable { handle.resume(); });
  }
  T await_resume() { return _state->value; }

  /**
   * @brief 添加完成值的设置后的回调
   *
   * @param callback 回调函数
   */
  void add_finish_callback(function<void(T&)> callback) {
    _state->add_finish_callback(move(callback));
  }

 private:
  shared_ptr<SharedState<T>> _state; /*> 共享状态 */
};

class Schedular {
  template <class T>
  friend class SharedState;

 public:
  using timer_callback = function<void()>;
  // 第一个bool属性代表该定时器是否重复，第二个float代表如果该定时器重复，下次触发定时器的间隔时间
  using timer_item =
      tuple<bool, float, time_point<steady_clock, duration<float>>,
            timer_callback>;
  using timer = steady_clock;
  struct timer_item_cmp {
    bool operator()(const timer_item& a, const timer_item& b) const {
      return get<2>(a) > get<2>(b);
    }
  };

  Schedular() = default;
  Schedular(Schedular&&) = delete;
  Schedular(const Schedular&) = delete;
  Schedular& operator=(Schedular&&) = delete;
  Schedular& operator=(const Schedular&) = delete;

  /**
   * @brief 通知所有观察者执行回调
   */
  void poll() {
    size_t sz = pending_states.size();
    for (size_t i = 0; i != sz; i++) {
      auto state = move(pending_states[i]);
      state->invoke_all_callback();
    }
    pending_states.erase(pending_states.begin(), pending_states.begin() + sz);

    if (timer_queue.empty()) return;

    // 如果pending_states为空，则可以sleep较长的时间，等待第一个将要完成的timer
    if (pending_states.empty()) {
      this_thread::sleep_until(get<2>(timer_queue.front()));
      auto now = timer::now();
      while (!timer_queue.empty() && get<2>(timer_queue.front()) <= now) {
        deal_one_timer();
      }
    } else {  // 否则只能处理当帧到期的timer，不能sleep，要及时返回给caller，让caller及时下一次poll处理剩下的pending_states
      auto now = timer::now();
      while (!timer_queue.empty() && get<2>(timer_queue.front()) <= now) {
        deal_one_timer();
      }
    }
  }

  // 总调度器，总调度器不能是一个协程，否则调度器作为协程会被其他地方调度，那就不能称为总调度器了
  // 因此，auto future = forward<F>(fn)(forward<Args>(args)...) 不能写为：
  // auto future = co_await forward<F>(fn)(forward<Args>(args)...);
  template <class F, class... Args>
  auto run_until_complete(F&& fn, Args&&... args) ->
      typename invoke_result_t<F&&, Args&&...>::result_type {
    auto future = forward<F>(fn)(forward<Args>(args)...);
    while (!future.await_ready()) {
      poll();
    }
    return future.await_resume();
  }

  /**
   * @brief 添加一个定时器
   *
   * @param repeat 是否重复
   * @param delay 定时时间
   * @param callback 回调
   */
  void add_timer(bool repeat, float delay, timer_callback callback) {
    auto cur_time = time_point_cast<duration<float>>(timer::now());
    auto timeout = cur_time + duration<float>(delay);
    timer_queue.emplace_back(repeat, delay, timeout, callback);
    push_heap(timer_queue.begin(), timer_queue.end(), timer_item_cmp{});
  }

  /**
   * @brief delay
   *
   * @param second 时间
   * @return Future<float>
   */
  Future<float> delay(float second) {
    auto promise = Promise<float>(*this);
    add_timer(false, second, [=]() mutable {
      promise.set_result(second);
    });
    return promise.get_future();
  }

 private:
  /**
   * @brief 处理一个超时回调
   */
  void deal_one_timer() {
    pop_heap(timer_queue.begin(), timer_queue.end(), timer_item_cmp{});
    auto item = move(timer_queue.back());
    timer_queue.pop_back();
    get<3>(item)();
    if (get<0>(item)) {
      add_timer(true, get<1>(item), move(get<3>(item)));
    }
  }

  /**
   * @brief 添加一个观察者
   *
   * @param state 观察者（共享状态）
   */
  void post_call_state(shared_ptr<SharedStateBase> state) {
    pending_states.push_back(move(state));
  }
  vector<shared_ptr<SharedStateBase>> pending_states; /*> 待处理的共享状态 */
  deque<timer_item> timer_queue;                      /*> 定时器容器 */
};

template <class T>
void SharedState<T>::post_all_callbacks() {
  if (callback_posted) {
    return;
  }
  callback_posted = true;
  schedular->post_call_state(shared_from_this());
}

Future<float> func(Schedular& schedular) {
  cout << "start sleep\n";
  auto r = co_await schedular.delay(1.2);
  co_return r;
}

Future<int> func2(Schedular& schedular) {
  auto r = co_await func(schedular);
  cout << "slept for " << r << "s\n";
  co_return 42;
}

int main() {
  Schedular schedular;

  auto r = schedular.run_until_complete(func2, schedular);

  cout << "run complete with " << r << "\n";
}