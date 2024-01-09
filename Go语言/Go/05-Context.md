## 接口定义

```go
type Context interface {
    Deadline() (deadline time.Time, ok bool)

    Done() <-chan struct{}

    Err() error

    Value(key interface{}) interface{}
}

var Canceled = errors.New("context canceled")

var DeadlineExceeded error = deadlineExceededError{}

type deadlineExceededError struct{}

func (deadlineExceededError) Error() string   { return "context deadline exceeded" }
func (deadlineExceededError) Timeout() bool   { return true }
func (deadlineExceededError) Temporary() bool { return true }
```

- Deadline(): 该方法返回一个deadline和标识是否已设置deadline的bool值，如果没有设置deadline，则ok == false，此
时deadline为一个初始值的time.Time值

- Done(): 

    - 该方法返回一个channel，需要在select-case语句中使用，如"case <-context.Done():"

    - 当context关闭后，Done()返回一个被关闭的管道，关闭的管理仍然是可读的，据此goroutine可以收到关闭请求；当context还未关闭时，Done()返回nil

- Err():

    - 该方法描述context关闭的原因。关闭原因由context实现控制，不需要用户设置

    - 因deadline关闭："context deadline exceeded"

    - 因主动关闭: "context canceled"

    - 当context关闭后，Err()返回context的关闭原因；当context还未关闭时，Err()返回nil

- Value():

    - 有一种context，它不是用于控制呈树状分布的goroutine，而是用于在树状分布的goroutine间传递信息

    - Value()方法就是用于此种类型的context，该方法根据key值查询map中的value


## 空context

- context包中定义了一个空的context， 名为emptyCtx，用于context的根节点，空的context只是简单的实现了Context，本身不包含任何值，仅用于其他context的父节点

- context包中定义了一个公用的emptCtx全局变量，名为background，可以使用context.Background()获取

- context包提供了4个方法创建不同类型的context，使用这四个方法时如果没有父context，都需要传入backgroud，即backgroud作为其父节点

    - WithCancel()

    - WithDeadline()

    - WithTimeout()

    - WithValue()

- context包中实现Context接口的struct，除了emptyCtx外，还有cancelCtx、timerCtx和valueCtx三种，正是基于这三种context实例，实现了上述4种类型的context

```go
type Context interface {
	Deadline() (deadline time.Time, ok bool)

	Done() <-chan struct{}

	Err() error

	Value(key any) any
}

type emptyCtx struct{}

type backgroundCtx struct{ emptyCtx }

type todoCtx struct{ emptyCtx }

func (emptyCtx) Deadline() (deadline time.Time, ok bool) {
	return
}

func (emptyCtx) Done() <-chan struct{} {
	return nil
}

func (emptyCtx) Err() error {
	return nil
}

func (emptyCtx) Value(key any) any {
	return nil
}

func Background() Context {
	return backgroundCtx{}
}

func TODO() Context {
	return todoCtx{}
}
```

## cancelCtx

```go
type cancelCtx struct {
	Context

	mu       sync.Mutex            // protects following fields
	done     atomic.Value          // of chan struct{}, created lazily, closed by first cancel call
	children map[canceler]struct{} // set to nil by the first cancel call
	err      error                 // set to non-nil by the first cancel call
	cause    error                 // set to non-nil by the first cancel call
}

type canceler interface {
	cancel(removeFromParent bool, err, cause error)
	Done() <-chan struct{}
}
```

- embed 了一个 context 作为其父 context. 可见，cancelCtx 必然为某个 context 的子 context

- 内置了一把锁，用以协调并发场景下的资源获取

- done：实际类型为 chan struct{}，即用以反映 cancelCtx 生命周期的通道

- children：一个 set，指向 cancelCtx 的所有子 context

- err：记录了当前 cancelCtx 的错误. 必然为某个 context 的子 context

- Deadline 方法: 继承自 Context.

- Done 方法: 懒加载

    ```go
    func (c *cancelCtx) Done() <-chan struct{} {
        d := c.done.Load()
        if d != nil {
            return d.(chan struct{})
        }
        c.mu.Lock()
        defer c.mu.Unlock()
        d = c.done.Load()
        if d == nil {
            d = make(chan struct{})
            c.done.Store(d)
        }
        return d.(chan struct{})
    }
    ```

- Err 方法: 

    ```go
    func (c *cancelCtx) Err() error {
        c.mu.Lock()
        err := c.err
        c.mu.Unlock()
        return err
    }
    ```

- Value 方法:

    ```go
    var cancelCtxKey int

    func (c *cancelCtx) Value(key any) any {
        // 倘若 key 特定值 &cancelCtxKey，则返回 cancelCtx 自身的指针
        if key == &cancelCtxKey {
            return c
        }

        // 否则遵循 valueCtx 的思路取值返回，具体见后文
        return value(c.Context, key)
    }
    ```
- context.WithCancel():

    ```go
    func WithCancel(parent Context) (ctx Context, cancel CancelFunc) {
        if parent == nil { // 校验父 context 非空
            panic("cannot create context from nil parent")
        }
        c := newCancelCtx(parent) // 注入父 context 构造好一个新的 cancelCtx
        propagateCancel(parent, &c) // 保证父 context 终止时，该 cancelCtx 也会被终止
        return &c, func() { c.cancel(true, Canceled) } // 将 cancelCtx 返回，连带返回一个用以终止该 cancelCtx 的闭包函数
    }

    func newCancelCtx(parent Context) cancelCtx {
        return cancelCtx{Context: parent}
    }

    func propagateCancel(parent Context, child canceler) {
        done := parent.Done()
        
        // 倘若 parent 是不会被 cancel 的类型（如 emptyCtx），则直接返回
        if done == nil {
            return // parent is never canceled
        }

        // 倘若 parent 已经被 cancel，则直接终止子 context，并以 parent 的 err 作为子 context 的 err
        select {
        case <-done:
            // parent is already canceled
            child.cancel(false, parent.Err())
            return
        default:
        }

        // 假如 parent 是 cancelCtx 的类型，则加锁，并将子 context 添加到 parent 的 children map
        if p, ok := parentCancelCtx(parent); ok {
            p.mu.Lock()
            if p.err != nil {
                // parent has already been canceled
                child.cancel(false, p.err)
            } else {
                if p.children == nil {
                    p.children = make(map[canceler]struct{})
                }
                p.children[child] = struct{}{}
            }
            p.mu.Unlock()
        } else {
            // 假如 parent 不是 cancelCtx 类型，但又存在 cancel 的能力（比如用户自定义实现的 context）
            // 则启动一个协程，通过多路复用的方式监控 parent 状态，倘若其终止，则同时终止子 context，并传递 parent 的 err.
            atomic.AddInt32(&goroutines, +1)
            go func() {
                select {
                case <-parent.Done():
                    child.cancel(false, parent.Err())
                case <-child.Done():
                }
            }()
        }
    }

    // 校验 parent 是否为 cancelCtx 的类型
    func parentCancelCtx(parent Context) (*cancelCtx, bool) {
        done := parent.Done()
        // 倘若 parent 的 channel 已关闭或者是不会被 cancel 的类型，则返回 false
        if done == closedchan || done == nil {
            return nil, false
        }

        // 倘若以特定的 cancelCtxKey 从 parent 中取值，取得的 value 是 parent 本身，则返回 true. 
        //（基于 cancelCtxKey 为 key 取值时返回 cancelCtx 自身，是 cancelCtx 特有的协议）.
        p, ok := parent.Value(&cancelCtxKey).(*cancelCtx)
        if !ok {
            return nil, false
        }
        pdone, _ := p.done.Load().(chan struct{})
        if pdone != done {
            return nil, false
        }
        return p, true
    }

    func (c *cancelCtx) cancel(removeFromParent bool, err error) {
        if err == nil {
            panic("context: internal error: missing cancel error")
        }
        c.mu.Lock()
        if c.err != nil {
            c.mu.Unlock()
            return // already canceled
        }
        c.err = err
        d, _ := c.done.Load().(chan struct{})
        if d == nil {
            c.done.Store(closedchan)
        } else {
            close(d)
        }
        for child := range c.children {
            // NOTE: acquiring the child's lock while holding parent's lock.
            child.cancel(false, err)
        }
        c.children = nil
        c.mu.Unlock()

        if removeFromParent {
            removeChild(c.Context, c)
        }
    }

    func removeChild(parent Context, child canceler) {
        p, ok := parentCancelCtx(parent)
        if !ok {
            return
        }
        p.mu.Lock()
        if p.children != nil {
            delete(p.children, child)
        }
        p.mu.Unlock()
    }
    ```

## timerCtx

```go
type timerCtx struct {
	cancelCtx
	timer *time.Timer // Under cancelCtx.mu.

	deadline time.Time
}

func (c *timerCtx) Deadline() (deadline time.Time, ok bool) {
	return c.deadline, true
}

func (c *timerCtx) cancel(removeFromParent bool, err error) {
	c.cancelCtx.cancel(false, err)
	if removeFromParent {
		removeChild(c.cancelCtx.Context, c)
	}
	c.mu.Lock()
	if c.timer != nil {
		c.timer.Stop()
		c.timer = nil
	}
	c.mu.Unlock()
}

func WithTimeout(parent Context, timeout time.Duration) (Context, CancelFunc) {
	return WithDeadline(parent, time.Now().Add(timeout))
}

func WithDeadline(parent Context, d time.Time) (Context, CancelFunc) {
	// 校验 parent context 非空
    if parent == nil {
		panic("cannot create context from nil parent")
	}

    // 校验 parent 的过期时间是否早于自己，若是，则构造一个 cancelCtx 返回即可
	if cur, ok := parent.Deadline(); ok && cur.Before(d) {
		// The current deadline is already sooner than the new one.
		return WithCancel(parent)
	}

    // 构造出一个新的 timerCtx
	c := &timerCtx{
		cancelCtx: newCancelCtx(parent),
		deadline:  d,
	}

    // 同步 parent 的 cancel 事件到子 context
	propagateCancel(parent, c)

    // 判断过期时间是否已到，若是，直接 cancel timerCtx，并返回 DeadlineExceeded 的错误
	dur := time.Until(d)
	if dur <= 0 {
		c.cancel(true, DeadlineExceeded) // deadline has already passed
		return c, func() { c.cancel(false, Canceled) }
	}

    // 加锁
	c.mu.Lock()
	defer c.mu.Unlock()

    // 启动 time.Timer，设定一个延时时间，即达到过期时间后会终止该 timerCtx，并返回 DeadlineExceeded 的错误
	if c.err == nil {
		c.timer = time.AfterFunc(dur, func() {
			c.cancel(true, DeadlineExceeded)
		})
	}

    // 返回 timerCtx，已经一个封装了 cancel 逻辑的闭包 cancel 函数
	return c, func() { c.cancel(true, Canceled) }
}
```

## valueCtx

```go
type valueCtx struct {
	Context         // 继承了一个 parent context
	key, val any    // 一个 valueCtx 中仅有一组 kv 对
}

func (c *valueCtx) Value(key any) any {
    // 假如当前 valueCtx 的 key 等于用户传入的 key，则直接返回其 value
	if c.key == key {
		return c.val
	}

    // 假如不等，则从 parent context 中依次向上寻找
	return value(c.Context, key)
}

func value(c Context, key any) any {
	for {
		switch ctx := c.(type) {
		case *valueCtx:
			if key == ctx.key {
				return ctx.val
			}
			c = ctx.Context
		case *cancelCtx:
			if key == &cancelCtxKey {
				return c
			}
			c = ctx.Context
		case *timerCtx:
			if key == &cancelCtxKey {
				return &ctx.cancelCtx
			}
			c = ctx.Context
		case *emptyCtx:
			return nil
		default:
			return c.Value(key)
		}
	}
}

func WithValue(parent Context, key, val any) Context {
	if parent == nil {
		panic("cannot create context from nil parent")
	}
	if key == nil {
		panic("nil key")
	}
	if !reflectlite.TypeOf(key).Comparable() {
		panic("key is not comparable")
	}
	return &valueCtx{parent, key, val}
}
```

- valueCtx 不适合视为存储介质，存放大量的 kv 数据，原因有三

    - 一个 valueCtx 实例只能存一个 kv 对，因此 n 个 kv 对会嵌套 n 个 valueCtx，造成空间浪费

    - 基于 k 寻找 v 的过程是线性的，时间复杂度 O(N)

    - 不支持基于 k 的去重，相同 k 可能重复存在，并基于起点的不同，返回不同的 v

- valueContext 的定位类似于请求头，只适合存放少量作用域较大的全局 meta 数据


## 使用实例

```go
package main

import (
	"context"
	"fmt"
	"time"
)

func HandleRequest(ctx context.Context) {
	go WriteRedis(ctx)
	go WriteDataBase(ctx)

	for {
		select {
		case <-ctx.Done():
			fmt.Println("HandleRequest Done.")
			return
		case <-time.After(2 * time.Second):
			fmt.Println("HandleRequest running")
		}
	}
}

func WriteRedis(ctx context.Context) {
	for {
		select {
		case <-ctx.Done():
			fmt.Println("WriteRedis Done.")
			return
		case <-time.After(2 * time.Second):
			fmt.Println("WriteRedis running")
		}
	}
}

func WriteDataBase(ctx context.Context) {
	for {
		select {
		case <-ctx.Done():
			fmt.Println("WriteDataBase Done.")
			return
		case <-time.After(2 * time.Second):
			fmt.Println("WriteDataBase running")
		}
	}
}

func main() {
	ctx, cancel := context.WithTimeout(context.Background(), 5*time.Second)
	defer cancel()

	go HandleRequest(ctx)

	time.Sleep(6 * time.Second)
}
```