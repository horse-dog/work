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

#ifndef __CO_CTX_H__
#define __CO_CTX_H__
#include <stdlib.h>
typedef void *(*coctx_pfn_t)(void *s, void *s2);

// 这个结构体仅在32位时用到了，不管
struct coctx_param_t {
  const void *s1;
  const void *s2;
};

// 协程的上下文
struct coctx_t {
#if defined(__i386__)
  void *regs[8];
#else
  void *regs[14]; /*> 存储13个通用寄存器和rip，这里不需要r10，r11以及rax */
  // r10、r11和rax会被调用者保存
#endif
  size_t ss_size; /*> 该上下文使用的栈的大小 */
  char *ss_sp;	  /*> 该上下文使用的栈的栈顶 */
};

/**
 * @brief 初始化一个上下文
 *
 * @param ctx 指向一个上下文
 * @return int
 */
int coctx_init(coctx_t *ctx);

/**
 * @brief 使用pfn作为协程调度的函数，初始化一个上下文
 *
 * @param ctx 指向一个上下文
 * @param pfn 指向协程准备调度的函数
 * @param s	@p pfn 的第一个参数
 * @param s1 @p pfn 的第二个参数
 * @return int
 */
int coctx_make(coctx_t *ctx, coctx_pfn_t pfn, const void *s, const void *s1);
#endif
