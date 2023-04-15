#### const关键字

- const char*、char const*、char* const 的区别

#### 指针和数组

- char str1[12] = "hello world" 和 char* str2 = "hello world" 的联系和区别

#### 指针和引用

TODO: unfinished

#### C++类型转换

- static_cast: 编译期内即可决定其类型的转换。不能转换掉底层的const、volatile和__unaligned属性。

- dynamic_cast: 仅适用于指针或引用。用于多态中父子类之间的多态转换，会在运行时检查类型转换是否合法，具有一定的安全性。由于运行时的检查，所以会额外消耗一些性能。

- const_cast: 仅适用于指针或引用。用于移除类型的const、volatile和__unaligned属性。

- reinterpret_cast: 非常激进的指针类型转换，在编译期完成，可以转换任何类型的指针，所以极不安全。非极端情况不要使用。

#### C++数据自动初始化

- 全局变量和静态变量如果没有显示地初始化，会自动进行初始化，存储在 bss 段。

  - 如果是基础类型，默认填充为全0

  - 如果是类类型、调用默认构造函数

- 局部非静态变量如果没有显式地初始化：

  - 如果是基础类型，不会自动初始化，可能是任意值

  - 如果是类对象，调用默认构造函数

- 数组的初始化：

  - 全局或静态的数组：基础类型填充为全0，类类型调用默认构造函数

  - 局部数组：基础类型不会自动初始化，可能是任意值，类类型调用默认构造函数。

- new 数组的初始化：

  - 基础类型不会自动初始化，可能是任意值。

  - 类类型调用默认构造函数。

- 类成员的初始化：

  - 基础类型类成员不会自动初始化，可能为任意值。

  - 类类型成员自动调用默认构造函数初始化。

#### C++数据初始化方式

- 对于变量：可以使用`{}`或`()`初始化，括号内可以填充构造函数的参数，不填则调用默认构造。`()`允许不安全的类型转换，`{}`则不允许。还可以在`{}`前面加上`=`，此时转变为调用移动构造函数。`()`前面不能加`=`。

- 对于数组：使用`{}`初始化，前面可以有`=`，`{}`内可以填充部分值，剩下的值会调用默认构造函数初始化（基础类型填全0）。

- 对于`new`数组：与普通数组一致。

#### 操作符重载

- 类成员函数重载单目操作符

  ```cpp
  class A {
   public:
    int& operator[](size_t i) {
      assert(i >= 0 && i < 10);
      return arr[i];
    }
   private:
    int arr[10];
  };

  void fn(A a) {
    int x = a[3];
  }
  ```

- 类成员函数重载类型转换操作符

  ```cpp
  class A {
   public:
    // 强制转换为bool
    explicit operator bool() const {
      return x != 0;
    }
   private:
    int x;
  };

  void fn(A a) {
    if (a) {
      // dosomething...
    }
  }
  ```

- 类成员函数重载调用操作符

  ```cpp
  struct A {
    int operator()() {
      return 0;
    }
    int operator()(int a, int b) {
      return a + b;
    }
  };

  void fn() {
    A a;
    int x = a();     // 0
    int y = a(1, 2); // 3
  }
  ```

- 类成员函数重载双目操作符

  ```cpp
  class A {
   public:
    bool operator==(const A& a, const A& b) const {
      return a.x == b.x;
    }
   private:
    int x;
  };

  void fn() {
    A a1, a2;
    if (a1 == a2) {
      // dosomething...
    }
  }
  ```

- 友元函数重载单目操作符

  ```cpp
  class A {
   public:
    A(int x) : x(x) {}
    friend A operator-(const A& a);
   private:
    int x;
  };

  A operator-(const A& a) {
    return A(-a.x);
  }

  void fn() {
    A a(10);
    A b = -a;
  }
  ```

- 友元函数重载双目操作符，例如 cout << a 的调用者为 cout 对象，而不是要输出的对象本身，因而不能使用成员函数重载，需要使用友元函数重载

  ```cpp
  class A {
   public:
    friend std::ostream& operator<<(std::ostream& os, const A& a);
   private:
    int x;
  };

  std::ostream& operator<<(std::ostream& os, const A& a) {
    return os << a.x;
  }
  ```

#### 面向对象的四大特性

- 封装：隐藏对象的属性和实现细节，仅对外公开接口

- 抽象：抽象数据和过程

- 继承：子类继承父类的特征和行为，使得子类具有父类的成员变量和方法。

- 多态：即同一个行为具有多个不同表现形式或形态的能力。表现形式有覆盖和重载。

#### 常见关键字

- static关键字

  - 修饰全局函数或变量，表示该符号不会导出到外部

  - 修饰局部变量，表示该变量是局部静态变量

  - 修饰类成员函数或变量，表示该符号是类共有的

- volatile关键字

  - 直接存取原始内存地址

  - 如果没有 volatile 关键字，则编译器可能优化读取和存储，可能暂时使用寄存器中的值，如果这个变量由别的程序更新了的话，将出现不一致的现象。

  - 频繁地使用 volatile 很可能会增加代码尺寸和降低性能,因此要合理的使用 volatile。

- restrict关键字

  - restrict 关键字用于修饰指针(C99标准)

  - 加上restrict关键字，编程者可提示编译器：在该指针的生命周期内，其指向的对象不会被别的指针所引用

- mutable关键字

  - 被const修饰的成员函数无法修改类的成员变量

  - 有时候需要在const成员函数中修改成员变量的值，用mutable关键字修饰想要修改成员变量即可

- const关键字：能够有效地避免程序中的意外修改，提高程序的可读性和可维护性。

  - 修饰类成员函数，该函数隐含传入的this指针为 const A* 类型，函数内部无法修改类成员，注意如果某个类成员是一个结构体的指针，函数内部是可以通过该指针修改指向的结构体的，因为 const A*只保证这个指针的值不变，不保证这个指针指向的对象的内容不变，除非这个指针本身也是常量指针。

  - 修饰类成员变量、全局变量或局部变量，指示该值是一个常量，表示该变量的值在定义后不能被修改。

  - 修饰常量对象，表示该对象的值在定义后不能被修改，常量对象只能调用常量成员函数，不能调用非常量成员函数。

  - const修饰类成员变量时，该成员变量可以通过构造函数参数列表进行初始化，但不能在构造函数函数体内用等号赋值初始化。

- constexpr关键字

  - 修饰函数，指示该函数可以在编译时计算出表达式的值

  - 修饰变量，指示变量的值在编译时就已经确定了

  - const和constexpr的区别：const关键字用于声明常量，而constexpr关键字用于声明常量表达式。C++ 11标准中，为了解决 const 关键字的双重语义问题，保留了const表示“只读”的语义，而将“常量”的语义划分给了新添加的constexpr关键字。

- virtual关键字

  - 指示虚继承

  - 指示虚函数

- override关键字：指示对重写的重写

#### 类继承规则：

- 父类的私有成员在子类中不可见，公开成员和保护成员在子类中可见，它们在子类中的保护级别取决于它们在父类中的保护级别和子类继承父类的方式二者的最大值

- public继承方式

  - 基类中所有 public 成员在派生类中为 public 属性；

  - 基类中所有 protected 成员在派生类中为 protected 属性；
  
  - 基类中所有 private 成员在派生类中不能使用。

- protected继承方式

  - 基类中的所有 public 成员在派生类中为 protected 属性；
  
  - 基类中的所有 protected 成员在派生类中为 protected 属性；
  
  - 基类中的所有 private 成员在派生类中不能使用。

- private继承方式

  - 基类中的所有 public 成员在派生类中均为 private 属性；
  
  - 基类中的所有 protected 成员在派生类中均为 private 属性；
  
  - 基类中的所有 private 成员在派生类中不能使用。