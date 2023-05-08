/*
* Tencent is pleased to support the open source community by making Libco available.

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

.globl coctx_swap
#if !defined( __APPLE__ )
.type  coctx_swap, @function
#endif
coctx_swap:

#if defined(__i386__)
# &(curr->ctx) in %edi, &(pending_co->ctx) in %esi
    movl 4(%esp), %eax
    movl %esp,  28(%eax)
    movl %ebp, 24(%eax)
    movl %esi, 20(%eax)
    movl %edi, 16(%eax)
    movl %edx, 12(%eax)
    movl %ecx, 8(%eax)
    movl %ebx, 4(%eax)


    movl 8(%esp), %eax
    movl 4(%eax), %ebx
    movl 8(%eax), %ecx
    movl 12(%eax), %edx
    movl 16(%eax), %edi
    movl 20(%eax), %esi
    movl 24(%eax), %ebp
    movl 28(%eax), %esp

	ret

#elif defined(__x86_64__)
# &(curr->ctx) in %rdi, &(pending_co->ctx) in %rsi
	  leaq (%rsp),%rax      # 保存 %rsp 到 %rax 寄存器
    movq %rax, 104(%rdi)  # 保存 %rax 到 curr->ctx.regs[13]
    movq %rbx, 96(%rdi)   # 保存 %rbx 到 curr->ctx.regs[12]
    movq %rcx, 88(%rdi)   # 保存 %rcx 到 curr->ctx.regs[11]
    movq %rdx, 80(%rdi)   # 保存 %rdx 到 curr->ctx.regs[10]
	  movq 0(%rax), %rax    # 取出栈顶的八字节到 %rax，这八字节是该汇编过程的返回地址
	  movq %rax, 72(%rdi)   # 保存返回地址到 curr->ctx.regs[9]
    movq %rsi, 64(%rdi)   # 保存 %rsi 到 curr->ctx.regs[8]
	  movq %rdi, 56(%rdi)   # 保存 %rdi 到 curr->ctx.regs[7]
    movq %rbp, 48(%rdi)   # 保存 %rbp 到 curr->ctx.regs[6]
    movq %r8, 40(%rdi)    # 保存 %r8  到 curr->ctx.regs[5]
    movq %r9, 32(%rdi)    # 保存 %r9  到 curr->ctx.regs[4]
    movq %r12, 24(%rdi)   # 保存 %r12 到 curr->ctx.regs[3]
    movq %r13, 16(%rdi)   # 保存 %r13 到 curr->ctx.regs[2]
    movq %r14, 8(%rdi)    # 保存 %r14 到 curr->ctx.regs[1]
    movq %r15, (%rdi)     # 保存 %r15 到 curr->ctx.regs[0]
	  xorq %rax, %rax       # 将 %rax 寄存器清零

    movq 48(%rsi), %rbp   # 恢复 pending_co 的 %rbp
    movq 104(%rsi), %rsp  # 恢复 pending_co 的 %rsp
    movq (%rsi), %r15     # 恢复 pending_co 的 %r15
    movq 8(%rsi), %r14    # 恢复 pending_co 的 %r14
    movq 16(%rsi), %r13   # 恢复 pending_co 的 %r13
    movq 24(%rsi), %r12   # 恢复 pending_co 的 %r12
    movq 32(%rsi), %r9    # 恢复 pending_co 的 %r9
    movq 40(%rsi), %r8    # 恢复 pending_co 的 %r8
    movq 56(%rsi), %rdi   # 恢复 pending_co 的 %rdi
    movq 80(%rsi), %rdx   # 恢复 pending_co 的 %rdx
    movq 88(%rsi), %rcx   # 恢复 pending_co 的 %rcx
    movq 96(%rsi), %rbx   # 恢复 pending_co 的 %rbx
		leaq 8(%rsp), %rsp    # 栈顶后退8字节，用于存放新的返回地址
		pushq 72(%rsi)        # 恢复 pending_co 的 返回地址

    movq 64(%rsi), %rsi   # 恢复 pending_co 的 %rsi
	ret
#endif
