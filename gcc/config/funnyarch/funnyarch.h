/* Target Definitions for funnyarch.
   Copyright (C) 2015-2023 Free Software Foundation, Inc.

   This file is part of GCC.

   GCC is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published
   by the Free Software Foundation; either version 3, or (at your
   option) any later version.

   GCC is distributed in the hope that it will be useful, but WITHOUT
   ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
   License for more details.

   You should have received a copy of the GNU General Public License
   along with GCC; see the file COPYING3.  If not see
   <http://www.gnu.org/licenses/>.  */

#ifndef GCC_FUNNYARCH_H
#define GCC_FUNNYARCH_H

#undef STARTFILE_SPEC
#define STARTFILE_SPEC "%{!mno-crt0:crt0%O%s} crti.o%s crtbegin.o%s"

/* Provide an ENDFILE_SPEC appropriate for svr4.  Here we tack on our own
   magical crtend.o file (see crtstuff.c) which provides part of the
   support for getting C++ file-scope static object constructed before
   entering `main', followed by the normal svr3/svr4 "finalizer" file,
   which is either `gcrtn.o' or `crtn.o'.  */

#undef ENDFILE_SPEC
#define ENDFILE_SPEC "crtend.o%s crtn.o%s"

/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 32

#define FLOAT_TYPE_SIZE 32
#define DOUBLE_TYPE_SIZE 32
#define LONG_DOUBLE_TYPE_SIZE 32

#define DEFAULT_SIGNED_CHAR 0

#undef SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef WCHAR_TYPE
#define WCHAR_TYPE "long int"

#undef WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

#define REGISTER_NAMES                                                                \
   {                                                                                  \
      "r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7", "r8",                           \
          "r9", "r10", "r11", "r12", "r13", "r14", "r15", "r16", "r17", "r18", "r19", \
          "r20", "r21", "r22", "r23", "r24", "r25",                                   \
          "rfp", "iptr", "lr", "rsp", "rip", "rf"                                     \
   }

#define FUNNYARCH_R0 0
#define FUNNYARCH_R1 1
#define FUNNYARCH_R2 2
#define FUNNYARCH_R3 3
#define FUNNYARCH_R4 4
#define FUNNYARCH_R5 5
#define FUNNYARCH_R6 6
#define FUNNYARCH_R7 7
#define FUNNYARCH_R8 8
#define FUNNYARCH_R9 9
#define FUNNYARCH_R10 10
#define FUNNYARCH_R11 11
#define FUNNYARCH_R12 12
#define FUNNYARCH_R13 13
#define FUNNYARCH_R14 14
#define FUNNYARCH_R15 15
#define FUNNYARCH_R16 16
#define FUNNYARCH_R17 17
#define FUNNYARCH_R18 18
#define FUNNYARCH_R19 19
#define FUNNYARCH_R20 20
#define FUNNYARCH_R21 21
#define FUNNYARCH_R22 22
#define FUNNYARCH_R23 23
#define FUNNYARCH_R24 24
#define FUNNYARCH_R25 25
#define FUNNYARCH_RFP 26
#define FUNNYARCH_IPTR 27
#define FUNNYARCH_LR 28
#define FUNNYARCH_RSP 29
#define FUNNYARCH_RIP 30
#define FUNNYARCH_RF 31
#define FIRST_PSEUDO_REGISTER 32

enum reg_class
{
   NO_REGS,
   GENERAL_REGS,
   SPECIAL_REGS,
   CC_REGS,
   ALL_REGS,
   LIM_REG_CLASSES
};

#define REG_CLASS_CONTENTS                                  \
   {                                                        \
      {0x00000000, 0x00000000},     /* Empty */             \
          {0x03FFFFFF, 0x00000000}, /* General registers */ \
          {0xFC000000, 0x00000000}, /* Special registers */ \
          {0x80000000, 0x00000000}, /* CC */                \
      {                                                     \
         0xFFFFFFFF, 0x00000000                             \
      } /* All registers */                                 \
   }

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES   \
   {                      \
      "NO_REGS",          \
          "GENERAL_REGS", \
          "SPECIAL_REGS", \
          "CC_REGS",      \
          "ALL_REGS"      \
   }

#define FIXED_REGISTERS                      \
   {                                         \
      0, 0, 0, 0, /*  r0, r1, r2, r3 */      \
          /* r4  r5  r6  r7   */ 0, 0, 0, 0, \
          /* r8  r9  r10 r11  */ 0, 0, 0, 0, \
          /* r12 r13 r14 r15  */ 0, 0, 0, 0, \
          /* r16 r17 r18 r19  */ 0, 0, 0, 0, \
          /* r20 r21 r22 r23  */ 0, 0, 0, 0, \
          /* r24 r25 rfp iptr */ 0, 0, 1, 1, \
          /* lr  rsp rip rf   */ 1, 1, 1, 1, \
   }

/* funnyarch calling convention:
 * caller saves r0-r7
 * callee saves: r8-r31
 * args passed in: r0-r7 and then on stack
 * return value in r0
 */
#define FUNNYARCH_NUM_ARGS 8

#define CALL_USED_REGISTERS                  \
   {                                         \
      1, 1, 1, 1, /*  r0, r1, r2, r3 */      \
          /* r4  r5  r6  r7   */ 1, 1, 1, 1, \
          /* r8  r9  r10 r11  */ 0, 0, 0, 0, \
          /* r12 r13 r14 r15  */ 0, 0, 0, 0, \
          /* r16 r17 r18 r19  */ 0, 0, 0, 0, \
          /* r20 r21 r22 r23  */ 0, 0, 0, 0, \
          /* r24 r25 rfp iptr */ 0, 0, 1, 1, \
          /* lr  rsp rip rf   */ 1, 1, 1, 1, \
   }

/* We can't copy to or from our CC register. */
#define AVOID_CCMODE_COPIES 1

/* A C expression whose value is a register class containing hard
   register REGNO.  */
#define REGNO_REG_CLASS(R) ((R < FUNNYARCH_RFP) ? GENERAL_REGS : ((R == FUNNYARCH_RF) ? CC_REGS : SPECIAL_REGS))

/* The Overall Framework of an Assembler File */

#undef ASM_SPEC
#define ASM_COMMENT_START "//"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#define ASM_BYTE "\t.byte\t"

#define FILE_ASM_OP "\t.file\n"

/* The prefix to add for compiler private assembler symbols.  */
#undef LOCAL_LABEL_PREFIX
#define LOCAL_LABEL_PREFIX "."

/* Prefix for internally generated assembler labels.  */
#define LPREFIX ".L"

/* Switch to the text or data segment.  */
#define TEXT_SECTION_ASM_OP "\t.section text"
#define DATA_SECTION_ASM_OP "\t.section data"

/* This is how to output an assembler line
   that says to advance the location counter
   to a multiple of 2**POWER bytes.  */
#define ASM_OUTPUT_ALIGN(STREAM, POWER) \
   fprintf(STREAM, "\t.align\t%d\n", (1 << POWER));

/* Output and Generation of Labels */

#define GLOBAL_ASM_OP "\t.global\t"

/* Passing Arguments in Registers */

/* A C type for declaring a variable that is used as the first
   argument of `FUNCTION_ARG' and other related values.  */
#define CUMULATIVE_ARGS unsigned int

/* If defined, the maximum amount of space required for outgoing arguments
   will be computed and placed into the variable
   `current_function_outgoing_args_size'.  No space will be pushed
   onto the stack for each call; instead, the function prologue should
   increase the stack frame size by this amount.  */
#define ACCUMULATE_OUTGOING_ARGS 1

/* A C statement (sans semicolon) for initializing the variable CUM
   for the state at the beginning of the argument list.
   For funnyarch, the first arg is passed in r0.  */
#define INIT_CUMULATIVE_ARGS(CUM, FNTYPE, LIBNAME, FNDECL, N_NAMED_ARGS) \
   (CUM = FUNNYARCH_R0)

/* How Scalar Function Values Are Returned */

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
   pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this if the above stack space is to be considered part of the
   space allocated by the caller.  */
#define OUTGOING_REG_PARM_STACK_SPACE(FNTYPE) 1
#define STACK_PARMS_IN_REG_PARM_AREA

/* Define this if it is the responsibility of the caller to allocate
   the area reserved for arguments passed in registers.  */
#define REG_PARM_STACK_SPACE(FNDECL) (FUNNYARCH_NUM_ARGS * UNITS_PER_WORD)

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  */
#define FIRST_PARM_OFFSET(F) 0

/* Define this macro to nonzero value if the addresses of local variable slots
   are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 1

/* EXIT_IGNORE_STACK should be nonzero if, when returning from a function,
   the stack pointer does not matter.  The value is tested only in
   functions that have frame pointers.
   No definition is equivalent to always zero.  */

#define EXIT_IGNORE_STACK 0

/* Define this macro as a C expression that is nonzero for registers that are
   used by the epilogue or the return pattern.  The stack and frame
   pointer registers are already assumed to be used as needed.  */
#define EPILOGUE_USES(R) (R == FUNNYARCH_R0)

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX \
   gen_frame_mem(Pmode,          \
                 plus_constant(Pmode, stack_pointer_rtx, UNITS_PER_WORD))

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N) ((N) < FUNNYARCH_NUM_ARGS ? (N) : INVALID_REGNUM)

/* Store the return handler into the call frame.  */
#define EH_RETURN_HANDLER_RTX \
   gen_frame_mem(Pmode,       \
                 plus_constant(Pmode, frame_pointer_rtx, UNITS_PER_WORD))

/* Storage Layout */

#define BITS_BIG_ENDIAN 0
#define BYTES_BIG_ENDIAN 0
#define WORDS_BIG_ENDIAN 0

/* Alignment required for a function entry point, in bits.  */
#define FUNCTION_BOUNDARY 32

/* Define this macro as a C expression which is nonzero if accessing
   less than a word of memory (i.e. a `char' or a `short') is no
   faster than accessing a word of memory.  */
#define SLOW_BYTE_ACCESS 1

/* Number of storage units in a word; normally the size of a
   general-purpose register, a power of two from 1 or 8.  */
#define UNITS_PER_WORD 4

/* Define this macro to the minimum alignment enforced by hardware
   for the stack pointer on this machine.  The definition is a C
   expression for the desired alignment (measured in bits).  */
#define STACK_BOUNDARY 32

/* Normal alignment required for function parameters on the stack, in
   bits.  All stack parameters receive at least this much alignment
   regardless of data type.  */
#define PARM_BOUNDARY 32

/* Alignment of field after `int : 0' in a structure.  */
#define EMPTY_FIELD_BOUNDARY 32

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Every structures size must be a multiple of 32 bits.  */
#define STRUCTURE_SIZE_BOUNDARY 32

/* Largest integer machine mode for structures.  If undefined, the default
   is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 32

/* Make arrays of chars word-aligned for the same reasons.  */
#define DATA_ALIGNMENT(TYPE, ALIGN) \
   (TREE_CODE(TYPE) == ARRAY_TYPE && TYPE_MODE(TREE_TYPE(TYPE)) == QImode && (ALIGN) < FASTEST_ALIGNMENT ? FASTEST_ALIGNMENT : (ALIGN))

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT 1

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE, LABELNO) (abort(), 0)

// FIXME:
/* Trampolines for Nested Functions.  */
#define TRAMPOLINE_SIZE (2 + 6 + 6 + 2 + 2 + 6)

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 32

/* An alias for the machine mode for pointers.  */
#define Pmode SImode

/* An alias for the machine mode used for memory references to
   functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE QImode

/* The register number of the stack pointer register, which must also
   be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM FUNNYARCH_RSP

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM FUNNYARCH_RFP

// FIXME:
/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM FUNNYARCH_RFP

// TODO: in case fake registers are added define INITIAL_ELIMINATION_OFFSET and ELIMINABLE_REGS

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(r) (r >= FUNNYARCH_R0 && r <= FUNNYARCH_R7)

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
   ((unsigned)(NUM) < FIRST_PSEUDO_REGISTER && (REGNO_REG_CLASS(NUM) == GENERAL_REGS))

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM) \
   (HARD_REGNO_OK_FOR_BASE_P(NUM) || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM) \
   ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) ((NUM) <= FUNNYARCH_R25)

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 4

/* All load operations zero extend.  */
#define LOAD_EXTEND_OP(MEM) ZERO_EXTEND

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Run-time Target Specification */

#define TARGET_CPU_CPP_BUILTINS()      \
   {                                   \
      builtin_define("__FUNNYARCH__"); \
   }

#define HAS_LONG_UNCOND_BRANCH true

#endif /* GCC_FUNNYARCH_H */
