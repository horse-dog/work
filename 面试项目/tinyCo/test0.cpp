// https://www.cnblogs.com/DarkH/p/16153260.html

#include <coroutine>
#include <iostream>
#include <thread>

namespace Coroutine {
struct task {
  struct promise_type {
    promise_type() {
      std::cout << "1.task-promise_type():create promise object\n";
    }

    task get_return_object() {
      std::cout << "2.task-get_return_object():create coroutine return object, "
                   "and the coroutine is created now\n";
      return {std::coroutine_handle<task::promise_type>::from_promise(*this)};
    }

    // initial_suspend()决定协程初始化后，是继续直接继续执行协程，还是挂起协程返回caller
    // 返回std::suspend_never，表示不挂起协程，会继续执行协程函数体(coroutine
    // body) 返回std::suspend_always，表示挂起协程，不会去执行coroutine
    // body，程序的执行返回到caller那里
    std::suspend_never initial_suspend() {
      std::cout << "3.task-initial_suspend():do you want to susupend the "
                   "current coroutine?\n";
      std::cout
          << "4.task-initial_suspend():don't suspend because return "
             "std::suspend_never, so continue to execute coroutine body\n";
      return {};
    }

    // 调用完void return_void()或者void return_value(T
    // v)后，就会调用final_suspend()
    // 如果final_suspend返回std::suspend_never表示不挂起协程，那么协程就会自动销毁，先后销毁promise,
    // 协程帧上得参数和协程帧；
    // 如果返回std::suspend_always则不会自动销毁协程，需要用户手动去删除协程。
    std::suspend_never final_suspend() noexcept {
      std::cout << "15.task-final_suspend():coroutine body finished, do you "
                   "want to susupend the current coroutine?\n";
      std::cout << "16.task-final_suspend():don't suspend because return "
                   "std::suspend_never, and the continue will be automatically "
                   "destroyed, bye\n";
      return {};
    }

    // 如果协程是void没有返回值，那么就需要定义void return_void()
    // 如果有返回值那么就定义void return_value(T v),用来保存协程的返回值
    // return_value或者return_void，这两个方法只允许存在一个
    void return_void() {
      std::cout << "14.task-return_void():coroutine don't return value, so "
                   "return_void is called\n";
    }

    void unhandled_exception() {}
  };

  std::coroutine_handle<task::promise_type> handle_;
};

struct awaiter {
  // 调用co_wait awaiter{};时调用await_ready()
  // 表示是否准备好，要不要挂起协程
  // await_ready()返回false一般表示要挂起协程，并执行await_suspend
  // 返回true说明协程已经执行完了，这时候调用await_resume返回协程的结果。
  bool await_ready() {
    std::cout << "6.await_ready():do you want to suspend current coroutine?\n";
    std::cout << "7.await_ready():yes, suspend becase awaiter.await_ready() "
                 "return false\n";
    return false;
  }

  // await_suspend 的返回值来决定是返回 caller 还是继续执行。
  // 返回void：协程执行权交还给当前协程的caller。当前协程在未来某个时机被resume之后，然后执行协程函数中co_await下面的语句
  // 返回true：同返回void。
  // 返回false：直接执行await_resume
  void await_suspend(std::coroutine_handle<task::promise_type> handle) {
    std::cout << "8.await_suspend(std::coroutine_handle<task::promise_type> "
                 "handle):execute awaiter.await_suspend()\n";
    std::thread([handle]() mutable {
      std::cout << "11.lambada():resume coroutine to execute coroutine body\n";
      handle();  // 等价于handle.resume();
      std::cout << "17.lambada():over\n";
    }).detach();
    std::cout
        << "9.await_suspend(std::coroutine_handle<task::promise_type> "
           "handle):a new thread lauched, and will return back to caller\n";
  }

  // 调用完await_resume后直接执行协程函数中co_await下面的语句
  void await_resume() { std::cout << "12.await_resume()\n"; }
};

task test() {
  std::cout << "5.test():begin to execute coroutine body, the thread id="
            << std::this_thread::get_id()
            << ",and call co_await awaiter{};\n";  // #1
  co_await awaiter{};
  std::cout << "13.test():coroutine resumed, continue execute coroutine body "
               "now, the thread id="
            << std::this_thread::get_id() << "\n";  // #3
}

template <typename T>
struct lazy {
 public:
  struct promise_type;

  lazy(std::coroutine_handle<promise_type> handle) : m_handle(handle) {
    std::cout << "3.lazy(std::coroutine_handle<promise_type> handle):Construct "
                 "a lazy object"
              << std::endl;
  }

  ~lazy() {
    std::cout << "15.~lazy():Destruct a lazy object " << std::endl;
    m_handle.destroy();
  }

  T get() {
    std::cout << "6.lazy.get():I want to execute the coroutine now. call "
                 "m_handle.resume()"
              << std::endl;
    if (!m_handle.done()) {
      m_handle.resume();
    }
    std::cout << "13.lazy.get():We got the return value...:"
              << m_handle.promise().value << std::endl;
    return m_handle.promise().value;
  }

  struct promise_type {
    T value = {};

    promise_type() {
      std::cout << "1.lazy-promise_type():Promise created" << std::endl;
    }

    ~promise_type() {
      std::cout << "16.lazy- ~promise_type():Promise died" << std::endl;
    }

    auto get_return_object() {
      std::cout << "2.lazy-get_return_object():create coroutine return object, "
                   "and the coroutine is created now"
                << std::endl;
      return lazy<T>{std::coroutine_handle<promise_type>::from_promise(*this)};
    }

    auto initial_suspend() {
      std::cout << "4.lazy-initial_suspend():Started the coroutine"
                << std::endl;
      return std::suspend_always{};
    }

    auto final_suspend() noexcept {
      std::cout << "12.lazy-final_suspend():Finished the coroutine"
                << std::endl;
      return std::suspend_always{};
    }

    void return_value(T v) {
      std::cout << "11.lazy-return_value(T v):Got coroutine result " << v
                << std::endl;
      value = v;
    }

    void unhandled_exception() { std::exit(1); }

    // 协程体中调用co_yield xxx;的时候调用yield_value(T val)
    auto yield_value(T val) {
      std::cout << "9.lazy-yield_value(T val): " << val << std::endl;
      value = val;

      // 后续不再挂起协程，继续执行
      return std::suspend_never();

      //                //后续继续挂起协程
      //                return std::suspend_always();
    }
  };

  std::coroutine_handle<promise_type> m_handle;
};

lazy<int> my_coroutine() {
  std::cout << "7.my_coroutine():Execute the coroutine function body"
            << std::endl;
  std::cout << "8.my_coroutine():call---co_yield 66;" << std::endl;
  co_yield 66;
  std::cout << "10.my_coroutine():call---co_return 88;" << std::endl;
  co_return 88;
}
}  // namespace Coroutine

int main() {
  Coroutine::test();
  std::cout << "10.main():come back to caller becuase of co_await awaiter\n";
  std::this_thread::sleep_for(std::chrono::seconds(1));

  std::cout << "-----------------------------------" << std::endl;

  auto coro = Coroutine::my_coroutine();
  std::cout << "5.main():call coro.get()" << std::endl;
  auto result = coro.get();
  std::cout << "14.main():The coroutine result: " << result << std::endl;

  std::cout << "main():Second call coro.get()  " << std::endl;
  result = coro.get();
  std::cout << "main():The coroutine result2: " << result << std::endl;

  return 0;
}
