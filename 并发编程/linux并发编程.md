### 线程模型

- M:1线程模型：M个用户态线程对应一个内核线程，内核线程无法感知用户态线程的存在，用户态线程之间的切换完全由用户层实现，例如setjmp，longjmp等方式。优点是用户线程切换无需进入内核态，开销很小，缺点是无法充分利用多处理器的特性，而且一个用户线程的阻塞可能会导致整个进程都被阻塞。

- 1:1线程模型：每个用户态线程都对应一个内核线程，优点是充分利用了多处理器，缺点是线程切换开销变大。

- M:N线程模型：M个用户态线程对应到N个内核线程，M>=N，是一种折中方案。


### linux上的线程库

- LinuxThreads
  
  - 采用1对1模型。
  
  - 内核线程是用clone系统调用创建的进程模拟的。
  
  - clone系统调用和fork类似，创建调用进程的子进程。不过可以为clone调用指定CLONE_THREAD标志，此时创建的子进程与调用进程共享相同的虚拟地址空间、文件描述符和信号处理函数，这些都是线程的特点，但用进程模拟内核线程会导致很多语义问题，例如：

    - 每个线程拥有不同的PID，因此不符合POSIX规范

    - Linux信号处理本来是基于进程的，但现在一个进程内部的所有线程都能而且必须处理信号

    - 用户ID、组ID对一个进程中的不同线程来说可能是不一样的

    - 程序产生的核心转储文件不会包含所有线程的信息，而只包含产生该核心转储文件的线程的信息。

    - 由于每个线程都是一个进程，因此系统允许的最大进程数也就是最大线程数

  - LinuxThreads线程库一个有名的特性是所谓的管理线程。它是进程中专门用于管理其他工作线程的线程。管理线程的引入额外增加了系统开销。

- NPTL

  - 真正的内核线程库，内核线程不再是一个进程，因此避免了很多用进程模拟内核线程导致的语义问题。

  - 摒弃了管理线程，终止线程，回收线程堆栈等工作都可以由内核来完成。

  - 由于不存在管理线程，所以一个进程的线程可以运行在不同的CPU上，从而充分利用了多处理器的优势。

  - 线程的同步由内核完成。隶属于不同进程的线程之间也能共享互斥锁，因此可实现跨进程的线程同步。


### 创建线程和结束线程

```cpp
#include <pthread.h>

int pthread_create(pthread_t* thread, const pthread_attr_t* attr, void*(*start_routine)(void*), void* arg);
void pthread_exit(void* retval);

pthread_t pthread_self(void);
int pthread_equal(pthread_t tid1, pthread_t tid2);

int pthread_join(pthread_t thread, void** retval);
int pthread_cancel(pthread_t thread);
int pthread_detach(pthread_t tid);

void pthread_cleanup_push(void (*rtn)(void *), void *arg);
void pthread_cleanup_pop(int execute);

int pthread_setcancelstate(int state, int* oldstate); // PTHREAD_CANCEL_ENABLE(允许取消)，PTHREAD_CANCEL_DISABLE(禁止取消)
int pthread_setcanceltype(int type, int* oldstate);   // PTHREAD_CANCEL_ASUNCHRONOUS(线程随时都可以被取消)，PTHREAD_CANCEL_DEFERRED(线程运行到取消点才会检查自己是否被取消)
```

### 线程属性

```cpp
#include <bits/pthreadtypes.h>

#define __SIZEOF_PTHREAD_ATTR_T 36
typedef union {
  char __size[__SIZEOF_PTHREAD_ATTR_T];
  long int __align;
} pthread_attr_t;

#include <pthread.h>

int pthread_attr_init(pthread_attr_t* attr);
int pthread_attr_destory(pthread_attr_t* attr);

int pthread_attr_getdetachstate(const pthread_attr_t* attr, int* detachstate);
int pthread_attr_setdetachstate(pthread_attr_t* attr, int detachstate);

int pthread_attr_getstackaddr(const pthread_attr_t* attr, void** stackaddr);
int pthread_attr_setstackaddr(pthread_attr_t* attr, void* stackaddr);

int pthread_attr_getstacksize(const pthread_attr_t* attr, size_t* stacksize);
int pthread_attr_setstacksize(pthread_attr_t* attr, size_t stacksize);

int pthread_attr_getstack(const pthread_attr_t* attr, void** stackaddr, size_t* stacksize);
int pthread_attr_setstack(pthread_attr_t* attr, void* stackaddr, size_t stacksize);

int pthread_attr_getguardsize(const pthread_attr_t* attr, size_t* guardsize);
int pthread_attr_setguardsize(pthread_attr_t* attr, size_t guardsize);

// 线程调度参数。目前sched_param仅由一个成员sched_priority
int pthread_attr_getschedparam(const pthread_attr_t* attr, struct sched_param* param);
int pthread_attr_setschedparam(pthread_attr_t* attr, const struct sched_param* param);

// 线程调度策略: SCHED_FIFO/SCHED_RR/SCHED_OTHER(默认值)。rr表示采用轮转算法调度，fifo表示使用先进先出算法调度，这两种方法都具备实时调度功能，但只能用于以超级用户身份运行的进程。
int pthread_attr_getschedpolicy(const pthread_attr_t* attr, int* policy);
int pthread_attr_setschedpolicy(pthread_attr_t* attr, int policy);

// 是否继承创建本线程的线程的调度属性：PTHREAD_INHERIT_SCHED/PTHREAD_EXPLICIT_SHCED
int pthread_attr_getinheritsched(const pthread_attr_t* attr, int* inherit);
int pthread_attr_setinheritsched(pthread_attr_t* attr, int inherit);

// 线程间竞争CPU的范围，即线程优先级的有效范围：PTHREAD_SCOPE_SYSTEM/PTHREAD_SCOPE_PROCESS。前者表示目标线程与系统中所有线程一起竞争CPU，后者表示目标线程仅与其他本进程中的线程竞争CPU，目前linux仅支持PTHREAD_SCOPE_SYSTEM选项。
int pthread_attr_getscope(const pthread_attr_t* attr, int* scope);
int pthread_attr_setscope(pthread_attr_t* attr, int scope);
```

### POSIX信号量

```cpp
#include <semaphore.h>

int sem_init(sem_t* sem, int pshared, unsigned int value);
int sem_destroy(sem_t* sem);
int sem_wait(sem_t* sem);
int sem_trywait(sem_t* sem);
int sem_post(sem_t* sem);
```


### 互斥锁

```cpp
#include <pthread.h>
#include <time.h>

int pthread_mutex_init(pthread_mutex_t* mutex, const pthread_mutexattr_t* mutexattr);
int pthread_mutex_destroy(pthread_mutex_t* mutex);
int pthread_mutex_lock(pthread_mutex_t* mutex);
int pthread_mutex_trylock(pthread_mutex_t* mutex);
int pthread_mutex_unlock(pthread_mutex_t* mutex);

int pthread_mutex_timedlock(pthread_mutex_t *restrict mutex,
                            const struct timespec *restrict tsptr);
// 返回值：若成功，返回0；否则，返回错误编号（超时时返回 ETIMEDOUT）

// 宏初始化
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;


// 互斥锁属性
int pthread_mutexattr_init(pthread_mutexattr_t* attr);
int pthread_mutexattr_destroy(pthread_mutexattr_t* attr);

// 跨进程共享：PTHREAD_PROCESS_SHARED / PTHREAD_PROCESS_PRIVATE
int pthread_mutexattr_getpshared(const pthread_mutexattr_t* attr, int* pshared);
int pthread_mutexattr_setpshared(pthread_mutexattr_t* attr, int pshared);

// 指定互斥锁类型
// PTHREAD_MUTEX_NORMAL: 普通锁，默认类型
// PTHREAD_MUTEX_ERRORCHECK: 检错锁，此互斥量类型提供错误检查。
// PTHREAD_MUTEX_RECURSIVE: 递归锁，允许递归
// PTHREAD_MUTEX_DEFAULT: 默认锁。实现时可能被映射到前面三种锁之一
int pthread_mutexattr_gettype(const pthread_mutexattr_t* attr, int* type);
int pthread_mutexattr_settype(pthread_mutexattr_t* attr, int type);
```


### 读写锁
```cpp
int pthread_rwlock_init(pthread_rwlock_t *restrict rwlock,
                        const pthread_rwlockattr_t *restrict attr);
int pthread_rwlock_destory(pthread_rwlock_t *rwlock);
// 两个函数返回值：若成功，返回0；否则，返回错误编号

int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);
// 三个函数返回值：若成功，返回0；否则，返回错误编号

int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);
// 两个函数返回值：若成功，返回0；否则，返回错误编号

int pthread_rwlock_timedrdlock(pthread_rwlock_t *restrict rwlock,
                               const struct timespec *restrict tsptr);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *restrict rwlock,
                               const struct timespec *restrict tsptr);
// 两个函数返回值：若成功，返回0；否则，返回错误编号
```


### 条件变量

```cpp
#include <pthread.h>

int pthread_cond_init(pthread_cond_t* cond, const pthread_condaddr_t* cond_addr);
int pthread_cond_destory(pthread_cond_t* cond);
int pthread_cond_broadcast(pthread_cond_t* cond);
int pthread_cond_signal(pthread_cond_t* cond);
int pthread_cond_wait(pthread_cond_t* cond, pthread_mutex_t* mutex);
int pthread_cond_timedwait(pthread_cond_t *restrict cond, pthread_mutex_t *restrict mutex,
                           const struct timespec *restrict tsptr);

// 宏初始化
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
```


### 自旋锁

```cpp
#include <pthread.h>
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);
int pthread_spin_destroy(pthread_spinlock_t *lock);
// 两个函数返回值：若成功，返回0；否则，返回错误编号
// pshared参数：设置为 PTHREAD_PROCESS_SHARED 时，自旋锁可以被其他进程访问，设置为 PTHREAD_PROCESS_PRIVATE 时，仅能被本进程内部的线程访问。

int pthread_spin_lock(pthread_spinlock_t *lock);
int pthread_spin_trylock(pthread_spinlock_t *lock);
int pthread_spin_unlock(pthread_spinlock_t *lock);
// 三个函数返回值：若成功，返回0；否则，返回错误编号
```

- 自旋锁与互斥量类似，但它不是通过休眠使进程阻塞，而是在获取锁之前一直处于忙等（自旋）阻塞状态。

- 自旋锁可用于以下情况：锁被持有的时间短，而且线程并不希望在重新调度上花费太多的成本。

- 自旋锁通常作为底层原语用于实现其他类型的锁。可以通过 `test_and_set` 指令有效地实现。

- 自旋锁通常用于内核。


### 屏障
```cpp
#include <pthread.h>
int pthread_barrier_init(pthread_barrier_t *restrict barrier,
                         const pthread_barrierattr_t *restrict attr,
                         unsigned int count);
int pthread_barrier_destory(pthread_barrier_t *barrier);
// 两个函数返回值：若成功，返回0；否则，返回错误编号
// 初始化屏障时，可以使用count参数指定，在允许所有线程继续运行之前，必须到达屏障的线程数目。

int pthread_barrier_wait(pthread_barrier_t *barrier);
// 返回值：若成功，返回0或者PTHREAD_BARRIER_SERIAL_THREAD；否则，返回错误编号
```


### 多线程环境

- 线程安全函数：如果一个函数能被多个线程同时调用而不发生竞态条件，则我们称它是线程安全的。linux库函数只有一小部分是线程不可重入的，一般原因都是因为其内部使用了静态变量。

- 可重入函数：在函数执行过程中，由于信号进入了信号处理程序，在信号处理程序中又调用了该函数，最后无错误发生，则该函数是可重入的。linux库函数中可重入函数很少。

- 异步信号安全函数：异步信号安全函数要么是可重入函数，要么具有原子性质。

- 不可重入的原因：

  - 使用了静态数据结构

  - 调用了malloc或free

  - 调用了标准IO函数，标准IO库的很多实现都以不可重入方式使用全局数据结构

  - 使用了不可重入的锁（例如malloc）

- 可重入函数必然是线程安全函数和异步信号安全函数；线程安全函数不一定是可重入函数。