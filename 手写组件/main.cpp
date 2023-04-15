#include <bits/stdc++.h>

#include <iostream>
#include <thread>

#include "com.h"

using namespace std;

void test_LFstack() {
  LockFreeStack<int> st;
  vector<thread> threads;
  for (int i = 0; i < 20; i++) {
    threads.emplace_back(&LockFreeStack<int>::push, &st, i);
  }
  for (auto&& th : threads) th.join();

  st.disp();

  threads.clear();

  printf("now pop them:\n");
  vector<int> popvc(20, -1);

  for (int i = 0; i < 20; i++) {
    threads.emplace_back(&LockFreeStack<int>::pop, &st, ref(popvc[i]));
  }

  for (auto&& th : threads) th.join();

  for (auto&& x : popvc) printf("%d ", x);
  printf("\n");
}

int main(int argc, char* argv[]) {
  test_LFstack();
  return 0;
}