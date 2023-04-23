/**************************************************
 * @file barrier.cc
 * @author your name (you@domain.com)
 * @brief 使用屏障 (11-16.c 的c++实现, std=c++2a)
 * @version 0.1
 * @date 2022-10-19
 * @copyright Copyright (c) 2022
 **************************************************/
#include <thread>
#include <chrono>
#include <vector>
#include <barrier>
#include <iostream>
using namespace std;
using namespace chrono;

const int num_threads = 8;
const int num_numbers = 8000000L;
const int num_batches = num_numbers / num_threads;
long nums[num_numbers];
long snums[num_numbers];
barrier b(num_threads + 1);

/**
 * @brief Compare two long integers (helper function for heapsort)
 * @param arg1
 * @param arg2
 */
int complong(const void * arg1, const void *arg2)
{ long l1 = *(long*)arg1;
  long l2 = *(long*)arg2;
  if (l1 == l2) return 0;
  return (l1 < l2) ? -1 : 1;
}

/**
 * @brief Worker thread to sort a portion of the set of numbers
 * @param idx
 */
void thr_fn(long idx)
{ qsort(&nums[idx], num_batches, sizeof(long), complong);
  b.arrive_and_wait();
}

/**
 * @brief Merge the results of the individual sorted ranges
 */
void merge()
{ long    idx[num_threads];
  long    i, minidx, sidx, num;
  for (i = 0; i < num_threads; i++)
    idx[i] = i * num_batches;
  for (sidx = 0; sidx < num_numbers; sidx++)
  { num = LONG_MAX;
    for (i = 0; i < num_threads; i++)
    { if ((idx[i] < (i + 1) * num_batches) && (nums[idx[i]] < num))
      { num = nums[idx[i]];
        minidx = i;
      }
    }
    snums[sidx] = nums[idx[minidx]];
    idx[minidx]++;
  }
}

int main(int argc, char *argv[])
{ srandom(1);
  vector<jthread> vc;
  for (int i = 0; i < num_numbers; i++)
    nums[i] = random();
  auto start = high_resolution_clock::now();
  for (int i = 0; i < num_threads; i++)
    vc.emplace_back(thr_fn, i * num_batches);
  b.arrive_and_wait();
  merge();
  auto elapsed = high_resolution_clock::now() - start;
  cout << "sort took " << duration_cast<milliseconds>(elapsed).count() << "ms\n";
}