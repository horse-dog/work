#### const关键字

- const char*、char const*、char* const 的区别

#### 指针和数组

- char str1[12] = "hello world" 和 char* str2 = "hello world" 的联系和区别：sizeof，数组传参时退化为指针

#### 指针和引用

- 引用的底层就是指针，可以通过查看汇编证明

- 引用声明时必须初始化，并且引用初始化后不能绑定到其他对象了

- sizeof引用和sizeof对象大小是一样的，但引用本身却只占用8字节（指针的大小）

```cpp
struct st {
  double a;
  double b;
  double c;
  double d;
};

int main() {
  st s;
  st& ss = s;
  // 都是32
  const int a = sizeof(st);
  const int b = sizeof(st&);
  const int c = sizeof(s);
  const int d = sizeof(ss);
  return 0;
}
```

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

#### 虚函数表指针、虚函数表、虚基类表指针、虚基类表

- 虚函数指针位于对象所在内存，在构造函数执行中进行初始化。

- 虚函数表位于只读常量区，在编译时初始化。

- 虚基类表指针、虚基类表与虚函数表指针和虚函数表类似。

#### 构造函数和析构函数

- 构造函数不能是虚函数，构造函数内部不能调用虚函数：从存储空间角度来看 虚函数的调用需要虚函数表(vptr)指针，而该指针存放在对象的内存空间中，在构造函数中进行初始化工作，即初始化vptr，让它指向正确的虚函数表。所以需要调用构造函数才可以创建或初始化它的值，否则即使开辟了空间，该指针也为随机值；若构造函数声明为虚函数，那么由于对象还未创建，还没有内存空间，更没有虚函数表地址用来调用虚函数——构造函数了。同理，构造函数中不能调用虚函数。

- 析构函数可以为虚函数，析构函数中可以调用虚函数。而且当要使用基类指针或引用调用子类时，最好将基类的析构函数声明为虚函数，否则会存在内存泄露的问题。

- 构造函数的调用顺序：基类构造函数、对象成员构造函数、派生类本身的构造函数

- 析构函数的调用顺序：派生类本身的析构函数、对象成员析构函数、基类析构函数（与构造顺序正好相反）

#### 静态变量的初始化时机

- 初始化只有一次，但是可以多次赋值，在主程序之前，编译器已经为其分配好了内存

- 静态局部变量和全局变量一样，数据都存放在全局区域，所以在主程序之前，编译器已经为其分配好了内存，但在C和C++中静态局部变量的初始化时机不太一样。在C中，初始化发生在main函数执行之前，程序运行结束，变量所处的全局内存会被全部回收。

- 在C++中，初始化是在执行相关的代码时才会进行初始化，主要是由于C++引入对象后，要进行初始化必须执行相应的构造函数和析构函数，在构造函数或析构函数中经常会需要进行某些程序中需要进行的特定操作，并非简单地分配内存。所以C++标准定为全局或静态对象是有首次用到时才会进行构造，并通过atexit来管理。在程序结束，按照构造顺序反方向逐个析构，所以在C++中是可以使用变量对静态局部变量进行初始化的。

```cpp
// c语言中这种代码编译报错，c++中不会
void fn(int x) {
  static int s = x;
}
```

#### c语言用户空间内存布局

- 代码段

- 数据段

- bss段

- 堆区

- 栈区

#### c++用户空间内存布局

- 代码段

- 只读常量区

- 静态数据区

- 堆区

- 栈区

#### 字符串操作相关

- C语言中的常用字符串操作

```c
// 复制字符串
void strcpy(char* dst, const char* src);

// 复制字符串，长度超过n会被截断
void strncpy(char* dst, const char* src, size_t n);

// 在尾部追加字符串
void strcat(char* dst, const char* src);

// 在尾部追加字符串，追加超过n会被截断
void strncat(char* dst, const char* src, size_t n);

// 计算字符串长度
size_t strlen(const char* str);

// 按字典序比较字符串
int strcmp(const char *s1, const char *s2);

// 按字典序比较字符串，比较长度不超过n
int strncmp(const char *s1, const char *s2, size_t n);

// 按字典序比较字符串，不区分大小写
int strcasecmp(const char *s1, const char *s2);

// 按字典序比较字符串，不区分大小写，比较长度不超过n
int strncasecmp(const char *s1, const char *s2, size_t n);

// 寻找指定字符在字符串中的位置，如果没找到，返回NULL
char *strchr(char *str, int c);
const char *strchr(const char *str, int c);

// 反向寻找指定字符在字符串中的位置，如果没找到，返回NULL
char *strrchr(char *str, int c);
const char *strrchr(const char *str, int c);

// 子串匹配
char *strstr(char *haystack, const char *needle);
const char *strstr(const char *haystack, const char *needle);

// 按照给定的分隔符集分割字符串
// 函数会修改字符串，每次调用都会将一个字符串中的分隔符替换为'\0'
char *strtok(char *s, const char *delim);

// strtok使用示例:
// 输出：
// How
// are
// you
// find
int main() {
  char str[] = "How are you\nfind";
  char *s;
  s = strtok(str, "\r\n\t ");
  while (s != nullptr) {
    printf("%s\n", s);
    s = strtok(NULL, "\r\n\t ");
  } 
  return 0;
}

// 依次检验字符串 str1 中的字符，当被检验字符在字符串 str2 中也包含时，则停止检验，并返回该字符位置。
char *strpbrk(const char *str1, const char *str2);
```

- std::string查找相关api

  - find(target, off)：寻找 char、char*、string、string_view 在字符串中第一次出现的位置，off 为开始寻找的下标，off 默认为0

  - rfind(traget, off)：与find类似，但反向查找

  - find_first_of(target, off)：同 find

  - find_first_not_of(target, off)：与find类似，但寻找从左到右第一个不满足 target 的下标

  - find_last_of(target, off)：同 rfind

  - find_last_not_of(target, off)：与rfind类似，但寻找从右到左第一个不满足 target 的下标

- C语言中字符串与数据类型的互转

  - 字符串转数值：atoi转int、atof转double、atol转long、atoll转long long

  - 字符串转数值：strtol、strtoll、strtoul、strtoull、strtof、strtod，这些函数类似于c++转换api，整型转换可以指定进制

  - 数值转字符串：通用方法为使用sprintf

- C++中string与数据类型的互转

  - 注意，以下函数很多都具有std::wstring宽字符串的重载

  - string转int：int std::stoi(const string &__str, size_t *idx = nullptr, int base = 10);

  ```cpp
  /**
   * @brief string转int
   * @param str string
   * @param idx 用于存储转换过程中第一个非数字字符的位置，默认为nullptr
   * @param base 转换的进制，默认为10进制
   * @return int 转换结果
   */
  int std::stoi(const string &str, size_t *idx = nullptr, int base = 10);

  // 使用示例
  int main() {
    string str = "64sb";
    size_t idx;
    const auto x = std::stoi(str, &idx, 16);
    cout << x << endl;   // 100
    cout << idx << endl; // 2
    return 0;
  }
  ```
  - string转其他类型

  ```cpp
  // string转浮点数
  double stod(const string &str, size_t *idx = nullptr);
  float stof(const string &str, std::size_t *idx = nullptr);
  long double stold(const string &str, size_t *idx = nullptr);

  // string转整型
  long std::stol(const string &str, size_t *idx = nullptr, int base = 10);
  long long std::stoll(const string &str, size_t *idx = nullptr, int base = 10);
  long std::stoul(const string &str, size_t *idx = nullptr, int base = 10);
  long long std::stoull(const string &str, size_t *idx = nullptr, int base = 10);
  ```

  - 数值类型转string：std::to_string，该函数具有常见的所有数值类型的重载

  - 数值类型与string互转的另外一种方式：使用std::stringstream

  ```cpp
  template <class out_type, class in_type>
  out_type convert(const in_type& t) {
    std::stringstream stream;
    stream << t;
    out_type result;
    stream >> result;
    return result;
  }

  int main() {
    double d;
    string salary;
    string s = "12.56";
    d = convert<double>(s);            // d等于12.56
    salary = convert<string>(9000.0);  // salary等于”9000”
  }
  ```

- 编码方式的转换

```cpp
#include <locale>
#include <codecvt>

// wstring转string
std::string wstring_to_utf8(const std::wstring& wstr) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  return conv.to_bytes(wstr);
}

// string转wstring
std::wstring utf8_to_wstring(const std::string& str) {
  std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
  return conv.from_bytes(str);
}
```