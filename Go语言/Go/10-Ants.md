## sync.Locker

```go
type Locker interface {
    Lock()
    unlock()
}
```

## Ants.spinLock

```go
type spinLock uint32
const maxBackoff = 16

func (sl *spinLock) Lock() {
    backoff := 1
    for !atomic.CompareAndSwapUint32((*uint32)(sl), 0, 1) {
        for i := 0; i < backoff; i++ {
            runtime.Gosched()
        }
        if backoff < maxBackoff {
            backoff <<= 1
        }
    }
}

func (sl *spinLock) UnLock() {
    atomic.StoreUint32((*uint32)(sl), 0)
}
```

## sync.Pool

```go
type Pool struct {
    noCopy noCopy
    local unsafe.Pointer
    localSize uintptr
    victim unsafe.Pointer
    victimSize uintptr
    New func()
}

type poolLocal struct {
    poolLocalInternal
}

type poolLocalInternal struct {
    private any       // Can be used only by the respective P.
    shared  poolChain // Local P can pushHead/popHead; any P can popTail.
}
```

- noCopy 防拷贝标志

- local 类型为 [P]poolLocal 的数组，数组容量 P 为 goroutine 处理器 P 的个数

- victim 为经过一轮 gc 回收，暂存的上一轮 local

- New 为用户指定的工厂函数，当 Pool 内存量元素不足时，会调用该函数构造新的元素

- poolLocal 为 Pool 中对应于某个 P 的缓存数据

- poolLocalInternal.private：对应于某个 P 的私有元素，操作时无需加锁

- poolLocalInternal.shared: 某个 P 下的共享元素链表，由于各 P 都有可能访问，因此需要加锁


## 核心方法

- Pool.pin

    ```go
    func (p *Pool) pin() (*poolLocal, int) {
        pid := runtime_procPin()
        s := runtime_LoadAcquintptr(&p.localSize) // load-acquire
        l := p.local                              // load-consume
        if uintptr(pid) < s {
            return indexLocal(l, pid), pid
        }
        return p.pinSlow()
    }
    ```

    - pin 方法内部通过 native 方法 runtime_procPin 取出当前 P 的 index，并且将当前 goroutine 与 P 进行绑定，短暂处于不可抢占状态

    - 如果是首次调用 pin 方法，则会走进 pinSlow 方法

    - 在pinSlow 方法中，会完成 Pool.local 的初始化，并且将当前 Pool 添加到全局的 allPool 数组中，用于 gc 回收

- Pool.Get

    ```go
    func (p *Pool) Get() any {
        l, pid := p.pin()
        x := l.private
        l.private = nil
        if x == nil {
            x, _ = l.shared.popHead()
            if x == nil {
                x = p.getSlow(pid)
            }
        }
        runtime_procUnpin()
        if x == nil && p.New != nil {
            x = p.New()
        }
        return x
    }
    ```

    - 调用 Pool.pin 方法，绑定当前 goroutine 与 P，并且取得该 P 对应的缓存数据

    - 尝试获取 P 缓存数据的私有元素 private

    - 倘若前一步失败，则尝试取 P 缓存数据中共享元素链表的头元素

    - 倘若前一步失败，则走入 Pool.getSlow 方法，尝试取其他 P 缓存数据中共享元素链表的尾元素

    - 同样在 Pool.getSlow 方法中，倘若前一步失败，则尝试从上轮 gc 前缓存中取元素（victim）

    - 调用 native 方法解绑 当前 goroutine 与 P

    - 倘若（2）-（5）步均取值失败，调用用户的工厂方法，进行元素构造并返回

- Pool.Put

    ```go
    // Put adds x to the pool.
    func (p *Pool) Put(x any) {
        if x == nil {
            return
        }
        l, _ := p.pin()
        if l.private == nil {
            l.private = x
        } else {
            l.shared.pushHead(x)
        }
        runtime_procUnpin()
    }
    ```

    - 判断存入元素 x 非空

    - 调用 Pool.pin 绑定当前 goroutine 与 P，并获取 P 的缓存数据

    - 倘若 P 缓存数据中的私有元素为空，则将 x 置为其私有元素

    - 倘若未走入（3）分支，则将 x 添加到 P 缓存数据共享链表的末尾

    - 解绑当前 goroutine 与 P

- 回收机制

    ```go
    func init() {
        runtime_registerPoolCleanup(poolCleanup)
    }

    func poolCleanup() {
        for _, p := range oldPools {
            p.victim = nil
            p.victimSize = 0
        }

        for _, p := range allPools {
            p.victim = p.local
            p.victimSize = p.localSize
            p.local = nil
            p.localSize = 0
        }

        oldPools, allPools = allPools, nil
    }
    ```

    - 存入 pool 的对象会不定期被 go 运行时回收，因此 pool 没有容量概念，即便大量存入元素，也不会发生内存泄露

    - 每个 Pool 首次执行 Get 方法时，会在内部首次调用 pinSlow 方法内将该 pool 添加到全局的 allPools 数组中

    - 每次 gc 时，会将上一轮的 oldPools 清空，并将本轮 allPools 的元素赋给 oldPools，allPools 置空

    - 新置入 oldPools 的元素统一将 local 转移到 victim，并且将 local 置为空

    - 综上可以得见，最多两轮 gc，pool 内的对象资源将会全被回收

## Ants

- 为什么用协程池

    - 提升性能

    - 有一个并发资源控制的概念

    - 协程生命周期控制

- goWorker

    ```go
    type goWorker struct {
        pool *Pool
        task chan func()
        recycleTime time.Time
    }
    ```

    - goWorker 可以简单理解为一个长时间运行而不回收的协程，用于反复处理用户提交的异步任务

    - pool：goWorker 所属的协程池

    - task：goWorker 用于接收异步任务包的管道

    - recycleTime：goWorker 回收到协程池的时间

- Ants.Pool

    ```go
    type Pool struct {
        capacity int32
        running int32
        lock sync.Locker
        workers workerArray
        state int32
        cond *sync.Cond
        workerCache sync.Pool
        waiting int32
        heartbeatDone int32
        stopHeartbeat context.CancelFunc
        options *Options
    }
    ```

    - capacity：池子的容量

    - running：运行中的协程数量

    - lock：自制的自旋锁，保证取 goWorker 时并发安全

    - workers：goWorker 列表，即“真正意义上的协程池”

    - state：池子状态标识，0-打开；1-关闭

    - cond：并发协调器，用于阻塞模式下，挂起和唤醒等待资源的协程

    - workerCache：存放 goWorker 的对象池，用于缓存释放的 goworker 资源用于复用. 对象池需要区别于协程池，协程池中的 goWorker 仍存活，进入对象池的 goWorker 严格意义上已经销毁

    - waiting：标识处于等待状态的协程数量

    - heartbeatDone：标识回收协程是否关闭

    - stopHeartbeat：用于关闭回收协程的控制器函数

    - options：一些定制化的配置

- options

    ```go
    type Options struct {
        DisablePurge bool
        ExpiryDuration time.Duration
        MaxBlockingTasks int
        Nonblocking bool
        PanicHandler func(interface{})
    }
    ```

    - DisablePurge：是否允许回收空闲 goWorker

    - ExpiryDuration: 空闲 goWorker 回收时间间隔；仅当 DisablePurge 为 false 时有效

    - Nonblocking：是否设置为非阻塞模式，若是，goWorker 不足时不等待，直接返回 err

    - MaxBlockingTasks：阻塞模式下，最多阻塞等待的协程数量；仅当 Nonblocking 为 false 时有效

    - PanicHandler：提交任务发生 panic 时的处理逻辑

- workerArray

    ```go
    type workerArray interface {
        len() int
        isEmpty() bool
        insert(worker *goWorker) error
        detach() *goWorker
        retrieveExpiry(duration time.Duration) []*goWorker
        reset()
    }

    // stack version.
    type workerStack struct {
        items  []*goWorker
        expiry []*goWorker
    }

    func newWorkerStack(size int) *workerStack {
        return &workerStack{
            items: make([]*goWorker, 0, size),
        }
    }

    func (wq *workerStack) len() int {
        return len(wq.items)
    }

    func (wq *workerStack) isEmpty() bool {
        return len(wq.items) == 0
    }

    func (wq *workerStack) insert(worker *goWorker) error {
        wq.items = append(wq.items, worker)
        return nil
    }

    func (wq *workerStack) detach() *goWorker {
        l := wq.len()
        if l == 0 {
            return nil
        }

        w := wq.items[l-1]
        wq.items[l-1] = nil // avoid memory leaks
        wq.items = wq.items[:l-1]
        return w
    }

    func (wq *workerStack) retrieveExpiry(duration time.Duration) []*goWorker {
        n := wq.len()
        if n == 0 {
            return nil
        }

        expiryTime := time.Now().Add(-duration)
        index := wq.binarySearch(0, n-1, expiryTime)

        wq.expiry = wq.expiry[:0]
        if index != -1 {
            wq.expiry = append(wq.expiry, wq.items[:index+1]...)
            m := copy(wq.items, wq.items[index+1:])
            for i := m; i < n; i++ {
                wq.items[i] = nil
            }
            wq.items = wq.items[:m]
        }
        return wq.expiry
    }

    func (wq *workerStack) binarySearch(l, r int, expiryTime time.Time) int {
        var mid int
        for l <= r {
            mid = (l + r) / 2
            if expiryTime.Before(wq.items[mid].recycleTime) {
                r = mid - 1
            } else {
                l = mid + 1
            }
        }
        return r
    }
    ```

    - workerArray 是一个 interface，其实现包含 stack 栈版本和 queue 队列包含

    - 该 interface 主要定义了作为数据集合的几个通用 api，以及用于回收过期 goWorker 的 api

    - items：存放的 goWorker 列表

    - expire：用于临时存放已过期的 goWorker 集合

- pool 构造器方法

    ```go
    func NewPool(size int, options ...Option) (*Pool, error) {
        opts := loadOptions(options...)
        // 读取用户配置，做一些前置校验，默认值赋值等前处理动作...

        p := &Pool{
            capacity: int32(size),
            lock:     internal.NewSpinLock(),
            options:  opts,
        }
        p.workerCache.New = func() interface{} {
            return &goWorker{
                pool: p,
                task: make(chan func(), workerChanCap),
            }
        }

        p.workers = newWorkerArray(stackType, 0)
        p.cond = sync.NewCond(p.lock)

        var ctx context.Context
        ctx, p.stopHeartbeat = context.WithCancel(context.Background())
        go p.purgePeriodically(ctx)
        return p, nil
    }
    ```

    - 读取用户传的配置参数，做一些校验和默认赋值的前处理动作

    - 构造好 Pool 数据结构

    - 构造好 goWorker 对象池 workerCache，声明好工厂函数

    - 构造好 Pool 内部的 goWorker 列表

    - 构造好 Pool 的并发协调器 cond

    - 异步启动 goWorker 过期销毁协程

- pool 提交任务

    ```go
    func (p *Pool) Submit(task func()) error {
        var w *goWorker
        if w = p.retrieveWorker(); w == nil {
            return ErrPoolOverload
        }
        w.task <- task
        return nil
    }

    func (p *Pool) retrieveWorker() (w *goWorker) {
        spawnWorker := func() {
            w = p.workerCache.Get().(*goWorker)
            w.run()
        }

        p.lock.Lock()

        w = p.workers.detach()
        if w != nil { 
            p.lock.Unlock()
        } else if capacity := p.Cap(); capacity == -1 || capacity > p.Running() {
            p.lock.Unlock()
            spawnWorker()
        } else { 
            if p.options.Nonblocking {
                p.lock.Unlock()
                return
            }
        retry:
            if p.options.MaxBlockingTasks != 0 && p.Waiting() >= p.options.MaxBlockingTasks {
                p.lock.Unlock()
                return
            }
            p.addWaiting(1)
            p.cond.Wait() // block and wait for an available worker
            p.addWaiting(-1)

            var nw int
            if nw = p.Running(); nw == 0 { // awakened by the scavenger
                p.lock.Unlock()
                spawnWorker()
                return
            }
            if w = p.workers.detach(); w == nil {
                if nw < p.Cap() {
                    p.lock.Unlock()
                    spawnWorker()
                    return
                }
                goto retry
            }
            p.lock.Unlock()
        }
        return
    }
    ```

    - 声明了一个构造 goWorker 的函数 spawnWorker 用于兜底，内部实际上是从对象池 workerCache 中获取 goWorker

    - 接下来的核心逻辑就是加锁，然后尝试从池子中取出 goWorker 执行任务

    - 倘若池子容量超限，且池子为阻塞模式，则基于并发协调器 cond 挂起协程阻塞等待

    - 倘若池子容量超限，且池子为非阻塞模式，直接抛回错误

    - 倘若池子容量未超限，且未取到 goWorker，调用 spawnWorker 构造新的 goWorker 用于执行任务

- goWorker 运行

    ```go
    func (w *goWorker) run() {
        w.pool.addRunning(1)
        go func() {
            defer func() {
                w.pool.addRunning(-1)
                w.pool.workerCache.Put(w)
                if p := recover(); p != nil {
                    // panic 后处理
                }
                w.pool.cond.Signal()
            }()

            for f := range w.task {
                if f == nil {
                    return
                }
                f()
                if ok := w.pool.revertWorker(w); !ok {
                    return
                }
            }
        }()
    }
    ```

    - 循环 + 阻塞等待，直到获取到用户提交的异步任务包 task 并执行

    - 执行完成 task 后，会将自己交还给协程池

    - 倘若回归协程池失败，或者用户提交了一个空的任务包，则该 goWorker 会被销毁，销毁方式是将自身放回协程池的对象池 workerCache. 并且会调用协调器 cond 唤醒一个阻塞等待的协程.

- pool 回收协程

    ```go
    // revertWorker puts a worker back into free pool, recycling the goroutines.
    func (p *Pool) revertWorker(worker *goWorker) bool {
        worker.recycleTime = time.Now()
        p.lock.Lock()
        err := p.workers.insert(worker)
        if err != nil {
            p.lock.Unlock()
            return false
        }

        p.cond.Signal()
        p.lock.Unlock()
        return true
    }
    ```

    - 回收时更新 goWorker 回收时间，用于 goWorker 的定期清理

    - 加锁后，将 goWorker 添加回协程池

    - 通过协调器 cond 唤醒下一个阻塞等待的协程，并解锁

- 定期回收过期 goWorker

    ```go
    func (p *Pool) purgePeriodically(ctx context.Context) {
        heartbeat := time.NewTicker(p.options.ExpiryDuration)
        defer func() {
            heartbeat.Stop()
            atomic.StoreInt32(&p.heartbeatDone, 1)
        }()

        for {
            select {
            case <-heartbeat.C:
            case <-ctx.Done():
                return
            }

            if p.IsClosed() {
                break
            }

            p.lock.Lock()
            expiredWorkers := p.workers.retrieveExpiry(p.options.ExpiryDuration)
            p.lock.Unlock()

            for i := range expiredWorkers {
                expiredWorkers[i].task <- nil
                expiredWorkers[i] = nil
            }

            if p.Running() == 0 || (p.Waiting() > 0 && p.Free() > 0) {
                p.cond.Broadcast()
            }
        }
    }
    ```

    - purgePeriodically 方法开启了一个 ticker，按照用户预设的过期时间间隔轮询回收过期的 goWorker

    - 回收的方式是往对应 goWorker 的 channel 中注入一个空值，goWorker 将会自动将自身放回协程池的对象池 workerCache 当中

    - 倘若当前存在空闲的 goWorker 且有协程阻塞等待，会唤醒所有阻塞协程