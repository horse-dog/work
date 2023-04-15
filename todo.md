
```cpp
struct empty_t {};
void fn() {
  empty_t a;
  empty_t b;
  void* addrA = &a;
  void* addrB = &b;
  printf("%p\n", addrA);
  printf("%p\n", addrB);
  printf("%p\n", &addrA);
  printf("%p\n", &addrB);
  // 栈不是向下增长的吗？
  assert((char*)addrA == (char*)addrB - 1);
}

int main() {
  fn();
  return 0;
}
```