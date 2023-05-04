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

#include "co_routine.h"

#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <netinet/in.h>
#include <poll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <map>
#include <string>

#include "co_epoll.h"
#include "co_routine_inner.h"

extern "C" {
extern void coctx_swap(coctx_t *, coctx_t *) asm("coctx_swap");
};

using namespace std;

stCoRoutine_t *GetCurrCo(stCoRoutineEnv_t *env);

struct stCoEpoll_t;

struct stCoRoutineEnv_t {
  // 这里实际上维护的是个调用栈
  // 最后一位是当前运行的协程，前一位是当前协程的父协程(即，resume该协程的协程)
  // 可以看出来，libco只能支持128层协程的嵌套调用。这个绝对够了
  stCoRoutine_t *pCallStack[128];
  int iCallStackSize;   // 当前调用栈长度
  stCoEpoll_t *pEpoll;  // 主要是epoll，作为协程的调度器

  // for copy stack log lastco and nextco
  stCoRoutine_t *pending_co;  // TODO
  stCoRoutine_t *occupy_co;   // TODO
};

void co_log_err(const char *fmt, ...) {}

#if defined(__LIBCO_RDTSCP__)
static unsigned long long counter(void) {
  register uint32_t lo, hi;
  register unsigned long long o;
  __asm__ __volatile__("rdtscp" : "=a"(lo), "=d"(hi)::"%rcx");
  o = hi;
  o <<= 32;
  return (o | lo);
}
static unsigned long long getCpuKhz() {
  FILE *fp = fopen("/proc/cpuinfo", "r");
  if (!fp) return 1;
  char buf[4096] = {0};
  fread(buf, 1, sizeof(buf), fp);
  fclose(fp);

  char *lp = strstr(buf, "cpu MHz");
  if (!lp) return 1;
  lp += strlen("cpu MHz");
  while (*lp == ' ' || *lp == '\t' || *lp == ':') {
    ++lp;
  }

  double mhz = atof(lp);
  unsigned long long u = (unsigned long long)(mhz * 1000);
  return u;
}
#endif

/**
 * @brief 获取当前系统时间（毫秒级）
 *
 * @return unsigned long long
 */
static unsigned long long GetTickMS() {
#if defined(__LIBCO_RDTSCP__)
  static uint32_t khz = getCpuKhz();
  return counter() / khz;
#else
  // 1秒 = 1000毫秒 = 1000'000微秒 = 1000'000'000纳秒
  // timeval结构体由秒 + 微秒组成
  struct timeval now = {0};
  gettimeofday(&now, NULL);
  unsigned long long u = now.tv_sec;
  u *= 1000;
  u += now.tv_usec / 1000;
  return u;
#endif
}

template <class T, class TLink>
void RemoveFromLink(T *ap) {
  TLink *lst = ap->pLink;
  if (!lst) return;
  assert(lst->head && lst->tail);

  if (ap == lst->head) {
    lst->head = ap->pNext;
    if (lst->head) {
      lst->head->pPrev = NULL;
    }
  } else {
    if (ap->pPrev) {
      ap->pPrev->pNext = ap->pNext;
    }
  }

  if (ap == lst->tail) {
    lst->tail = ap->pPrev;
    if (lst->tail) {
      lst->tail->pNext = NULL;
    }
  } else {
    ap->pNext->pPrev = ap->pPrev;
  }

  ap->pPrev = ap->pNext = NULL;
  ap->pLink = NULL;
}

/**
 * @brief 模板算法，将一个结点尾插到一个链表
 *
 * @tparam TNode 结点类型
 * @tparam TLink 链表类型
 * @param apLink 链表
 * @param ap 结点
 */
template <class TNode, class TLink>
void inline AddTail(TLink *apLink, TNode *ap) {
  // 如果结点ap已经有了主人（链表），直接返回
  if (ap->pLink) {
    return;
  }
  // 如果链表apLink有尾结点（即链表非空）
  if (apLink->tail) {
    apLink->tail->pNext = (TNode *)ap;
    ap->pNext = NULL;
    ap->pPrev = apLink->tail;
    // 这里记得更新链表的尾结点
    apLink->tail = ap;
  } else {  // 链表还是空的。。。
    apLink->head = apLink->tail = ap;
    ap->pNext = ap->pPrev = NULL;  // 这里说明实现的是无环的双链表
  }
  // 最后记得设置parent
  ap->pLink = apLink;
}

template <class TNode, class TLink>
void inline PopHead(TLink *apLink) {
  if (!apLink->head) {
    return;
  }
  TNode *lp = apLink->head;
  if (apLink->head == apLink->tail) {
    apLink->head = apLink->tail = NULL;
  } else {
    apLink->head = apLink->head->pNext;
  }

  lp->pPrev = lp->pNext = NULL;
  lp->pLink = NULL;

  if (apLink->head) {
    apLink->head->pPrev = NULL;
  }
}

/**
 * @brief 合并链表，类似于stl的list<T>::merge
 * 该函数把apOther尾插到apLink，并把apOther置空
 *
 * @tparam TNode 结点类型
 * @tparam TLink 链表类型
 * @param apLink 目的链表
 * @param apOther 待合并的链表
 */
template <class TNode, class TLink>
void inline Join(TLink *apLink, TLink *apOther) {
  // 如果apOther链表为空，那不需要Join了
  if (!apOther->head) {
    return;
  }

  // apOther的所有结点的parent设置为apLink
  TNode *lp = apOther->head;
  while (lp) {
    lp->pLink = apLink;
    lp = lp->pNext;
  }

  lp = apOther->head;

  // 把apOther链接到apLink尾部
  if (apLink->tail) {
    apLink->tail->pNext = (TNode *)lp;
    lp->pPrev = apLink->tail;
    apLink->tail = apOther->tail;
  } else {
    apLink->head = apOther->head;
    apLink->tail = apOther->tail;
  }

  // apOther现在被掏空了
  apOther->head = apOther->tail = NULL;
}

/////////////////for copy stack //////////////////////////
stStackMem_t *co_alloc_stackmem(unsigned int stack_size) {
  stStackMem_t *stack_mem = (stStackMem_t *)malloc(sizeof(stStackMem_t));
  stack_mem->occupy_co = NULL;
  stack_mem->stack_size = stack_size;
  stack_mem->stack_buffer = (char *)malloc(stack_size);
  stack_mem->stack_bp = stack_mem->stack_buffer + stack_size;
  return stack_mem;
}

stShareStack_t *co_alloc_sharestack(int count, int stack_size) {
  stShareStack_t *share_stack =
      (stShareStack_t *)malloc(sizeof(stShareStack_t));
  share_stack->alloc_idx = 0;
  share_stack->stack_size = stack_size;

  // alloc stack array
  share_stack->count = count;
  stStackMem_t **stack_array =
      (stStackMem_t **)calloc(count, sizeof(stStackMem_t *));
  for (int i = 0; i < count; i++) {
    stack_array[i] = co_alloc_stackmem(stack_size);
  }
  share_stack->stack_array = stack_array;
  return share_stack;
}

static stStackMem_t *co_get_stackmem(stShareStack_t *share_stack) {
  if (!share_stack) {
    return NULL;
  }
  int idx = share_stack->alloc_idx % share_stack->count;
  share_stack->alloc_idx++;

  return share_stack->stack_array[idx];
}

// 管理epoll的结构体
struct stCoEpoll_t {
  int iEpollFd;                             /*> epfd */
  static const int _EPOLL_SIZE = 1024 * 10; /*> epoll_wait最大支持的事件数 */
  struct stTimeout_t *pTimeout;             /*> 超时管理器 */
  struct stTimeoutItemLink_t *pstTimeoutList; /*> 用于管理超时事件的链表 */
  struct stTimeoutItemLink_t *pstActiveList; /*> 用于管理活跃事件的链表 */
  co_epoll_res *result;                      /*> 存储结果 */
};

// 结点事件预处理函数
typedef void (*OnPreparePfn_t)(stTimeoutItem_t *, struct epoll_event &ev,
                               stTimeoutItemLink_t *active);

// 结点事件处理函数
typedef void (*OnProcessPfn_t)(stTimeoutItem_t *);

struct stTimeoutItem_t {
  enum {
    eMaxTimeout = 40 * 1000 /*> 最长超时时间: 40s */
  };
  stTimeoutItem_t *pPrev;     /*> 指向上一个结点 */
  stTimeoutItem_t *pNext;     /*> 指向下一个结点 */
  stTimeoutItemLink_t *pLink; /*> 指向本结点所属的链表 */

  unsigned long long ullExpireTime; /*> 超时时间 */

  OnPreparePfn_t pfnPrepare; /* 预处理函数，在eventloop中会被调用 */
  OnProcessPfn_t pfnProcess; /* 处理函数，在eventloop中会被调用 */

  void *pArg;    /*> 预处理函数和处理函数的参数 */
  bool bTimeout; /*> 本结点是否已经超时 */
};

struct stTimeoutItemLink_t {
  stTimeoutItem_t *head; /*> 指向链表头部 */
  stTimeoutItem_t *tail; /*> 指向链表尾部 */
};

struct stTimeout_t {
  stTimeoutItemLink_t *pItems; /*> 时间轮，是一个由多个超时事件链表组成的数组 */
  int iItemSize; /*> 时间轮的大小，默认为60*1000 */

  unsigned long long ullStart; /*> 目前本超时管理器中最早的超时事件的超时时间 */
  long long llStartIdx; /*> 目前本超时管理器中最早的超时事件在链表中的索引 */
};

stTimeout_t *AllocTimeout(int iSize) {
  stTimeout_t *lp = (stTimeout_t *)calloc(1, sizeof(stTimeout_t));

  lp->iItemSize = iSize;
  lp->pItems = (stTimeoutItemLink_t *)calloc(
      1, sizeof(stTimeoutItemLink_t) * lp->iItemSize);

  lp->ullStart = GetTickMS();
  lp->llStartIdx = 0;

  return lp;
}

void FreeTimeout(stTimeout_t *apTimeout) {
  free(apTimeout->pItems);
  free(apTimeout);
}

int AddTimeout(stTimeout_t *apTimeout, stTimeoutItem_t *apItem,
               unsigned long long allNow) {
  /**
   * 如果时间轮中目前最早的超时时间是0（这不太可能，apTimeout构造时ullStart就已经被赋值为了当前时间）
   * 则回绕最早超时时间和超时索引
   */
  if (apTimeout->ullStart == 0) {
    apTimeout->ullStart = allNow;
    apTimeout->llStartIdx = 0;
  }

  // 如果当前时间比时间轮中的最早超时时间还早（这不太可能，直接输出错误日志）
  if (allNow < apTimeout->ullStart) {
    co_log_err(
        "CO_ERR: AddTimeout line %d allNow %llu apTimeout->ullStart %llu",
        __LINE__, allNow, apTimeout->ullStart);

    return __LINE__;
  }

  // 如果当前时间已经超过了待添加的超时事件结点的超时时间，此时该事件已经超时，报告错误
  if (apItem->ullExpireTime < allNow) {
    co_log_err(
        "CO_ERR: AddTimeout line %d apItem->ullExpireTime %llu allNow %llu "
        "apTimeout->ullStart %llu",
        __LINE__, apItem->ullExpireTime, allNow, apTimeout->ullStart);

    return __LINE__;
  }

  // 计算时间差，单位为ms
  unsigned long long diff = apItem->ullExpireTime - apTimeout->ullStart;

  // 比最早超时的事件的超时时间还要长超过了一分钟（60*1000ms)，（溢出了。。。）报错
  // 不过这里又改为了将超时时间设置为最长支持的时间，然后添加。。。
  if (diff >= (unsigned long long)apTimeout->iItemSize) {
    diff = apTimeout->iItemSize - 1;
    co_log_err("CO_ERR: AddTimeout line %d diff %d", __LINE__, diff);

    // return __LINE__;
  }

  // 好的，把这个新结点添加到时间轮中对应的槽指向的链表的尾部
  AddTail(
      apTimeout->pItems + (apTimeout->llStartIdx + diff) % apTimeout->iItemSize,
      apItem);

  return 0;
}

/**
 * @brief 取出所有超时事件到apResult链表
 *
 * @param apTimeout 超时管理器
 * @param allNow 当前事件
 * @param apResult 指向的链表用于存储取出的超时事件
 */
inline void TakeAllTimeout(stTimeout_t *apTimeout, unsigned long long allNow,
                           stTimeoutItemLink_t *apResult) {
  // 如果最早超时时间和索引没有初始化，在这里初始化
  if (apTimeout->ullStart == 0) {
    apTimeout->ullStart = allNow;
    apTimeout->llStartIdx = 0;
  }

  // 如果当前时间还未到达最早超时时间，说明一个超时的事件都没发生，直接返回
  if (allNow < apTimeout->ullStart) {
    return;
  }

  // 现在肯定有事件超时了，超时的时间槽的数量可以通过时间差除以粒度得到
  int cnt = allNow - apTimeout->ullStart + 1;

  // 如果超时的时间槽的数量超过了时间轮的最大容量，把它限制为这个最大容量
  if (cnt > apTimeout->iItemSize) {
    cnt = apTimeout->iItemSize;
  }

  // 如果cnt小于0，肯定有问题（但是这里感觉已经肯定不会小于0了。。。）
  if (cnt < 0) {
    return;
  }

  // 好的，现在开始遍历这些时间槽各自指向的链表吧，把这些链表全部splice到apResult
  for (int i = 0; i < cnt; i++) {
    int idx = (apTimeout->llStartIdx + i) % apTimeout->iItemSize;
    Join<stTimeoutItem_t, stTimeoutItemLink_t>(apResult,
                                               apTimeout->pItems + idx);
  }

  // 现在更新最早超时时间和最早超时索引
  apTimeout->ullStart = allNow;
  apTimeout->llStartIdx += cnt - 1;
}

static int CoRoutineFunc(stCoRoutine_t *co, void *) {
  if (co->pfn) {
    co->pfn(co->arg);
  }
  co->cEnd = 1;

  stCoRoutineEnv_t *env = co->env;

  co_yield_env(env);

  return 0;
}

/**
 * @brief 根据协程管理器env, 新建一个协程
 *
 * @param env  协程所在线程的环境
 * @param attr 协程属性，目前主要是共享栈
 * @param pfn  协程所运行的函数
 * @param arg  协程运行函数的参数
 */
struct stCoRoutine_t *co_create_env(stCoRoutineEnv_t *env,
                                    const stCoRoutineAttr_t *attr,
                                    pfn_co_routine_t pfn, void *arg) {
  stCoRoutineAttr_t at;
  
  // 如果attr不为NULL，设置协程属性（主要是共享栈）
  if (attr) {
    memcpy(&at, attr, sizeof(at));
  }

  // 限制栈的大小为合理的值
  if (at.stack_size <= 0) {
    at.stack_size = 128 * 1024;
  } else if (at.stack_size > 1024 * 1024 * 8) {
    at.stack_size = 1024 * 1024 * 8;
  }

  // 栈大小对齐
  if (at.stack_size & 0xFFF) {
    at.stack_size &= ~0xFFF;
    at.stack_size += 0x1000;
  }

  stCoRoutine_t *lp = (stCoRoutine_t *)malloc(sizeof(stCoRoutine_t));

  memset(lp, 0, (long)(sizeof(stCoRoutine_t)));

  lp->env = env;
  lp->pfn = pfn;
  lp->arg = arg;

  stStackMem_t *stack_mem = NULL;
  if (at.share_stack) {
    stack_mem = co_get_stackmem(at.share_stack);
    at.stack_size = at.share_stack->stack_size;
  } else {
    stack_mem = co_alloc_stackmem(at.stack_size);
  }
  lp->stack_mem = stack_mem;

  lp->ctx.ss_sp = stack_mem->stack_buffer;
  lp->ctx.ss_size = at.stack_size;

  lp->cStart = 0;
  lp->cEnd = 0;
  lp->cIsMain = 0;
  lp->cEnableSysHook = 0;
  lp->cIsShareStack = at.share_stack != NULL;

  lp->save_size = 0;
  lp->save_buffer = NULL;

  return lp;
}

int co_create(stCoRoutine_t **ppco, const stCoRoutineAttr_t *attr,
              pfn_co_routine_t pfn, void *arg) {
  if (!co_get_curr_thread_env()) {
    co_init_curr_thread_env();
  }
  stCoRoutine_t *co = co_create_env(co_get_curr_thread_env(), attr, pfn, arg);
  *ppco = co;
  return 0;
}
void co_free(stCoRoutine_t *co) {
  if (!co->cIsShareStack) {
    free(co->stack_mem->stack_buffer);
    free(co->stack_mem);
  }
  // walkerdu fix at 2018-01-20
  // 存在内存泄漏
  else {
    if (co->save_buffer) free(co->save_buffer);

    if (co->stack_mem->occupy_co == co) co->stack_mem->occupy_co = NULL;
  }

  free(co);
}
void co_release(stCoRoutine_t *co) { co_free(co); }

void co_swap(stCoRoutine_t *curr, stCoRoutine_t *pending_co);

void co_resume(stCoRoutine_t *co) {
  stCoRoutineEnv_t *env = co->env;
  stCoRoutine_t *lpCurrRoutine = env->pCallStack[env->iCallStackSize - 1];
  if (!co->cStart) {
    coctx_make(&co->ctx, (coctx_pfn_t)CoRoutineFunc, co, 0);
    co->cStart = 1;
  }
  env->pCallStack[env->iCallStackSize++] = co;
  co_swap(lpCurrRoutine, co);
}

// walkerdu 2018-01-14
// 用于reset超时无法重复使用的协程
void co_reset(stCoRoutine_t *co) {
  if (!co->cStart || co->cIsMain) return;

  co->cStart = 0;
  co->cEnd = 0;

  // 如果当前协程有共享栈被切出的buff，要进行释放
  if (co->save_buffer) {
    free(co->save_buffer);
    co->save_buffer = NULL;
    co->save_size = 0;
  }

  // 如果共享栈被当前协程占用，要释放占用标志，否则被切换，会执行save_stack_buffer()
  if (co->stack_mem->occupy_co == co) co->stack_mem->occupy_co = NULL;
}

void co_yield_env(stCoRoutineEnv_t *env) {
  stCoRoutine_t *last = env->pCallStack[env->iCallStackSize - 2];
  stCoRoutine_t *curr = env->pCallStack[env->iCallStackSize - 1];

  env->iCallStackSize--;

  co_swap(curr, last);
}

void co_yield_ct() { co_yield_env(co_get_curr_thread_env()); }
void co_yield (stCoRoutine_t *co) { co_yield_env(co->env); }

void save_stack_buffer(stCoRoutine_t *occupy_co) {
  /// copy out
  stStackMem_t *stack_mem = occupy_co->stack_mem;
  int len = stack_mem->stack_bp - occupy_co->stack_sp;

  if (occupy_co->save_buffer) {
    free(occupy_co->save_buffer), occupy_co->save_buffer = NULL;
  }

  occupy_co->save_buffer = (char *)malloc(len);  // malloc buf;
  occupy_co->save_size = len;

  memcpy(occupy_co->save_buffer, occupy_co->stack_sp, len);
}

void co_swap(stCoRoutine_t *curr, stCoRoutine_t *pending_co) {
  stCoRoutineEnv_t *env = co_get_curr_thread_env();

  // get curr stack sp
  char c;
  curr->stack_sp = &c;

  if (!pending_co->cIsShareStack) {
    env->pending_co = NULL;
    env->occupy_co = NULL;
  } else {
    env->pending_co = pending_co;
    // get last occupy co on the same stack mem
    stCoRoutine_t *occupy_co = pending_co->stack_mem->occupy_co;
    // set pending co to occupy thest stack mem;
    pending_co->stack_mem->occupy_co = pending_co;

    env->occupy_co = occupy_co;
    if (occupy_co && occupy_co != pending_co) {
      save_stack_buffer(occupy_co);
    }
  }

  // swap context
  coctx_swap(&(curr->ctx), &(pending_co->ctx));

  // stack buffer may be overwrite, so get again;
  stCoRoutineEnv_t *curr_env = co_get_curr_thread_env();
  stCoRoutine_t *update_occupy_co = curr_env->occupy_co;
  stCoRoutine_t *update_pending_co = curr_env->pending_co;

  if (update_occupy_co && update_pending_co &&
      update_occupy_co != update_pending_co) {
    // resume stack buffer
    if (update_pending_co->save_buffer && update_pending_co->save_size > 0) {
      memcpy(update_pending_co->stack_sp, update_pending_co->save_buffer,
             update_pending_co->save_size);
    }
  }
}

// int poll(struct pollfd fds[], nfds_t nfds, int timeout);
//  { fd,events,revents }
struct stPollItem_t;
struct stPoll_t : public stTimeoutItem_t {
  struct pollfd *fds;
  nfds_t nfds;  // typedef unsigned long int nfds_t;

  stPollItem_t *pPollItems;

  int iAllEventDetach;

  int iEpollFd;

  int iRaiseCnt;
};
struct stPollItem_t : public stTimeoutItem_t {
  struct pollfd *pSelf;
  stPoll_t *pPoll;

  struct epoll_event stEvent;
};
/*
 *   EPOLLPRI 		POLLPRI    // There is urgent data to read.
 *   EPOLLMSG 		POLLMSG
 *
 *   				POLLREMOVE
 *   				POLLRDHUP
 *   				POLLNVAL
 *
 * */
static uint32_t PollEvent2Epoll(short events) {
  uint32_t e = 0;
  if (events & POLLIN) e |= EPOLLIN;
  if (events & POLLOUT) e |= EPOLLOUT;
  if (events & POLLHUP) e |= EPOLLHUP;
  if (events & POLLERR) e |= EPOLLERR;
  if (events & POLLRDNORM) e |= EPOLLRDNORM;
  if (events & POLLWRNORM) e |= EPOLLWRNORM;
  return e;
}
static short EpollEvent2Poll(uint32_t events) {
  short e = 0;
  if (events & EPOLLIN) e |= POLLIN;
  if (events & EPOLLOUT) e |= POLLOUT;
  if (events & EPOLLHUP) e |= POLLHUP;
  if (events & EPOLLERR) e |= POLLERR;
  if (events & EPOLLRDNORM) e |= POLLRDNORM;
  if (events & EPOLLWRNORM) e |= POLLWRNORM;
  return e;
}

// __thread是GNU C支持的thread_local,只能通过指针引用，然后需要动态创建
static __thread stCoRoutineEnv_t *gCoEnvPerThread = NULL;

void co_init_curr_thread_env() {
  gCoEnvPerThread = (stCoRoutineEnv_t *)calloc(1, sizeof(stCoRoutineEnv_t));
  stCoRoutineEnv_t *env = gCoEnvPerThread;

  env->iCallStackSize = 0;
  struct stCoRoutine_t *self = co_create_env(env, NULL, NULL, NULL);
  self->cIsMain = 1;

  env->pending_co = NULL;
  env->occupy_co = NULL;

  coctx_init(&self->ctx);

  env->pCallStack[env->iCallStackSize++] = self;

  stCoEpoll_t *ev = AllocEpoll();
  SetEpoll(env, ev);
}

// 获取当前线程的协程管理器
stCoRoutineEnv_t *co_get_curr_thread_env() { return gCoEnvPerThread; }

void OnPollProcessEvent(stTimeoutItem_t *ap) {
  stCoRoutine_t *co = (stCoRoutine_t *)ap->pArg;
  co_resume(co);
}

void OnPollPreparePfn(stTimeoutItem_t *ap, struct epoll_event &e,
                      stTimeoutItemLink_t *active) {
  stPollItem_t *lp = (stPollItem_t *)ap;
  lp->pSelf->revents = EpollEvent2Poll(e.events);

  stPoll_t *pPoll = lp->pPoll;
  pPoll->iRaiseCnt++;

  if (!pPoll->iAllEventDetach) {
    pPoll->iAllEventDetach = 1;

    RemoveFromLink<stTimeoutItem_t, stTimeoutItemLink_t>(pPoll);

    AddTail(active, pPoll);
  }
}

void co_eventloop(stCoEpoll_t *ctx, pfn_co_eventloop_t pfn, void *arg) {
  if (!ctx->result) {
    ctx->result = co_epoll_res_alloc(stCoEpoll_t::_EPOLL_SIZE);
  }
  co_epoll_res *result = ctx->result;

  for (;;) {
    int ret = co_epoll_wait(ctx->iEpollFd, result, stCoEpoll_t::_EPOLL_SIZE, 1);

    stTimeoutItemLink_t *active = (ctx->pstActiveList);
    stTimeoutItemLink_t *timeout = (ctx->pstTimeoutList);

    memset(timeout, 0, sizeof(stTimeoutItemLink_t));

    for (int i = 0; i < ret; i++) {
      stTimeoutItem_t *item = (stTimeoutItem_t *)result->events[i].data.ptr;
      if (item->pfnPrepare) {
        item->pfnPrepare(item, result->events[i], active);
      } else {
        AddTail(active, item);
      }
    }

    unsigned long long now = GetTickMS();
    TakeAllTimeout(ctx->pTimeout, now, timeout);

    stTimeoutItem_t *lp = timeout->head;
    while (lp) {
      // printf("raise timeout %p\n",lp);
      lp->bTimeout = true;
      lp = lp->pNext;
    }

    Join<stTimeoutItem_t, stTimeoutItemLink_t>(active, timeout);

    lp = active->head;
    while (lp) {
      PopHead<stTimeoutItem_t, stTimeoutItemLink_t>(active);
      if (lp->bTimeout && now < lp->ullExpireTime) {
        int ret = AddTimeout(ctx->pTimeout, lp, now);
        if (!ret) {
          lp->bTimeout = false;
          lp = active->head;
          continue;
        }
      }
      if (lp->pfnProcess) {
        lp->pfnProcess(lp);
      }

      lp = active->head;
    }
    if (pfn) {
      if (-1 == pfn(arg)) {
        break;
      }
    }
  }
}

void OnCoroutineEvent(stTimeoutItem_t *ap) {
  stCoRoutine_t *co = (stCoRoutine_t *)ap->pArg;
  co_resume(co);
}

stCoEpoll_t *AllocEpoll() {
  stCoEpoll_t *ctx = (stCoEpoll_t *)calloc(1, sizeof(stCoEpoll_t));

  ctx->iEpollFd = co_epoll_create(stCoEpoll_t::_EPOLL_SIZE);
  ctx->pTimeout = AllocTimeout(60 * 1000);

  ctx->pstActiveList =
      (stTimeoutItemLink_t *)calloc(1, sizeof(stTimeoutItemLink_t));
  ctx->pstTimeoutList =
      (stTimeoutItemLink_t *)calloc(1, sizeof(stTimeoutItemLink_t));

  return ctx;
}

void FreeEpoll(stCoEpoll_t *ctx) {
  if (ctx) {
    free(ctx->pstActiveList);
    free(ctx->pstTimeoutList);
    FreeTimeout(ctx->pTimeout);
    co_epoll_res_free(ctx->result);
  }
  free(ctx);
}

stCoRoutine_t *GetCurrCo(stCoRoutineEnv_t *env) {
  return env->pCallStack[env->iCallStackSize - 1];
}
stCoRoutine_t *GetCurrThreadCo() {
  stCoRoutineEnv_t *env = co_get_curr_thread_env();
  if (!env) return 0;
  return GetCurrCo(env);
}

typedef int (*poll_pfn_t)(struct pollfd fds[], nfds_t nfds, int timeout);
int co_poll_inner(stCoEpoll_t *ctx, struct pollfd fds[], nfds_t nfds,
                  int timeout, poll_pfn_t pollfunc) {
  if (timeout == 0) {
    return pollfunc(fds, nfds, timeout);
  }
  if (timeout < 0) {
    timeout = INT_MAX;
  }
  int epfd = ctx->iEpollFd;
  stCoRoutine_t *self = co_self();

  // 1.struct change
  stPoll_t &arg = *((stPoll_t *)malloc(sizeof(stPoll_t)));
  memset(&arg, 0, sizeof(arg));

  arg.iEpollFd = epfd;
  arg.fds = (pollfd *)calloc(nfds, sizeof(pollfd));
  arg.nfds = nfds;

  stPollItem_t arr[2];
  if (nfds < sizeof(arr) / sizeof(arr[0]) && !self->cIsShareStack) {
    arg.pPollItems = arr;
  } else {
    arg.pPollItems = (stPollItem_t *)malloc(nfds * sizeof(stPollItem_t));
  }
  memset(arg.pPollItems, 0, nfds * sizeof(stPollItem_t));

  arg.pfnProcess = OnPollProcessEvent;
  arg.pArg = GetCurrCo(co_get_curr_thread_env());

  // 2. add epoll
  for (nfds_t i = 0; i < nfds; i++) {
    arg.pPollItems[i].pSelf = arg.fds + i;
    arg.pPollItems[i].pPoll = &arg;

    arg.pPollItems[i].pfnPrepare = OnPollPreparePfn;
    struct epoll_event &ev = arg.pPollItems[i].stEvent;

    if (fds[i].fd > -1) {
      ev.data.ptr = arg.pPollItems + i;
      ev.events = PollEvent2Epoll(fds[i].events);

      int ret = co_epoll_ctl(epfd, EPOLL_CTL_ADD, fds[i].fd, &ev);
      if (ret < 0 && errno == EPERM && nfds == 1 && pollfunc != NULL) {
        if (arg.pPollItems != arr) {
          free(arg.pPollItems);
          arg.pPollItems = NULL;
        }
        free(arg.fds);
        free(&arg);
        return pollfunc(fds, nfds, timeout);
      }
    }
    // if fail,the timeout would work
  }

  // 3.add timeout

  unsigned long long now = GetTickMS();
  arg.ullExpireTime = now + timeout;
  int ret = AddTimeout(ctx->pTimeout, &arg, now);
  int iRaiseCnt = 0;
  if (ret != 0) {
    co_log_err(
        "CO_ERR: AddTimeout ret %d now %lld timeout %d arg.ullExpireTime %lld",
        ret, now, timeout, arg.ullExpireTime);
    errno = EINVAL;
    iRaiseCnt = -1;

  } else {
    co_yield_env(co_get_curr_thread_env());
    iRaiseCnt = arg.iRaiseCnt;
  }

  {
    // clear epoll status and memory
    RemoveFromLink<stTimeoutItem_t, stTimeoutItemLink_t>(&arg);
    for (nfds_t i = 0; i < nfds; i++) {
      int fd = fds[i].fd;
      if (fd > -1) {
        co_epoll_ctl(epfd, EPOLL_CTL_DEL, fd, &arg.pPollItems[i].stEvent);
      }
      fds[i].revents = arg.fds[i].revents;
    }

    if (arg.pPollItems != arr) {
      free(arg.pPollItems);
      arg.pPollItems = NULL;
    }

    free(arg.fds);
    free(&arg);
  }

  return iRaiseCnt;
}

int co_poll(stCoEpoll_t *ctx, struct pollfd fds[], nfds_t nfds,
            int timeout_ms) {
  return co_poll_inner(ctx, fds, nfds, timeout_ms, NULL);
}

void SetEpoll(stCoRoutineEnv_t *env, stCoEpoll_t *ev) { env->pEpoll = ev; }

stCoEpoll_t *co_get_epoll_ct() {
  if (!co_get_curr_thread_env()) {
    co_init_curr_thread_env();
  }
  return co_get_curr_thread_env()->pEpoll;
}

struct stHookPThreadSpec_t {
  stCoRoutine_t *co;
  void *value;

  enum { size = 1024 };
};
void *co_getspecific(pthread_key_t key) {
  stCoRoutine_t *co = GetCurrThreadCo();
  if (!co || co->cIsMain) {
    return pthread_getspecific(key);
  }
  return co->aSpec[key].value;
}
int co_setspecific(pthread_key_t key, const void *value) {
  stCoRoutine_t *co = GetCurrThreadCo();
  if (!co || co->cIsMain) {
    return pthread_setspecific(key, value);
  }
  co->aSpec[key].value = (void *)value;
  return 0;
}

void co_disable_hook_sys() {
  stCoRoutine_t *co = GetCurrThreadCo();
  if (co) {
    co->cEnableSysHook = 0;
  }
}
bool co_is_enable_sys_hook() {
  stCoRoutine_t *co = GetCurrThreadCo();
  return (co && co->cEnableSysHook);
}

stCoRoutine_t *co_self() { return GetCurrThreadCo(); }

// 条件变量
struct stCoCond_t;

// 等待条件变量的队列
struct stCoCondItem_t {
  stCoCondItem_t *pPrev;   /*> 指向上一个item */
  stCoCondItem_t *pNext;   /*> 指向下一个item */
  stCoCond_t *pLink;       /*> 指向该队列所等待的条件变量 */
  stTimeoutItem_t timeout; /*> 指定超时 */
};

// 条件变量链表
struct stCoCond_t {
  stCoCondItem_t *head; /*> 指向链表头部 */
  stCoCondItem_t *tail; /*> 指向链表尾部 */
};

static void OnSignalProcessEvent(stTimeoutItem_t *ap) {
  stCoRoutine_t *co = (stCoRoutine_t *)ap->pArg;
  co_resume(co);
}

stCoCondItem_t *co_cond_pop(stCoCond_t *link);
int co_cond_signal(stCoCond_t *si) {
  stCoCondItem_t *sp = co_cond_pop(si);
  if (!sp) {
    return 0;
  }
  RemoveFromLink<stTimeoutItem_t, stTimeoutItemLink_t>(&sp->timeout);

  AddTail(co_get_curr_thread_env()->pEpoll->pstActiveList, &sp->timeout);

  return 0;
}
int co_cond_broadcast(stCoCond_t *si) {
  for (;;) {
    stCoCondItem_t *sp = co_cond_pop(si);
    if (!sp) return 0;

    RemoveFromLink<stTimeoutItem_t, stTimeoutItemLink_t>(&sp->timeout);

    AddTail(co_get_curr_thread_env()->pEpoll->pstActiveList, &sp->timeout);
  }

  return 0;
}

int co_cond_timedwait(stCoCond_t *link, int ms) {
  stCoCondItem_t *psi = (stCoCondItem_t *)calloc(1, sizeof(stCoCondItem_t));
  psi->timeout.pArg = GetCurrThreadCo();
  psi->timeout.pfnProcess = OnSignalProcessEvent;

  if (ms > 0) {
    unsigned long long now = GetTickMS();
    psi->timeout.ullExpireTime = now + ms;

    int ret = AddTimeout(co_get_curr_thread_env()->pEpoll->pTimeout,
                         &psi->timeout, now);
    if (ret != 0) {
      free(psi);
      return ret;
    }
  }
  AddTail(link, psi);

  co_yield_ct();

  RemoveFromLink<stCoCondItem_t, stCoCond_t>(psi);
  free(psi);

  return 0;
}
stCoCond_t *co_cond_alloc() {
  return (stCoCond_t *)calloc(1, sizeof(stCoCond_t));
}
int co_cond_free(stCoCond_t *cc) {
  free(cc);
  return 0;
}

stCoCondItem_t *co_cond_pop(stCoCond_t *link) {
  stCoCondItem_t *p = link->head;
  if (p) {
    PopHead<stCoCondItem_t, stCoCond_t>(link);
  }
  return p;
}
