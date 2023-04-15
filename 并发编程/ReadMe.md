#### linux并发编程

- 线程模型: 熟悉三种线程模型以及各自优缺点

- linux上的线程库: 了解管程、理解linux进程和线程的区别和联系

- pthread: 熟悉常用api，知道可以设置线程的栈顶位置，栈大小，栈警戒区大小、detach、调度优先级、调度策略、是否继承父线程的调度属性等

- posix信号量: 熟悉常用api，知道可以设置进程共享

- 互斥锁: 熟悉常用api，知道可以设置进程共享，递归锁和检错锁

- 读写锁: 熟悉常用api，了解读锁和写锁的工作原理

- 条件变量: 熟悉常用api

- 自旋锁: 熟悉常用api，了解自旋锁的工作原理、优缺点以及适用场景

- 屏障: 立即屏障的作用，熟悉常用api

- 多线程环境: 熟悉线程安全、可重入、异步信号三者的定义。掌握不可重入的常见原因。掌握 "可重入函数必然是线程安全函数和异步信号安全函数,线程安全函数不一定是可重入函数。"

#### C++并发编程

- 熟悉std::this_thread::get_id()、sleep_for()、sleep_until()、yield()

- 熟悉thread_local、了解其实现原理

- 熟悉std::thread使用，如何绑定类成员函数

  ```cpp
  Object obj;
  std::thread t(&Object::fn, &obj, /* args for fn */);
  ```

- 熟悉mutex、recursive_mutex、time_mutex、recursive_timed_mutex、shared_mutex、shared_timed_mutex互斥量

- 熟悉lock_guard、unique_lock、shared_lock、scoped_lock互斥量RAII封装类

- 熟悉全局函数std::lock、std::try_lock

- 熟悉std::once_flag 与 std::call_once

- 熟悉std::promise、std::packaged_task、std::async、std::future。了解std::shared_future

- 熟悉std::condition_variable、std::condition_variable_any。了解全局函数std::notify_all_at_thread_exit

- 熟悉信号丢失、虚假唤醒与惊群效应的定义及解决方式。了解惊群效应及nginx解决惊群效应的方法。

- 熟悉std::atomic_flag，熟悉testAndSet语义。

- 熟悉std::atomic，熟悉compareAndSet语义，熟悉compare_exchange_weak和compare_exchange_strong。了解伪失败及其解决方法。

- 了解并行算法库的概念

- 熟悉六种c++11内存序。了解编译期指令重排、CPU乱序执行。

#### 并发编程八股

- 熟悉乐观锁定义、适用场景、优缺点、两种实现方式的原理。

- 熟悉悲观锁定义、适用场景、优缺点。

- 熟悉CAS定义、适用场景、优缺点。

- 熟悉自旋锁定义、适用场景、优缺点。

- 熟悉原子操作的汇编支持

- 熟悉Read copy Update的概念、实现原理，适用场景，优缺点，相比读写锁所具有的优势。

- 熟悉Copy on Write的概念、在RCU中的应用。