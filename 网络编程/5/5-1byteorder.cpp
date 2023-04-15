#include <iostream>

void byte_order() {

  union {
    short value;
    unsigned char union_data[sizeof(short)];
  } test;

  test.value = 0x0102;

  if (test.union_data[0] == 1 && test.union_data[1] == 2)
    printf("big endian\n");
  else if (test.union_data[0] == 2 && test.union_data[1] == 1)
    printf("little endian\n");
  else
    printf("unknown...\n");

}

int main() {
  byte_order();
  return 0;
}
