/* Layout of Source Language Data Types */

#define INT_TYPE_SIZE 32
#define SHORT_TYPE_SIZE 16
#define LONG_TYPE_SIZE 32
#define LONG_LONG_TYPE_SIZE 64

#define DEFAULT_SIGNED_CHAR 0

#undef  SIZE_TYPE
#define SIZE_TYPE "unsigned int"

#undef  PTRDIFF_TYPE
#define PTRDIFF_TYPE "int"

#undef  WCHAR_TYPE
#define WCHAR_TYPE "unsigned int"

#undef  WCHAR_TYPE_SIZE
#define WCHAR_TYPE_SIZE BITS_PER_WORD

#define REGISTER_NAMES { \
  "r0", "r1", "r2", "r3", \
  "r4", "r5", "r6", "r7", \
  "r8", "r9", "r10", "r11", \
  "r12", "r13", "r14", "r15", \
  "r16", "r17", "r18", "r19", \
  "r20", "r21", "r22", "r23", \
  "r24", "r25", "r26", "rfp", \
  "lr", "rsp", "rip", "rf", \
  "?fp", "?ap" \
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
#define FUNNYARCH_R26 26
#define FUNNYARCH_RFP 27
#define FUNNYARCH_LR  28
#define FUNNYARCH_RSP 29
#define FUNNYARCH_RIP 30
#define FUNNYARCH_RF  31
#define FUNNYARCH_QFP 32
#define FUNNYARCH_QAP 33

#define FIRST_PSEUDO_REGISTER 34

enum reg_class
{
  NO_REGS,
  GENERAL_REGS,
  SPECIAL_REGS,
  CC_REGS,
  ALL_REGS,
  LIM_REG_CLASSES
};


#define REG_CLASS_CONTENTS \
{ { 0x00000000, 0x00000000 }, /* NO_REGS */			   \
  { 0xFFFFFFFF & ~(1ul << FUNNYARCH_RF), 0x00000000 | (1ul << (FUNNYARCH_QFP - 32)) | (1ul << (FUNNYARCH_QAP - 32)) }, /* GENERAL_REGS */ \
  { 0x00000000, 0x00000000 }, /* SPECIAL_REGS */ \
  { 0x00000000 | (1ul << (FUNNYARCH_RF)), 0x00000000 }, /* CC_REGS */ \
  { 0xFFFFFFFF, 0x00000003 } /* ALL_REGS */\
}

#define N_REG_CLASSES LIM_REG_CLASSES

#define REG_CLASS_NAMES {\
    "NO_REGS", \
    "GENERAL_REGS", \
    "SPECIAL_REGS", \
    "CC_REGS", \
    "ALL_REGS" }

#define FIXED_REGISTERS { \
  0, 0, 0, 0, /* r0-r3 */ \
  0, 0, 0, 0, /* r4-r7 */ \
  0, 0, 0, 0, /* r8-r11 */ \
  0, 0, 0, 0, /* r12-r5 */ \
  0, 0, 0, 0, /* r16-r19 */ \
  0, 0, 0, 0, /* r20-r23 */ \
  0, 0, 0, 1, /* r24-r26, rfp */ \
  1, 1, 1, 1, /* lr, rsp, rip, rf */ \
  1, 1 /* ?fp, ?ap */ \
}

// FIXME:
/*
 * calling convention:
 * caller saves r0-r7 (arguments)
 * callee saves: r8-r26, rfp, lr
 * args passed in: r0-r7 and then on stack (first non-register argument is on top of stack)
 * return value in r0
 */

#define CALL_USED_REGISTERS { \
  1, 1, 1, 1, /* r0-r3 */ \
  1, 1, 1, 1, /* r4-r7 */ \
  0, 0, 0, 0, /* r8-r11 */ \
  0, 0, 0, 0, /* r12-r5 */ \
  0, 0, 0, 0, /* r16-r19 */ \
  0, 0, 0, 0, /* r20-r23 */ \
  0, 0, 0, 1, /* r24-r26, rfp */ \
  1, 1, 1, 1, /* lr, rsp, rip, rf */ \
  1, 1 /* ?fp, ?ap */ \
}

/* We can't copy to or from our CC register. */
#define AVOID_CCMODE_COPIES 1

/* A C expression whose value is a register class containing hard
   register REGNO.  */
#define REGNO_REG_CLASS(R) ((R != FUNNYARCH_RF) ? GENERAL_REGS :		\
                            (R == FUNNYARCH_RF ? CC_REGS : SPECIAL_REGS))

/* The Overall Framework of an Assembler File */

#undef  ASM_SPEC
#define ASM_SPEC ""
#define ASM_COMMENT_START "//"
#define ASM_APP_ON ""
#define ASM_APP_OFF ""

#undef LINK_SPEC
#define LINK_SPEC ""

#define FILE_ASM_OP     "// file:\n"

#define TEXT_SECTION_ASM_OP  ".section .text"
#define DATA_SECTION_ASM_OP  ".section .data"

#define ASM_OUTPUT_ALIGN(STREAM,POWER) \
	fprintf (STREAM, ".align %d\n", (1 << POWER));

#define GLOBAL_ASM_OP ".global "

#undef TYPE_ASM_OP
#define TYPE_ASM_OP	"// type: "
#undef SIZE_ASM_OP
#define SIZE_ASM_OP	"// size: "

#undef TARGET_ASM_FILE_START_FILE_DIRECTIVE
#define TARGET_ASM_FILE_START_FILE_DIRECTIVE false

#undef TARGET_ASM_OUTPUT_IDENT
#define TARGET_ASM_OUTPUT_IDENT hook_void_constcharptr

#undef PREFERRED_DEBUGGING_TYPE
#define PREFERRED_DEBUGGING_TYPE NO_DEBUG

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
   for the state at the beginning of the argument list. */
#define INIT_CUMULATIVE_ARGS(CUM,FNTYPE,LIBNAME,FNDECL,N_NAMED_ARGS) \
  (CUM = FUNNYARCH_R0)

/* How Scalar Function Values Are Returned */

/* STACK AND CALLING */

/* Define this macro if pushing a word onto the stack moves the stack
   pointer to a smaller address.  */
#define STACK_GROWS_DOWNWARD 1

/* Define this if the above stack space is to be considered part of the
   space allocated by the caller.  */
#define OUTGOING_REG_PARM_STACK_SPACE(FNTYPE) 0
//#define STACK_PARMS_IN_REG_PARM_AREA

/* Define this if it is the responsibility of the caller to allocate
   the area reserved for arguments passed in registers.  */
#define REG_PARM_STACK_SPACE(FNDECL) 0 // (6 * UNITS_PER_WORD)

/* Offset from the argument pointer register to the first argument's
   address.  On some machines it may depend on the data type of the
   function.  */
#define FIRST_PARM_OFFSET(F) 0 //12

/* Define this macro to nonzero value if the addresses of local variable slots
   are at negative offsets from the frame pointer.  */
#define FRAME_GROWS_DOWNWARD 1

/* Define this macro as a C expression that is nonzero for registers that are
   used by the epilogue or the return pattern.  The stack and frame
   pointer registers are already assumed to be used as needed.  */
#define EPILOGUE_USES(R) (R == FUNNYARCH_LR)

/* A C expression whose value is RTL representing the location of the
   incoming return address at the beginning of any function, before
   the prologue.  */
#define INCOMING_RETURN_ADDR_RTX					\
  gen_frame_mem (Pmode,							\
		 plus_constant (Pmode, stack_pointer_rtx, UNITS_PER_WORD))

/* Describe how we implement __builtin_eh_return.  */
#define EH_RETURN_DATA_REGNO(N)	((N) < 4 ? (N+2) : INVALID_REGNUM)

/* Store the return handler into the call frame.  */
#define EH_RETURN_HANDLER_RTX						\
  gen_frame_mem (Pmode,							\
		 plus_constant (Pmode, frame_pointer_rtx, UNITS_PER_WORD))

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
#define EMPTY_FIELD_BOUNDARY  32

/* No data type wants to be aligned rounder than this.  */
#define BIGGEST_ALIGNMENT 32

/* The best alignment to use in cases where we have a choice.  */
#define FASTEST_ALIGNMENT 32

/* Every structures size must be a multiple of ... bits.  */
#define STRUCTURE_SIZE_BOUNDARY 32

/* Look at the fundamental type that is used for a bit-field and use
   that to impose alignment on the enclosing structure.
   struct s {int a:8}; should have same alignment as "int", not "char".  */
#define	PCC_BITFIELD_TYPE_MATTERS	1

/* Largest integer machine mode for structures.  If undefined, the default
   is GET_MODE_SIZE(DImode).  */
#define MAX_FIXED_MODE_SIZE 32

#define DATA_ALIGNMENT(TYPE, ALIGN)				\
  ((((ALIGN) < BITS_PER_WORD)					\
    && (TREE_CODE (TYPE) == ARRAY_TYPE					\
	|| TREE_CODE (TYPE) == UNION_TYPE				\
	|| TREE_CODE (TYPE) == RECORD_TYPE)) ? BITS_PER_WORD : (ALIGN))

/* Make sure that objects on the stack are sensibly aligned. */
#define LOCAL_ALIGNMENT(EXP, ALIGN) (((ALIGN) < BITS_PER_WORD) ? BITS_PER_WORD : (ALIGN))

/* Set this nonzero if move instructions will actually fail to work
   when given unaligned data.  */
#define STRICT_ALIGNMENT (!TARGET_UNALIGNED_ACCESS)

/* Generating Code for Profiling */
#define FUNCTION_PROFILER(FILE,LABELNO) (abort (), 0)

// FIXME: no trampoline??
/* Trampolines for Nested Functions.  */
#define TRAMPOLINE_SIZE 0

/* Alignment required for trampolines, in bits.  */
#define TRAMPOLINE_ALIGNMENT 32

/* An alias for the machine mode for pointers.  */
#define Pmode         SImode

#define PROMOTE_MODE(MODE, UNSIGNEDP, TYPE) \
do { \
  if (((MODE) == QImode) \
   || ((MODE) == HImode)) { \
       (MODE) = SImode;  \
   } \
} while (0)

/* An alias for the machine mode used for memory references to
   functions being called, in `call' RTL expressions.  */
#define FUNCTION_MODE SImode

/* Condition code information.  */
/* Given a comparison code (EQ, NE, etc.) and the first operand of a COMPARE,
   return the mode to be used for the comparison.  */

#define SELECT_CC_MODE(OP, X, Y) ((OP == LT || OP == GT || OP == LE || OP == GE) ? CCSIGNEDmode : CCmode)

/* The register number of the stack pointer register, which must also
   be a fixed register according to `FIXED_REGISTERS'.  */
#define STACK_POINTER_REGNUM FUNNYARCH_RSP

/* The register number of the frame pointer register, which is used to
   access automatic variables in the stack frame.  */
#define FRAME_POINTER_REGNUM FUNNYARCH_QFP

/* The register number of the arg pointer register, which is used to
   access the function's argument list.  */
#define ARG_POINTER_REGNUM FUNNYARCH_QAP

#define HARD_FRAME_POINTER_REGNUM FUNNYARCH_RFP

#define ELIMINABLE_REGS							\
{{ FRAME_POINTER_REGNUM, HARD_FRAME_POINTER_REGNUM },			\
 { FRAME_POINTER_REGNUM, STACK_POINTER_REGNUM },			   \
 { ARG_POINTER_REGNUM,   HARD_FRAME_POINTER_REGNUM },			\
 { ARG_POINTER_REGNUM,   STACK_POINTER_REGNUM }}

/* This macro returns the initial difference between the specified pair
   of registers.  */
#define INITIAL_ELIMINATION_OFFSET(FROM, TO, OFFSET)			\
  do {									\
    (OFFSET) = funnyarch_initial_elimination_offset ((FROM), (TO));		\
  } while (0)

/* A C expression that is nonzero if REGNO is the number of a hard
   register in which function arguments are sometimes passed.  */
#define FUNCTION_ARG_REGNO_P(r) (r >= FUNNYARCH_R0 && r <= FUNNYARCH_R7)

/* A macro whose definition is the name of the class to which a valid
   base register must belong.  A base register is one used in an
   address which is the register value plus a displacement.  */
#define BASE_REG_CLASS GENERAL_REGS

#define INDEX_REG_CLASS NO_REGS

#define HARD_REGNO_OK_FOR_BASE_P(NUM) \
  ((unsigned) (NUM) < FIRST_PSEUDO_REGISTER \
   && (REGNO_REG_CLASS(NUM) == GENERAL_REGS \
       || (NUM) == HARD_FRAME_POINTER_REGNUM))

/* A C expression which is nonzero if register number NUM is suitable
   for use as a base register in operand addresses.  */
#ifdef REG_OK_STRICT
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  (HARD_REGNO_OK_FOR_BASE_P(NUM) 		 \
   || HARD_REGNO_OK_FOR_BASE_P(reg_renumber[(NUM)]))
#else
#define REGNO_OK_FOR_BASE_P(NUM)		 \
  ((NUM) >= FIRST_PSEUDO_REGISTER || HARD_REGNO_OK_FOR_BASE_P(NUM))
#endif

/* A C expression which is nonzero if register number NUM is suitable
   for use as an index register in operand addresses.  */
#define REGNO_OK_FOR_INDEX_P(NUM) 0

/* The maximum number of bytes that a single instruction can move
   quickly between memory and registers or between two memory
   locations.  */
#define MOVE_MAX 4

/* Operations between registers always perform the operation
   on the full register even if a narrower mode is specified.  */
#define WORD_REGISTER_OPERATIONS 1

/* All load operations zero extend.  */
#define LOAD_EXTEND_OP(MODE) ZERO_EXTEND

/* A number, the maximum number of registers that can appear in a
   valid memory address.  */
#define MAX_REGS_PER_ADDRESS 1

/* An alias for a machine mode name.  This is the machine mode that
   elements of a jump-table should have.  */
#define CASE_VECTOR_MODE SImode

/* Run-time Target Specification */

#define TARGET_CPU_CPP_BUILTINS() \
  { \
    builtin_define_std ("__FUNNYARCH__");   \
  }

#define HAS_LONG_UNCOND_BRANCH true
