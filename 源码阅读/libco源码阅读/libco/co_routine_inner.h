/*
* Tencent is pleased to support the open source community by making Libco
available.

* Copyright (C) 2014 THL A29 Limited, a Tencent company. All rights reserved.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*	http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing,
* software distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/

#ifndef __CO_ROUTINE_INNER_H__

#include "co_routine.h"
#include "coctx.h"

/**
 * 线程所管理的协程的运行环境
 * 一个线程只有一个这个属性
 */
struct stCoRoutineEnv_t;

// co local变量
struct stCoSpec_t {
  void *value;
};

// 栈内存
struct stStackMem_t {
  stCoRoutine_t *occupy_co; /*> 当前正在使用该栈的协程 */
  int stack_size;           /*> 栈的大小 */
  char *stack_bp;           /*> 栈基址 */
  char *stack_buffer;       /*> 栈顶，stack_bp = stack_buffer + stack_size */
};

// 共享栈数组
struct stShareStack_t {
  unsigned int alloc_idx; /* 当前共享栈数组分配的索引 */
  int stack_size; /* 每个共享栈的大小都一样，为stack_size */
  int count;      /* 共享栈数组长度 */
  stStackMem_t **stack_array; /* 指向共享栈数组 */
};

// 协程
struct stCoRoutine_t {
  stCoRoutineEnv_t *env; /*> 协程所在的运行环境，即该协程所属的协程管理器 */
  pfn_co_routine_t pfn; /*> 协程对应的函数 */
  void *arg;            /*> 协程对应的函数的参数 */
  coctx_t ctx;          /*> 协程上下文，包括寄存器和栈 */

  // 多个char表示bool，节省内存
  char cStart;         /*> 该协程是否已经开始运行 */
  char cEnd;           /*> 该协程是否已经执行完毕 */
  char cIsMain;        /*> 该协程是否是主协程 */
  char cEnableSysHook; /*> 是否打开钩子标识，默认关闭 */
  char cIsShareStack;  /*> 是否使用共享栈 */

  void *pvEnv; /* 存储该协程的环境变量 */

  stStackMem_t *stack_mem; /*> 指向栈内存 */

  char *stack_sp; /*> 当前协程的栈的栈顶，假设当前协程的栈内push了x个字节，则 stack_sp = stack_mem->stack_bp - x */
  unsigned int save_size; /*> save_buffer的大小 */
  char *save_buffer; /*> 当协程挂起时，栈的内容会暂存到save_buffer中 */

  stCoSpec_t aSpec[1024]; /* 存储co local变量的数组 */
};

/**
 * @brief 初始化当前线程的协程运行环境
 * 该函数创建当前线程的协程运行环境，并且创建主协程
 */
void co_init_curr_thread_env();

// 获取当前线程的协程运行环境
stCoRoutineEnv_t *co_get_curr_thread_env();

/**
 * @brief 释放协程co
 *
 * @param co 指向一个协程
 */
void co_free(stCoRoutine_t *co);

/**
 * @brief yield指定协程环境中正在调度的协程
 *
 * @param env 指定一个协程环境
 */
void co_yield_env(stCoRoutineEnv_t *env);

// 超时链表结点
struct stTimeoutItem_t;

// 超时链表
struct stTimeoutItemLink_t;

// 毫秒级的超时管理器，使用时间轮实现
struct stTimeout_t;

/**
 * @brief 分配一个超时管理器
 *
 * @param iSize 默认为 60*1000
 * @return stTimeout_t* 指向在堆中分配的超时管理器
 */
stTimeout_t *AllocTimeout(int iSize);

/**
 * @brief 释放堆中分配的超时管理器
 *
 * @param apTimeout 指向一个堆中分配的超时管理器
 */
void FreeTimeout(stTimeout_t *apTimeout);

/**
 * @brief 添加事件到定时器
 *
 * @param apTimeout 超时管理器
 * @param apItem 待添加的超时事件结点
 * @param allNow 当前时间
 * @return int
 */
int AddTimeout(stTimeout_t *apTimeout, stTimeoutItem_t *apItem,
               uint64_t allNow);

// 管理epoll的结构体
struct stCoEpoll_t;

/**
 * @brief 申请一个epoll
 *
 * @return stCoEpoll_t*
 */
stCoEpoll_t *AllocEpoll();

/**
 * @brief 释放一个epoll
 *
 * @param ctx 指向epoll结构体
 */
void FreeEpoll(stCoEpoll_t *ctx);

/**
 * @brief 获取当前线程正在调度的协程
 *
 * @return stCoRoutine_t* 指向协程的指针
 */
stCoRoutine_t *GetCurrThreadCo();

/**
 * @brief 设置指定协程管理器的管理epoll的结构体
 *
 * @param env 协程管理器
 * @param ev 指向管理epoll的结构体
 */
void SetEpoll(stCoRoutineEnv_t *env, stCoEpoll_t *ev);

#endif

#define __CO_ROUTINE_INNER_H__
