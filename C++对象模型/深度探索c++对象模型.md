#### sizeof空类

- sizeof空类的结果为1，因为c++要求每个对象具有不同的地址，如果在某个调用里，连续声明了两个某空类的对象，如果空类的对象占0字节，则这两个对象将拥有相同地址。所以空类的对象不能占0字节，只占1字节当然是出于节省内存的考虑。

  ```cpp
  struct empty_t {};
  void fn() {
    empty_t a;
    empty_t b;
    void* addrA = &a;
    void* addrB = &b;
    assert(addrA + 1 == addrB);
  }
  ```

#### this指针调整

```cpp
struct A {
  int a;
  A() { printf("'A::A()': this=%p\n", this); }
  void fnA() { printf("'A::fnA()': this=%p\n", this); }
};

struct B {
  int b;
  B() { printf("'B::B()': this=%p\n", this); }
  void fnB() { printf("'B::fnB()': this=%p\n", this); }
};

struct C : public A, public B {
  int c;
  C() { printf("'C::C()': this=%p\n", this); }
  void fnB() { printf("'C::fnB()': this=%p\n", this); }
  void fnC() { printf("'C::fnC()': this=%p\n", this); }
};

/**
 * 某次输出如下：
 * 'A::A()'  : this=0x7ffd9ac2aadc
 * 'B::B()'  : this=0x7ffd9ac2aae0
 * 'C::C()'  : this=0x7ffd9ac2aadc
 * 'A::fnA()': this=0x7ffd9ac2aadc
 * 'B::fnB()': this=0x7ffd9ac2aae0
 * 'C::fnB()': this=0x7ffd9ac2aadc
 * 'C::fnC()': this=0x7ffd9ac2aadc
 */
int main() {
  C c;
  c.fnA();
  c.B::fnB();
  c.fnB();
  c.fnC();
  return 0;
}
```

#### 构造函数语义

- POD类型
  
  - 没有用户定义的构造函数、拷贝构造函数和析构函数
  
  - 没有虚函数和虚基类
  
  - 所有非静态成员都是POD类型
  
  - 要么没有任何基类，要么继承自POD类型

-