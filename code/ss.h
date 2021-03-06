/* ss.h: STACK SCANNING
 *
 * $Id$
 * Copyright (c) 2001-2020 Ravenbrook Limited.  See end of file for license.
 *
 * This module saves the mutator context on entry to the MPS, and
 * provides functions for decoding the context and scanning the root
 * registers. <design/stack-scan>.
 */

#ifndef ss_h
#define ss_h

#include "mpm.h"


/* StackContext -- some of the mutator's state
 *
 * The jumpBuffer is used to capture most of the mutator's state on
 * entry to the MPS, but can't capture it all.  See
 * <design/stack-scan#.sol.setjmp.scan>.
 */

#include <setjmp.h>

typedef struct StackContextStruct {
  jmp_buf jumpBuffer;
} StackContextStruct;


/* StackHot -- capture a hot stack pointer
 *
 * Sets *stackOut to a stack pointer that includes the current frame.
 */

void StackHot(void **stackOut);


/* STACK_CONTEXT_BEGIN -- save context */

#define STACK_CONTEXT_BEGIN(arena) \
  BEGIN \
    StackContextStruct _sc; \
    STACK_CONTEXT_SAVE(&_sc); \
    AVER(arena->stackWarm == NULL); \
    StackHot(&arena->stackWarm); \
    AVER(arena->stackWarm < (void *)&_sc); /* <code/ss.c#assume.desc> */ \
    BEGIN


/* STACK_CONTEXT_END -- clear context */

#define STACK_CONTEXT_END(arena) \
    END; \
    AVER(arena->stackWarm != NULL); \
    arena->stackWarm = NULL; \
  END


/* STACK_CONTEXT_SAVE -- save the callee-saves and stack pointer */

#if defined(MPS_OS_XC)

/* We call _setjmp rather than setjmp because we can be confident what
 * it does via the source code at
 * <https://opensource.apple.com/source/Libc/Libc-825.24/i386/sys/_setjmp.s>,
 * and because _setjmp saves only the register set and the stack while
 * setjmp also saves the signal mask, which we don't care about. See
 * _setjmp(2). */

#define STACK_CONTEXT_SAVE(sc) ((void)_setjmp((sc)->jumpBuffer))

#else  /* other platforms */

#define STACK_CONTEXT_SAVE(sc) ((void)setjmp((sc)->jumpBuffer))

#endif /* platform defines */


/* StackScan -- scan the mutator's stack and registers
 *
 * This must be called between STACK_CONTEXT_BEGIN and
 * STACK_CONTEXT_END.
 */

extern Res StackScan(ScanState ss, void *stackCold,
                      mps_area_scan_t scan_area, void *closure);


#endif /* ss_h */


/* C. COPYRIGHT AND LICENSE
 *
 * Copyright (C) 2001-2020 Ravenbrook Limited <http://www.ravenbrook.com/>.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *   notice, this list of conditions and the following disclaimer in the
 *   documentation and/or other materials provided with the
 *   distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 * IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
