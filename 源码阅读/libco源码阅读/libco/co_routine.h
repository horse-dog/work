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

#ifndef __CO_ROUTINE_H__
#define __CO_ROUTINE_H__

#include <pthread.h>
#include <stdint.h>
#include <sys/poll.h>

// 1.struct

struct stCoRoutine_t;
struct stShareStack_t;

// 协程属性
struct stCoRoutineAttr_t {
  int stack_size;              /*> 协程栈大小 */
  stShareStack_t *share_stack; /*> 共享栈 */
  stCoRoutineAttr_t() {
    stack_size = 128 * 1024;  // 协程栈大小默认为128K
    share_stack = NULL;       // 默认不使用共享栈
  }
} __attribute__((packed));

struct stCoEpoll_t;
typedef int (*pfn_co_eventloop_t)(void *);
typedef void *(*pfn_co_routine_t)(void *);

/**
 * @brief 创建一个协程，类pthread风格api
 * 
 * @param co 用于保存返回的协程的指针
 * @param attr 协程属性
 * @param routine 协程例程
 * @param arg 例程参数
 * @return int 返回0代表创建成功
 */
int co_create(stCoRoutine_t **co, const stCoRoutineAttr_t *attr,
              void *(*routine)(void *), void *arg);

/**
 * @brief 继续执行指定协程
 * 
 * @param co 指定一个协程
 */
void co_resume(stCoRoutine_t *co);

/**
 * @brief yield指定协程
 * 
 * @param co 指定一个协程
 */
void co_yield (stCoRoutine_t *co);

/**
 * @brief yield当前线程正在调度的协程
 * 
 * @note  ct = current thread
 */
void co_yield_ct();

/**
 * @brief 释放协程co
 * 
 * @param co 指向一个协程
 */
void co_release(stCoRoutine_t *co);

/**
 * @brief 用于reset超时无法重复使用的协程
 * 
 * @param co 指向一个协程
 */
void co_reset(stCoRoutine_t *co);

/**
 * @brief 获取当前线程正在调度的协程
 * 
 * @return stCoRoutine_t* 
 */
stCoRoutine_t *co_self();

/**
 * @brief 类似于poll函数IO复用，注册事件到epoller中，并指定超时
 * 
 * @param ctx epoller
 * @param fds pollfd数组
 * @param nfds fds数组的长度
 * @param timeout_ms epoll_wait 的超时时间
 * @return int 
 */
int co_poll(stCoEpoll_t *ctx, struct pollfd fds[], nfds_t nfds, int timeout_ms);

void co_eventloop(stCoEpoll_t *ctx, pfn_co_eventloop_t pfn, void *arg);

// 3.specific

int co_setspecific(pthread_key_t key, const void *value);

void *co_getspecific(pthread_key_t key);

/**
 * @brief 获取当前线程的epoll
 * @note ct = current thread
 *
 * @return stCoEpoll_t*
 */
stCoEpoll_t *co_get_epoll_ct();

// 5.hook syscall ( poll/read/write/recv/send/recvfrom/sendto )

/**
 * @brief 启用当前线程中正在运行的协程的hook
 */
void co_enable_hook_sys();

/**
 * @brief 关闭当前线程中正在运行的协程的hook
 */
void co_disable_hook_sys();

/**
 * @brief 获取当前线程中正在运行的协程是否启用了hook
 * 
 * @return true 
 * @return false 
 */
bool co_is_enable_sys_hook();

// 条件变量链表
struct stCoCond_t;

/**
 * @brief 申请一个条件变量
 * 
 * @return stCoCond_t* 
 */
stCoCond_t *co_cond_alloc();

/**
 * @brief 释放一个条件变量
 * 
 * @param cc 指向一个条件变量
 * @return int 
 */
int co_cond_free(stCoCond_t *cc);

/**
 * @brief 唤醒 @p si 头部的条件变量
 * 
 * @param si 指向一个条件变量链表
 * @return int 
 */
int co_cond_signal(stCoCond_t *si);

/**
 * @brief 唤醒 @p si 中所有的条件变量
 * 
 * @param si 指向一个条件变量链表
 * @return int 
 */
int co_cond_broadcast(stCoCond_t *si);

/**
 * @brief 新建一个条件变量，加入链表 @p link ，然后yield当前协程
 * 直到链表 @p link 被唤醒（通过co_cond_signal或co_cond_broadcast）
 * 或者超时 @p timeout_ms .
 * 
 * @param link 指向一个条件变量链表
 * @param timeout_ms 超时时间
 * @return int 
 */
int co_cond_timedwait(stCoCond_t *link, int timeout_ms);

/**
 * @brief 分配共享栈
 * 
 * @param iCount 共享栈管理的栈的数量
 * @param iStackSize 每个栈的大小
 * @return stShareStack_t* 
 */
stShareStack_t *co_alloc_sharestack(int iCount, int iStackSize);

// 8.init envlist for hook get/set env
void co_set_env_list(const char *name[], size_t cnt);

/**
 * @brief 打印错误日志
 *
 * @param fmt 格式化串
 * @param ... 不定参数
 */
void co_log_err(const char *fmt, ...);
#endif
