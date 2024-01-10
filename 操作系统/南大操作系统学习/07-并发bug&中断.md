#### check and update

```c
// 判断和操作之间可能存在线程切换，导致BUG.
if (balance >= amount) {
    balance -= amount;
}
```

#### 死锁产生的四个必要条件

- 独占资源

- 如果逻辑没有执行完成，不会主动释放已占用资源

- 无法抢占已被其他逻辑占用的资源

- 循环等待


#### 死锁避免

- lock ordering: 对一个锁的集合排序，任意进程对该集合中的一些锁进行上锁只能按照顺序上锁

#### 防御性编程

- 把程序需要满足的条件用assert表达出来

#### Lockdep：运行时Lock Ordering检查

- 为每个锁确定唯一的 allocation site，assert同一个allocation site的锁存在全局唯一的上锁顺序

- 检查方法：printf

#### data race

- gcc -fsanitize=thread

#### 理解中断

- 硬件上的中断：一根线。IRQ、NMI（边沿触发，低电平有效），注意NMI是不可屏蔽中断

- 首先检查处理器配置是否允许中断，如果处理器关闭中断，则忽略。（该 flag 位于 eflags/rflags 寄存器中的 IF 位） 

    - 询问中断控制器获得中断号 n

    - 保存必要的寄存器（CS、RIP、RFLAGS、SS、RSP）到堆栈

    - 跳转到IDT[n]指定的地址，并设置处理器状态（例如关闭中断）

    - 上述三步是原子性的

- cli指令：关中断，但在保护模式下执行该指令会触发段错误（安全保证）。

#### 上下文切换原理

发生中断，中断处理程序调用前保存当前上下文，中断处理程序最后返回时设置cpu寄存器为另一个线程的上下文
