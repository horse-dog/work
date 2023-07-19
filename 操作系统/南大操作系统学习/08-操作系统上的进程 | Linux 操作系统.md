#### fork

```cpp
for (int i = 0; i < 2; i++) {
    fork();
    printf("hello world!\n");
}
```

- 上述代码：直接运行，输出6行

- 上述代码：重定向输出到文件或用管道传入其他程序，输出8行，原因是printf不再是直接输出到终端，而是一个缓冲区

#### exec函数族

- 重置当前进程的状态机，可以灵活地指定参数和环境变量

#### exit、_exit、syscall exit、syscall exit_group

- exit函数是C标准库函数，会调用一些清理操作，然后调用_exit

- _exit调用syscall exit 和 syscall exit_group，结束进程中的各个线程

- syscall exit 仅结束当前线程

- syscall exit_group 结束当前进程的其他线程