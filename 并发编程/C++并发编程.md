### 多线程相关头文件

- `<atomic>`：std::atomic 和 std::atomic_flag

- `<thread>`：std::thread 和 std::this_thread

- `<mutex>`：std::mutex 系列类，std::lock_guard，std::unique_lock

- `<condition_variable>`：std::condition_variable 和 std::condition_variable_any

- `<future>`：std::promise, std::package_task 两个 Provider 类，以及 std::future 和 std::shared_future 两个 Future 类，std::async() 函数


### volatile 关键字

- volatile 提醒编译器它后面所定义的变量随时都有可能改变，因此编译后的程序每次需要存储或读取这个变量的时候，告诉编译器对该变量不做优化，都会直接从变量内存地址中读取数据，从而可以提供对特殊地址的稳定访问。


### std::this_thread

```cpp
// 获取当前线程的 id
std::thread::id std::this_thread::get_id();
// 阻塞当前线程一段时间
void std::this_thread::sleep_for(const duration &time);
// 阻塞当前线程到某个时间点
void std::this_thread::sleep_until(const time_point &time);
// 放弃当前线程的时间片，使CPU重新调度以便其它线程执行
void std::this_thread::yield();
```

### thread_local

- 用 thread_local 修饰的变量具有 thread 周期，每一个线程都拥有并只拥有一个该变量的独立实例， 一般用于需要保证线程安全的函数中。

```cpp
struct A {
 public:
  A &test(const std::string &name) {
    thread_local int count = 0;
    ++count;
    cout << name << ": " << count << std::endl;
    return *this;
  }
};

void func(const string &name) {
  A a1, a2;
  a1.test(name).test(name);
  a2.test(name).test(name);
}

// 输出：
// t1: 1
// t1: 2
// t1: 3
// t1: 4
// t2: 1
// t2: 2
// t2: 3
// t2: 4
int main(int argc, char *argv[]) {
  thread t1(func, "t1");
  t1.join();
  thread t2(func, "t2");
  t2.join();
  return 0;
}
```


### std::thread

- 没有拷贝语义

- 具有移动语义

```cpp
bool std::thread::joinable();                     
void std::thread::join();
void std::thread::detach();
void std::thread::swap(std::thread &other);
// 获取 handle，可用于 pthread 相关操作
std::thread::native_handle_type std::thread::native_handle();
static unsigned int std::thread::hardware_concurrency();
```


### Lockable语义

- BasicLockable 类型：满足两种操作的类：lock 和 unlock 的类型

- Lockable 类型：满足三种操作的类：lock、unlock 和 try_lock 的类型

- TimedLockable 类型：满足五种操作的类：lock、unlock、try_lock、try_lock_for、try_lock_until

- SharedLockable 类型：满足六种操作的类：lock、unlock、try_lock、lock_shared、unlock_shared、try_lock_shared

- SharedTimedLockable 类型：满足十种操作的类：TimedLockable 的五个函数及其 shared 版本


### std::mutex系列

- std::mutex：独占的互斥量，不能递归使用，不带超时功能

    ```cpp
    void std::_Mutex_base::lock();
    bool std::_Mutex_base::try_lock();
    void std::_Mutex_base::unlock();
    ```

- std::recursive_mutex：递归互斥量，可重入，不带超时功能

- std::time_mutex：带超时的互斥量，不能递归

    ```cpp
    // lock、try_lock、unlock 与 mutex 相同
    bool std::timed_mutex::try_lock_for(const chrono::milliseconds &time);
    bool std::timed_mutex::try_lock_until(const chrono::time_point &time);
    ```

- std::recursive_timed_mutex：带超时的互斥量，可以递归使用

- std::shared_mutex：读写互斥量，根据读写锁的定义，读写锁均不可递归使用，因而不存在 recursive_shared_mutex 类

    ```cpp
    // 写锁
    void std::shared_mutex::lock();
    bool std::shared_mutex::try_lock();
    void std::shared_mutex::unlock();

    // 读锁
    void std::shared_mutex::lock_shared();
    bool std::shared_mutex::try_lock_shared();
    void std::shared_mutex::unlock_shared();
    ```

- std::shared_timed_mutex：带超时的读写互斥量，根据读写锁的定义，读写锁均不可递归使用，因而不存在 recursive_shared_timed_mutex 类


### std::lock

- 相关的 Tag 类

    ```cpp
    struct adopt_lock_t {};
    struct defer_lock_t {};
    struct try_to_lock_t {};

    constexpr adopt_lock_t adopt_lock{};
    constexpr defer_lock_t defer_lock{};
    constexpr try_to_lock_t try_to_lock{};
    ```

- std::lock_guard

    - 比 unique_lock 轻量级

    - 是一个模板类，模板参数 Mutex 代表互斥量类型，它应该是一个基本的 BasicLockable 类型

    - locking 初始化：在构造时对互斥量进行上锁

        ```cpp
        explicit lock_guard (mutex_type& m);
        ```
        
    - adopting 初始化：在构造时不对互斥量进行上锁

        ```cpp
        lock_guard (mutex_type& m, adopt_lock_t tag);
        ```

    - 禁用拷贝语义和移动语义

- std::unique_lock

    - 比 lock_guard 更灵活
    
    - 是一个模板类，模板参数 Mutex 代表互斥量类型，它应该是一个基本的 BasicLockable 类型

    - 初始化方式
    
        - locking 初始化：同 lock_guard
        
        - adopting 初始化：同 lock_guard
        
        - try-locking 初始化：unique_lock(mutex_type& m, try_to_lock_t tag);，构造函数调用互斥量的 try_lock 函数
        
        - deferred 初始化：unique_lock(mutex_type& m, defer_lock_t tag) noexcept; 在初始化的时候并不锁住互斥量，暗示之后会在 unique_lock 对象外部手动锁住互斥量

        - locking_for 初始化：unique_lock(mutex_type& m, const duration& time);，构造函数调用互斥量的 lock_for 函数，若互斥量无该函数，编译报错
        
        - locking_until 初始化：unique_lock(mutex_type& m, const time_point& time);，构造函数调用互斥量的 lock_until 函数，若互斥量无该函数，编译报错
        
        - 拷贝构造被禁用，但移动构造未被禁用

    - 成员函数
        - lock、try_lock、try_lock_for、try_lock_until 和 unlock：调用互斥量的对应函数
        
        - swap：与另一个 unique_lock 对象进行交换
        
        - release：返回指向它所管理的 Mutex 对象的指针，并释放所有权
        
        - mutex：返回所管理的 Mutex 对象的指针
        
        - owns_lock：返回当前 std::unique_lock 对象是否获得了锁
        
        - operator bool()：与 owns_lock 功能相同

- std::shared_lock：类似于 unique_lock，模板参数需要是 SharedLockable 的，底层操作的是互斥量的 shared 版本函数

- std::scoped_lock：类似于 lock_guard，但可以对多个互斥量进行上锁


### std::lock 函数与 std::try_lock 函数

- std::lock 函数

    - 它可以一次性 lock 两个或者两个以上的互斥量。
    
    - 它不存在因为多个线程中因为锁的顺序问题导致死锁的风险的问题。

- std::try_lock 函数

    - 它可以一次性 try_lock 两个或者两个以上的互斥量。

    - 如果某一个互斥量 lock 失败，函数退出并解锁已经上锁的所有互斥量。

    - 返回一个整数，在成功时返回-1，失败时会返回失败的互斥量的索引(从0开始)


### std::once_flag 与 std::call_once

- std::once_flag 为一个类型，它只用默认构造函数构造，不能拷贝不能移动，表示函数的一种内在状态，使用时需要配合 call_once 函数。

- 多个线程同时调用某个函数，std::call_once 函数可以保证多个线程对该函数只调用一次。

    ```cpp
    template< class Callable, class... Args >
    void call_once( std::once_flag& flag, Callable&& f, Args&&... args );
    ```


### std::promise

- std::promise 对象可以保存某一类型 T 的值，该值可被 future 对象读取（可能在另外一个线程中）

- 可以通过 std::promise::get_future 来获取与该 promise 对象相关联的 future 对象

- std::promise 禁用拷贝语义，保留移动语义

- 成员函数

    ```cpp
    // 返回关联的future对象
    std::future<T> get_future();
    // 设置共享状态的值，此后promise的共享状态标志变为ready
    void set_value(val);
    // 设置共享状态的值，但是不将共享状态的标志设置为ready，当线程退出时该promise对象会自动设置为ready
    void set_value_at_thread_exit(val);
    // 为promise设置异常，此后promise的共享状态变标志变为ready
    void set_exception(std::exception_ptr _Exc);
    // 为promise设置异常，但是不将共享状态的标志设置为ready，当线程退出时该promise对象会自动设置为ready
    void set_exception_at_thread_exit(std::exception_ptr _Exc);
    // 交换函数
    void swap(std::promise<T> &_Other);
    ```

- 使用示例

    ```cpp
    void func(std::future<int>& fut) {
        int x = fut.get();
        cout << "value: " << x << endl;
    }

    // 执行结果：5秒后输出 "value: 6"
    int main() {
        std::promise<int> prom;
        std::future<int> fut = prom.get_future();
        std::thread t(func, std::ref(fut));
        this_thread::sleep_for(5s);
        prom.set_value(6);
        t.join();
        return 0;
    }
    ```

### std::packaged_task

- std::packaged_task 包装一个可调用的对象，将其包装的可调用对象的执行结果传递给一个 std::future 对象，允许异步获取可调用对象产生的结果

- 可以通过 std::packged_task::get_future 来获取与共享状态相关联的 std::future 对象

- std::packaged_task 禁用拷贝语义，保留移动语义

- 成员函数

    ```cpp
    // 返回当前 packaged_task是否包装了一个可调用对象，例如 packaged_task 默认构造后该函数返回 false
    bool valid();
    // 返回关联的future对象
    std::future<T> get_future();
    // 操作符重载，使得 packaged_task 对象可以像可调用对象一样直接调用，并在调用后设置共享状态标志为ready
    void operator()(_ArgTypes... _Args);
    // 调用 packaged_task 的可调用对象，但是不将共享状态的标志设置为 ready，当线程退出时该 packaged_task 对象会自动设置为 ready
    void make_ready_at_thread_exit(_ArgTypes... _Args);
    // 重置 packaged_task 的共享状态，但是保留之前的被包装的任务
    void reset();
    // 交换函数
    void swap(std::packaged_task<T> &_Other);
    ```

- 使用示例

    ```cpp
    int func(int in) {
      this_thread::sleep_for(5s);
      return in + 1;
    }

    // 执行结果：5秒后输出 "value: 6", 再过5秒后输出 "value: 11"
    int main() {
      std::packaged_task<int(int)> task(func);
    
      auto fut = task.get_future();
      std::thread(std::ref(task), 5).join();
      cout << "result: " << fut.get() << endl;
    
      task.reset();
      fut = task.get_future();
      std::thread(std::ref(task), 10).join();
      cout << "result: " << fut.get() << endl;
      return 0;
    }
    ```

### std::async

- std::async 用于创建异步任务，实际上就是创建一个线程异步执行相应任务，它接受回调（即函数或函数对象）作为参数。

- std::async 是异步编程的高级封装，相当于封装了 std::promise、std::packaged_task 和 std::thread，基本上可以代替 std::thread 的所有事情。

    ```cpp
    // 采用系统默认启动策略
    future async(Fn&& fn, Args&&... args);

    /**
     * @brief 采用指定启动策略执行异步任务
        * @param policy 启动策略，以下3种取值之一
        *  launch::async：它保证了异步行为，即传递的函数将在单独的线程中执行。
        *  launch::deferred：非异步行为，即当其他线程将来调用 get() 或 wait() 以访问共享状态时，将调用 fn。
        *  launch::async | launch::deferred：系统默认行为。它可以异步运行或不异步运行，具体取决于系统上的负载。但是我们无法控制它。
        * @param fn 可调用对象，可以是函数、仿函数、lambda 表达式、packaged_task等，当可调用对象为 packaged_task 时，返回值恒为 future<void>
        * @param args 可调用对象 fn 的输入参数
        * @return future对象
        */
    future async(launch policy, Fn&& fn, Args&&... args);
    ```

- 使用示例

    ```cpp
    int func(int in) {
    this_thread::sleep_for(5s);
    return in + 1;
    }
    // 执行结果：5秒后输出 "value: 6"
    int main() {
    auto fut = async(func, 5);
    cout << fut.get() << endl;
    return 0;
    }
    ```

### std::future

- std::future 可以用来获取异步任务的结果

- std::future 通常由某个 Provider 创建（async、promise 或 packaged_task）

- std::future 禁用拷贝语义，保留移动语义

- 成员函数

    ```cpp
    // 检查当前的 std::future 对象是否有效，即是否与某个共享状态相关联。
    // 一个有效的 std::future 对象只能通过 std::async(),std::promise::get_future 或者 std::packaged_task::get_future 来初始化。
    // 由 std::future 默认构造函数创建的 std::future 对象是无效(invalid)的。
    bool valid();

    // 通过 get 函数（通常在另外一个线程中）获取值
    // 如果共享状态的标志不为 ready，则调用 get 会阻塞当前的调用者，直到关联的promise对象设置共享状态的标志变为 ready，get 返回异步任务的值或异常（如果发生了异常）
    T get();

    // 返回一个 std::shared_future 对象。调用该函数之后，该 std::future 对象本身已经不和任何共享状态相关联，因此该 std::future 的状态不再是 valid 的了。
    std::shared_future<T> share();

    // 阻塞等待与当前std::future 对象相关联的共享状态的标志变为 ready。但不读取共享状态的值或异常
    void wait();

    // 与 wait 类似：
    // (1)当future对象是从deferred启动的async函数创建时，立即返回 future_status::deferred
    // (2)否则，等待 time 指定的时间，未超时则返回 future_status::ready
    // (3)超时返回 future_status::timeout
    std::future_status wait_for(duration &time);

    // 与 wait_for类似，等待至某一个时刻
    std::future_status wait_until(time_point &time);
    ```

- 相关结构

    ```cpp
    // 主要用在 std::future (或 std::shared_future) 中的 wait_for 和 wait_until 两个函数中
    enum class future_status { ready, timeout, deferred };

    // 主要用在调用 std::async 时设置异步任务的启动策略
    enum class launch { async = 0x1, deferred = 0x2 };

    // 继承自 C++ 标准异常体系中的 logic_error
    class future_error : public logic_error;

    // 定义 std::future_error 报告的错误代码
    enum class future_errc {
    broken_promise = 1,           // 与该 std::future 共享状态相关联的 std::promise 对象在设置值或者异常之前已被销毁。
    future_already_retrieved,     // 共享状态的内容已通过 std::future 访问
    promise_already_satisfied,    // 重复设置共享状态的值
    no_state                      // 试图访问没有关联共享状态的 std::promise 或 std::future。
    };
    ```

### std::shared_future

- 与 std::future 类似，但是 std::shared_future 可以拷贝。即 shared_future 支持拷贝语义和移动语义

- 多个 std::shared_future 可以共享某个共享状态的最终结果（即共享状态的某个值或者异常）

- shared_future 可以通过某个 std::future 对象隐式转换（即存在以 future 对象作为输入参数的构造函数），或者通过 std::future::share() 显示转换。无论哪种转换，被转换的那个 std::future 对象都会变为 not-valid。

- 成员函数

    ```cpp
    bool valid();                                     // 类似于 future 类的 valid
    T get();                                          // 类似于 future 类的 get 
    void wait();                                      // 类似于 future 类的 wait
    std::future_status wait_for(duration &time);      // 类似于 future 类的 wait_for
    std::future_status wait_until(time_point &time);  // 类似于 future 类的 wait_until
    ```

### std::condition_variable

- condition_variable 需要配合unique_lock 一起使用

- 成员函数

    - 调用 wait 时，必须保证互斥量已经被 unique_lock 上锁，此时 wait 函数会阻塞本线程，并且将互斥量解锁。

    - 被 wait 函数调用阻塞后，下一次线程被唤醒时，将会尝试对互斥量上锁，随后执行 wait 函数之后的代码。若尝试对互斥量上锁而互斥量已经被其他线程锁住时，本线程将再次被阻塞，但阻塞原因是 互斥量的 lock 函数调用，而不再是被条件变量阻塞，即线程会从等待条件变量的队列中转移到等待互斥锁的队列中。

        ```cpp
        void wait(std::unique_lock<std::mutex> &lck);

        // equals with: while (!pred()) wait(lck);
        void wait(std::unique_lock<std::mutex> &lck, _Predicate pred);
        ```

    - waitfor、waituntil 类似于 wait 函数，当前线程收到通知或者指定的时间 time 超时之前，该线程都会处于阻塞状态。

    - 一旦超时或者收到了其他线程的通知，wait_for 返回，剩下的处理步骤和 wait 类似。

    - 若函数没有超时，返回 cv_status::no_timeout，否则返回 cv_status::timeout。

        ```cpp
        std::cv_status wait_for(unique_lock<mutex> &lck, const chrono::duration &time);
        bool wait_for(unique_lock<mutex> &lck, const chrono::duration &time, _Predicate pred);

        std::cv_status wait_until(unique_lock<mutex> &lck, const chrono::time_point &time);
        bool wait_until(unique_lock<mutex> &lck, const chrono::time_point &time, _Predicate pred);
        ```

    - notify_one：唤醒某个等待线程。如果当前没有等待线程，则该函数什么也不做，如果同时存在多个等待线程，则唤醒哪个线程是不确定的。该函数的语义是只唤醒一个线程，但操作系统底层的实现上某些场景下该函数唤醒的线程可能不止一个。

    - notify_all：唤醒所有的等待线程。如果当前没有等待线程，则该函数什么也不做。

        ```cpp
        void notify_one();
        void notify_all();
        ```
    
    - native_handle：访问\*this的原生句柄，此函数结果的含义和类型是实现定义的。 POSIX 系统上，这可以是 pthread_cond_t\* 类型值。 Windows 系统上，这可以是 PCONDITION_VARIABLE。

        ```cpp
        std::condition_variable::native_handle_type native_handle();
        ```

### std::condition_variable_any

- 与 std::condition_variable 类似，只不过 std::condition_variable_any 的 wait 函数可以接受任何 lockable 参数，而 std::condition_variable 只能接受 std::unique_lock\<std::mutex> 类型的参数，除此以外，和 std::condition_variable 几乎完全一样。

### std::notify_all_at_thread_exit

```cpp
// 当调用该函数的线程退出时，所有在 cond 条件变量上等待的线程都会收到通知。
void notify_all_at_thread_exit (condition_variable& cond, unique_lock<mutex> lck);
```

### 信号丢失、虚假唤醒与惊群效应

```cpp
std::mutex mutex;
std::condition_variable cv;
std::vector<int> vec;

void Consume() {
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock);
  std::cout << "consume " << vec.size() << "\n";
}
void Produce() {
  std::unique_lock<std::mutex> lock(mutex);
  vec.push_back(1);
  cv.notify_all();
  std::cout << "produce \n";
}
int main() {
  std::thread t(Consume);
  t.detach();
  Produce();
  return 0;
}
```

- 上述代码本意是消费者线程阻塞，等待生产者生产数据后去通知消费者线程，这样消费者线程就可以拿到数据去消费。

- **BUG**：信号丢失。如果先执行的 Produce()，后执行的 Consume()，生产者提前生产出了数据，去通知消费者，但是此时消费者线程如果还没有执行到 wait 语句，即线程还没有处于挂起等待状态，线程没有等待此条件变量上，那通知的信号就丢失了， 后面 Consume() 中才执行 wait 处于等待状态，但此时生产者已经不会再触发 notify，那消费者线程就会始终阻塞下去，出现信号丢失 bug。

```cpp
// 修改 Consume 函数如下，可以解决信号丢失 bug
void Consume() {
  std::unique_lock<std::mutex> lock(mutex);
  if (vec.empty()) {  // 加入此判断条件
    cv.wait(lock);
  }
  std::cout << "consume " << vec.size() << "\n";
}
```

- 通过增加附加条件可以解决信号丢失的问题。

- **BUG**：虚假唤醒。处于等待的条件变量可以通过 notify_one/notify_all 进行唤醒（注意notify_one可能也会唤醒多个线程），调用函数进行信号的唤醒时，处于等待的条件变量会重新进行互斥锁的竞争。没有得到互斥锁的线程就会发生等待转移，从等待条件变量的队列中转移到等待互斥锁的队列中，一旦获取到互斥锁的所有权就会接着向下执行，但是此时其他线程可能已经执行并重置了执行条件，这时该线程执行就可能引发未定义的错误。

```cpp
// 修改 Consume 函数如下，可以解决虚假唤醒 bug
void Consume() {
  std::unique_lock<std::mutex> lock(mutex);
  while (vec.empty()) {  // 加入此判断条件
    cv.wait(lock);
  }
  std::cout << "consume " << vec.size() << "\n";
}

// 可以使用 wait 的重载函数简化代码
void Consume() {
  std::unique_lock<std::mutex> lock(mutex);
  cv.wait(lock, [&]() { return !vec.empty(); });
  std::cout << "consume " << vec.size() << "\n";
}
```

- 惊群效应：多个进程/线程等待一个唤醒的情况叫做惊群效应。

TODO
- nginx解决惊群效应的方法：

### std::atomic_flag

- std::atomic_flag：是一种简单的原子布尔类型，只支持两种操作，test_and_set 和 clear。

- std::atomic_flag 只有默认构造函数，禁用拷贝语义和移动语义

- 如果在初始化时没有明确使用 ATOMIC_FLAG_INIT 初始化，那么新创建的 std::atomic_flag 对象的状态是未指定的（既没有被 set 也没有被 clear）

- 如果某个 std::atomic_flag 对象使用 ATOMIC_FLAG_INIT 宏初始化，那么可以保证该 std::atomic_flag 对象在创建时处于 clear 状态

- test_and_set() 函数检查 std::atomic_flag 标志，并返回先前该 std::atomic_flag 对象是否被设置过。无论std::atomic_flag之前有没有被设置过，都设置 std::atomic_flag 的标志为true。

- test-and-set 操作是原子的（因此 test-and-set 是原子 read-modify-write （RMW）操作）。

- test_and_set 可以使用 order 参数指定内存序。

- clear 函数清除 std::atomic_flag 对象的标志位，即设置 atomic_flag 的值为 false。

- clear 函数可以使用 order 参数指定内存序。

    ```cpp
    bool test_and_set(std::memory_order order = std::memory_order_seq_cst);
    void clear(std::memory_order order = std::memory_order_seq_cst);
    ```

### std::atomic

- std::atomic 是模板类，一个模板类型为 T 的原子对象中封装了一个类型为 T 的值。

- C++11 标准库 std::atomic 提供了针对整形 (integral) 和指针类型的特化实现

- 禁用拷贝语义和移动语义

- 成员函数

```cpp
// 赋值操作符，允许使用模板类型 T 的变量赋值给 atomic<T> 类型原子变量。
T operator= (T val);

// 判断该 std::atomic 对象是否具备 lock-free 的特性。
bool is_lock_free();

// 修改被封装的值，可以指定内存序。
void store(int value, std::memory_order order = std::memory_order_seq_cst);

// 读取被封装的值，可以指定内存序。
T load(memory_order order = std::memory_order_seq_cst);

// 与load功能类似，也是读取被封装的值，operator T() 是类型转换操作。
operator T() const;

// 读取并修改被封装的值，exchange 会使用 val 替换掉之前该原子对象封装的值，并返回之前该原子对象封装的值，整个过程是原子的 (因此 exchange 操作也称为 read-modify-write 操作)。order 参数指定内存序。
T exchange(T val, memory_order order = std::memory_order_seq_cst);

bool compare_exchange_weak(T& expected, T val, memory_order order = std::memory_order_seq_cst) noexcept;

bool compare_exchange_weak(T& expected, T val, memory_order success, memory_order failure) noexcept;

bool compare_exchange_strong(T& expected, T val, memory_order order = std::memory_order_seq_cst) noexcept;

bool compare_exchange_strong(T& expected, T val, memory_order success, memory_order failure) noexcept;
```

- compare_exchange_weak和compare_exchange_strong原子性地实现如下功能

    - 比较并交换被封装的值与参数 expected 所指定的值是否相等

    - 如果相等，则用 val 替换原子对象的旧值

    - 如果不相等，则用原子对象的旧值替换 expected

    - 这些函数返回原子对象的旧值和expected是否相等。

    - compare_exchange_weak可能发生伪失败。正常情况下，比较物理内容相等时会返回 true，不相等时会返回 false，但某些时候比较物理内容相等时也会返回 false，此时函数返回 false，并且参数 expected 的值不会改变。伪失败的解决方法是循环调用该函数直至不再出现伪失败。

    - compare_exchange_weak 在一些平台下性能优于 compare_exchange_strong 函数。如果算法本身需要循环操作来做检查， compare_exchange_weak 的性能会优于 compare_exchange_strong。

    - order 指定内存序，success 指定比较物理内容相等时的内存序，failure 指定比较物理内容不相等时的内存序。

    - compare_exchange_strong 保证不会产生伪失败。

    - 对于某些不需要采用循环操作的算法而言, 通常采用 compare_exchange_strong 优于 compare_exchange_weak。

- std::atomic 针对整型和指针类型的特化版本新增的函数

    ```cpp
    // 将原子对象的封装值加 val，并返回原子对象的旧值（适用于整形和指针类型的 std::atomic 特化版本）
    T fetch_add(T val, memory_order order = std::memory_order_seq_cst);
    // 类似于 fetch_add，执行减法操作。
    T fetch_sub(T val, memory_order order = std::memory_order_seq_cst);
    // 将原子对象的封装值按位与 val，并返回原子对象的旧值（只适用于整型的 std::atomic 特化版本）
    T fetch_and(T val, memory_order order = std::memory_order_seq_cst);
    // 类似于 fetch_and，执行按位或操作。
    T fetch_or (T val, memory_order order = std::memory_order_seq_cst);
    // 类似于 fetch_and，执行按位异或操作。
    T fetch_xor (T val, memory_order order = std::memory_order_seq_cst);

    // 自增自减运算符，适用于整形和指针类型的 std::atomic 特化版本。
    T operator++();     // 注意不是T&
    T operator++ (int);
    T operator--();     // 注意不是T&
    T operator-- (int);

    // 复合赋值运算符，都有对应的 fetch 操作，内存序为默认内存序。
    // if T is integral
    T operator+= (T val) volatile noexcept;
    T operator+= (T val) noexcept;
    T operator-= (T val) volatile noexcept;
    T operator-= (T val) noexcept;
    T operator&= (T val) volatile noexcept;
    T operator&= (T val) noexcept;
    T operator|= (T val) volatile noexcept;
    T operator|= (T val) noexcept;
    T operator^= (T val) volatile noexcept;
    T operator^= (T val) noexcept;

    // if T is pointer	
    T operator+= (ptrdiff_t val) volatile noexcept;
    T operator+= (ptrdiff_t val) noexcept;
    T operator-= (ptrdiff_t val) volatile noexcept;
    T operator-= (ptrdiff_t val) noexcept;
    ```

### 并行算法库

- 从C++17开始。\<algorithm> 和 \<numeric> 头文件的中的很多算法都添加了一个新的参数：sequenced_policy。借助这个参数，开发者可以直接使用这些算法的并行版本，不用再自己创建并发系统和划分数据来调度这些算法。

    ```cpp
    std::vector<int> vc;
    std::sort(std::execution::par, vc.begin(), vc.end());
    ```

### 内存序

- 编译器在生成指令时可能会对代码做了重排，因为对编译器来说某些赋值操作没有任何联系

- CPU在执行时也可能做了指令重排

- 以下代码可能断言失败

  ```cpp
  int x = 0;
  int y = 0;

  void threadA() {
    x = 1;
    y = 2;
  }

  void threadB() {
    if (y == 2) {
      assert(x == 1);
    }
  }
  ```

- 内存序可以对指令的重排程序进行约束

  ```cpp
  typedef enum memory_order {
    memory_order_relaxed,
    memory_order_consume,
    memory_order_acquire,
    memory_order_release,
    memory_order_acq_rel,
    memory_order_seq_cst
  } memory_order;
  ```

  - memory_order_relaxed: 不对指令重排进行限制

  - memory_order_release: 线程中此屏障之前的读写指令不能重排到此屏障之后

  - memory_order_acquire: 线程中此屏障之后的读写指令不能重排到此屏障之前

  - memory_order_consume: 与memory_order_acquire的功能相似，而且大多数编译器并没有实现这个屏障

  - memory_order_acq_rel: 双向读写内存屏障，相当于结合了memory_order_release、memory_order_acquire。

  - memory_order_seq_cst: 最严格的限制，在memory_order_acq_rel的基础上，一个线程中两个此屏障之间的读写指令不能重排到这两个此屏障之外的区域。
