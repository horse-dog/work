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

#ifndef __CO_EPOLL_H__
#define __CO_EPOLL_H__
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#if !defined(__APPLE__) && !defined(__FreeBSD__)

#include <sys/epoll.h>

// epoll_wait的结果
struct co_epoll_res {
  int size;                   /* epoll_wait返回的事件的数量 */
  struct epoll_event *events; /* epoll_wait返回的事件的数组 */
  struct kevent
      *eventlist; /* kqueue, 在BSD（如macos）里面才用这个，linux用epoll */
};

/**
 * @brief 封装了epoll_wait
 * 
 * @param epfd epfd
 * @param events 用于从内核接收到达事件
 * @param maxevents 最大支持的事件数量
 * @param timeout 超时时间
 * @return int 
 */
int co_epoll_wait(int epfd, struct co_epoll_res *events, int maxevents,
                  int timeout);

/**
 * @brief 封装epoll_ctl，其实和epoll_ctl一模一样
 * 
 * @param epfd epfd
 * @param op 操作：增删改
 * @param fd 操作的键
 * @param ev epoll事件
 * @return int 
 */
int co_epoll_ctl(int epfd, int op, int fd, struct epoll_event * ev);

/**
 * @brief 封装了epoll_create，其实和epoll_create一模一样
 * 
 * @param size 没什么用
 * @return int 
 */
int co_epoll_create(int size);

/**
 * @brief 分配一个co_epoll_res结构
 * 
 * @param n co_epoll_res中管理的epoll_events事件数组的大小
 * @return struct co_epoll_res* 
 */
struct co_epoll_res *co_epoll_res_alloc(int n);

/**
 * @brief 释放一个co_epoll_res结构
 */
void co_epoll_res_free(struct co_epoll_res *);

#else

#include <sys/event.h>
enum EPOLL_EVENTS {
  EPOLLIN = 0X001,
  EPOLLPRI = 0X002,
  EPOLLOUT = 0X004,

  EPOLLERR = 0X008,
  EPOLLHUP = 0X010,

  EPOLLRDNORM = 0x40,
  EPOLLWRNORM = 0x004,
};
#define EPOLL_CTL_ADD 1
#define EPOLL_CTL_DEL 2
#define EPOLL_CTL_MOD 3
typedef union epoll_data {
  void *ptr;
  int fd;
  uint32_t u32;
  uint64_t u64;

} epoll_data_t;

struct epoll_event {
  uint32_t events;
  epoll_data_t data;
};

struct co_epoll_res {
  int size;
  struct epoll_event *events;
  struct kevent *eventlist;
};
int co_epoll_wait(int epfd, struct co_epoll_res *events, int maxevents,
                  int timeout);
int co_epoll_ctl(int epfd, int op, int fd, struct epoll_event *);
int co_epoll_create(int size);
struct co_epoll_res *co_epoll_res_alloc(int n);
void co_epoll_res_free(struct co_epoll_res *);

#endif
#endif
