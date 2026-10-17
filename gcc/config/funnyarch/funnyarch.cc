#define IN_TARGET_CODE 1

#include "config.h"
#include "system.h"
#include "coretypes.h"
#include "backend.h"
#include "target.h"
#include "rtl.h"
#include "tree.h"
#include "stringpool.h"
#include "attribs.h"
#include "df.h"
#include "regs.h"
#include "memmodel.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "varasm.h"
#include "calls.h"
#include "explow.h"
#include "expr.h"
#include "builtins.h"

/* This file should be included last.  */
#include "target-def.h"

#define LOSE_AND_RETURN(msgid, x)		\
  do						\
    {						\
      funnyarch_operand_lossage (msgid, x);		\
      return;					\
    } while (0)

/* Per-function machine data.  */
struct GTY(()) machine_function
 {
   /* number of bytes saved on the stack for registers like lr, rfp.. */
   int call_saved_reg_size;

   /* Number of bytes saved on the stack for callee saved registers.  */
   int callee_saved_reg_size;

   /* Number of bytes saved on the stack for local variables.  */
   int local_vars_size;

   /* The sum of 2 sizes: locals vars and padding byte for saving the
    * registers.  Used in expand_prologue () and expand_epilogue().  */
   int size_for_adjusting_sp;
 };

/* Emit an error message when we're in an asm, and a fatal error for
   "normal" insns.  Formatted output isn't easily implemented, since we
   use output_operand_lossage to output the actual message and handle the
   categorization of the error.  */

static void
funnyarch_operand_lossage (const char *msgid, rtx op)
{
  debug_rtx (op);
  output_operand_lossage ("%s", msgid);
}

/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */
static void funnyarch_compute_frame(void){
  /* For aligning the local variables.  */
  int stack_alignment = STACK_BOUNDARY / BITS_PER_UNIT;
  int padding_locals;
  int regno;

  /* Padding needed for each element of the frame.  */
  cfun->machine->local_vars_size = get_frame_size ();

  /* Align to the stack alignment.  */
  padding_locals = cfun->machine->local_vars_size % stack_alignment;
  if (padding_locals)
    padding_locals = stack_alignment - padding_locals;

  cfun->machine->local_vars_size += padding_locals;

  cfun->machine->call_saved_reg_size = 0;

  /* link register save */
  if (!crtl->is_leaf) {
    cfun->machine->call_saved_reg_size += 4;
  }

  /* frame pointer save */
  if (frame_pointer_needed) {
    cfun->machine->call_saved_reg_size += 4;
  }

  cfun->machine->callee_saved_reg_size = 0;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (df_regs_ever_live_p (regno) && (! call_used_or_fixed_reg_p (regno)))
      cfun->machine->callee_saved_reg_size += 4;

  cfun->machine->size_for_adjusting_sp =
    0 //crtl->args.pretend_args_size
    + cfun->machine->local_vars_size
    + (ACCUMULATE_OUTGOING_ARGS
       ? (HOST_WIDE_INT) crtl->outgoing_args_size : 0);
}


void
funnyarch_expand_prologue (void)
{
  rtx insn;

  /* push varargs */
  for (int i = 0; i < (crtl->args.pretend_args_size / GET_MODE_SIZE (SImode)); i++)
  {
    insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, FUNNYARCH_R7 - i)));
	  RTX_FRAME_RELATED_P (insn) = 1;
  }

  if (flag_stack_usage_info)
    current_function_static_stack_size = cfun->machine->size_for_adjusting_sp;

  /* Save link register */
  if (!crtl->is_leaf) {
    insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, FUNNYARCH_LR)));
	  RTX_FRAME_RELATED_P (insn) = 1;
  }

  /* Save frame pointer */
  if (frame_pointer_needed)
    {
      /* Push old frame pointer */
      insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, HARD_FRAME_POINTER_REGNUM)));
	    RTX_FRAME_RELATED_P (insn) = 1;

      /* Initialize RFP */
      insn = emit_insn (gen_rtx_SET (gen_rtx_REG (SImode, HARD_FRAME_POINTER_REGNUM),
                                     gen_rtx_REG (SImode, STACK_POINTER_REGNUM)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }

  /* Save callee-saved registers.  */
  for (int regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    {
      if (df_regs_ever_live_p (regno)
	  && !call_used_or_fixed_reg_p (regno))
	{
	  insn = emit_insn (gen_movsi_push (gen_rtx_REG (Pmode, regno)));
	  RTX_FRAME_RELATED_P (insn) = 1;
	}
    }

  if (cfun->machine->size_for_adjusting_sp > 0)
    {
      insn = emit_insn (gen_subsi3 (stack_pointer_rtx,
            stack_pointer_rtx,
            GEN_INT (cfun->machine->size_for_adjusting_sp)));
      RTX_FRAME_RELATED_P (insn) = 1;
    }
}

void
funnyarch_expand_epilogue (void)
{
  if (cfun->machine->size_for_adjusting_sp > 0)
    {
      emit_insn (gen_addsi3 (stack_pointer_rtx,
            stack_pointer_rtx,
            GEN_INT (cfun->machine->size_for_adjusting_sp)));
    }

  if (cfun->machine->callee_saved_reg_size != 0)
    {
  /* Restore callee-saved registers */
    for (int regno = FIRST_PSEUDO_REGISTER; regno-- > 0; ) {
      if (!call_used_or_fixed_reg_p (regno)
          && df_regs_ever_live_p (regno))
      {
        rtx preg = gen_rtx_REG (Pmode, regno);
        emit_insn (gen_movsi_pop (preg));
      }
    }
  }

  /* Restore frame pointer */
  if (frame_pointer_needed) {
      emit_insn (gen_movsi_pop (gen_rtx_REG (Pmode, HARD_FRAME_POINTER_REGNUM)));
  }

  /* Restore link register */
  if (!crtl->is_leaf) {
    emit_insn (gen_movsi_pop (gen_rtx_REG (Pmode, FUNNYARCH_LR)));
  }

  /* pop varargs */
  if (crtl->args.pretend_args_size) {
    emit_insn (gen_addsi3 (stack_pointer_rtx,
            stack_pointer_rtx,
            GEN_INT (crtl->args.pretend_args_size)));
  }

  emit_jump_insn (gen_returner ());
}


/* Implements the macro INITIAL_ELIMINATION_OFFSET, return the OFFSET.  */
int funnyarch_initial_elimination_offset(int from, int to) {
  int ret;

  // stack layout:
  // - caller stack args (rsp at the very entry)
  // - pretend args / pushed varargs (crtl->args.pretend_args_size)
  // - lr, rfp (cfun->machine->call_saved_reg_size)
  // - callee saved regs (cfun->machine->callee_saved_reg_size)
  // - local variables

  if ((from) == FRAME_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    {
      ret = -cfun->machine->callee_saved_reg_size;
    }
  else if (from == FRAME_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    {
      ret = cfun->machine->local_vars_size;
    }
  else if ((from) == ARG_POINTER_REGNUM && (to) == HARD_FRAME_POINTER_REGNUM)
    {
      ret = cfun->machine->call_saved_reg_size;
    }
  else if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
    {
      ret = cfun->machine->call_saved_reg_size + cfun->machine->callee_saved_reg_size + cfun->machine->local_vars_size;
    }
  else
    abort ();

  return ret;
}

/* Implement TARGET_PROMOTE_FUNCTION_MODE.  */

/* This function is equivalent to default_promote_function_mode_always_promote
   except that it returns a promoted mode even if type is NULL_TREE.  This is
   needed by libcalls which have no type (only a mode) such as fixed conversion
   routines that take a signed or unsigned char/short argument and convert it
   to a fixed type.  */

static machine_mode
funnyarch_promote_function_mode (const_tree type,
				 machine_mode mode,
				 int *punsignedp,
				 const_tree fntype ATTRIBUTE_UNUSED,
				 int for_return ATTRIBUTE_UNUSED)
{
  int unsignedp;

  if (type != NULL_TREE)
    return promote_mode (type, mode, punsignedp);

  unsignedp = *punsignedp;
  PROMOTE_MODE (mode, unsignedp, type);
  *punsignedp = unsignedp;
  return mode;
}

/* Worker function for TARGET_RETURN_IN_MEMORY.  */

static bool
funnyarch_return_in_memory (const_tree type, const_tree fntype ATTRIBUTE_UNUSED)
{
  const HOST_WIDE_INT size = int_size_in_bytes (type);
  return (size == -1 || size > 2 * UNITS_PER_WORD);
}

/* Return the fixed registers used for condition codes.  */

static bool
funnyarch_fixed_condition_code_regs (unsigned int *p1, unsigned int *p2)
{
  *p1 = FUNNYARCH_RF;
  *p2 = INVALID_REGNUM;
  return true;
}

/* Define how to find the value returned by a function.
   VALTYPE is the data type of the value (as a tree).
   If the precise function being called is known, FUNC is its
   FUNCTION_DECL; otherwise, FUNC is 0.

   We always return values in register r0 for funnyarch.  */

static rtx
funnyarch_function_value (const_tree valtype,
		      const_tree fntype_or_decl ATTRIBUTE_UNUSED,
		      bool outgoing ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (TYPE_MODE (valtype), FUNNYARCH_R0);
}

/* Define how to find the value returned by a library function.

   We always return values in register r0 for funnyarch.  */

static rtx
funnyarch_libcall_value (machine_mode mode,
                     const_rtx fun ATTRIBUTE_UNUSED)
{
  return gen_rtx_REG (mode, FUNNYARCH_R0);
}

/* Handle TARGET_FUNCTION_VALUE_REGNO_P.

   We always return values in register r0 for funnyarch.  */

static bool
funnyarch_function_value_regno_p (const unsigned int regno)
{
  return (regno == FUNNYARCH_R0);
}

/* Handle TARGET_FRAME_POINTER_REQUIRED. */

static bool
funnyarch_frame_pointer_required (void)
{
  /* If the function contains dynamic stack allocations, we need to
     use the frame pointer to access the static parts of the frame.  */
  if (cfun->calls_alloca)
    return true;

  return false;
}

/* Worker function for TARGET_STATIC_CHAIN.  */

static rtx
funnyarch_static_chain (const_tree ARG_UNUSED (fndecl_or_type), bool incoming_p)
{
  rtx addr, mem;

  if (incoming_p)
    addr = plus_constant (Pmode, arg_pointer_rtx, 2 * UNITS_PER_WORD);
  else
    addr = plus_constant (Pmode, stack_pointer_rtx, -UNITS_PER_WORD);

  mem = gen_rtx_MEM (Pmode, addr);
  MEM_NOTRAP_P (mem) = 1;

  return mem;
}

/* Return non-zero if the function argument described by ARG is to be
   passed by reference.  */

static bool
funnyarch_pass_by_reference (cumulative_args_t, const function_arg_info &arg)
{
  if (arg.aggregate_type_p ())
    return true;
  unsigned HOST_WIDE_INT size = arg.type_size_in_bytes ();
  return size > 4*6;
}

/* Some function arguments will only partially fit in the registers
   that hold arguments.  Given a new arg, return the number of bytes
   that fit in argument passing registers.  */

static int
funnyarch_arg_partial_bytes (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);
  int bytes_left, size;

  if (*cum >= 8)
    return 0;

  if (funnyarch_pass_by_reference (cum_v, arg))
    size = 4;
  else if (arg.type)
    {
      if (AGGREGATE_TYPE_P (arg.type))
	return 0;
      size = int_size_in_bytes (arg.type);
    }
  else
    size = GET_MODE_SIZE (arg.mode);

  bytes_left = (4 * 6) - ((*cum - 2) * 4);

  if (size > bytes_left)
    return bytes_left;
  else
    return 0;
}

/* Return the next register to be used to hold a function argument or
   NULL_RTX if there's no more space.  */

static rtx
funnyarch_function_arg (cumulative_args_t cum_v, const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  if (*cum < 8)
    return gen_rtx_REG (arg.mode, *cum);
  else
    return NULL_RTX;
}

static void
funnyarch_function_arg_advance (cumulative_args_t cum_v,
			    const function_arg_info &arg)
{
  CUMULATIVE_ARGS *cum = get_cumulative_args (cum_v);

  unsigned int arg_size = arg.mode != BLKmode ? GET_MODE_SIZE (arg.mode) : (unsigned int) int_size_in_bytes (arg.type);

  *cum = (*cum <= FUNNYARCH_R7 ? *cum + ((3 + arg_size) / 4) : *cum);
}

/* Helper function for `funnyarch_legitimate_address_p'.  */

static bool
funnyarch_reg_ok_for_base_p (const_rtx reg, bool strict_p)
{
  int regno = REGNO (reg);

  if (strict_p)
    return HARD_REGNO_OK_FOR_BASE_P (regno)
	   || HARD_REGNO_OK_FOR_BASE_P (reg_renumber[regno]);
  else
    return !HARD_REGISTER_NUM_P (regno)
	   || HARD_REGNO_OK_FOR_BASE_P (regno);
}

/* Worker function for TARGET_LEGITIMATE_ADDRESS_P.  */

static bool
funnyarch_legitimate_address_p (machine_mode mode ATTRIBUTE_UNUSED,
			    rtx x, bool strict_p,
			    addr_space_t as,
			    code_helper = ERROR_MARK)
{
  gcc_assert (ADDR_SPACE_GENERIC_P (as));

  if (GET_CODE(x) == PLUS
      && REG_P (XEXP (x, 0))
      && funnyarch_reg_ok_for_base_p (XEXP (x, 0), strict_p)
      && CONST_INT_P (XEXP (x, 1))
      && IN_RANGE (INTVAL (XEXP (x, 1)), -4096, 4095))
    return true;
  if (REG_P (x) && funnyarch_reg_ok_for_base_p (x, strict_p))
    return true;
  if (GET_CODE (x) == SYMBOL_REF
      || GET_CODE (x) == LABEL_REF
      || GET_CODE (x) == CONST)
    return true;
  return false;
}

/* Worker function for TARGET_SETUP_INCOMING_VARARGS.  */

static void
funnyarch_setup_incoming_varargs (cumulative_args_t cum_v,
			      const function_arg_info &arg,
			      int *pretend_size, int no_rtl)
{
  CUMULATIVE_ARGS local_cum;

  /* The caller has advanced CUM up to, but not beyond, the last named
     argument.  Advance a local copy of CUM past the last "real" named
     argument, to find out how many registers are left over.  */
  local_cum = *get_cumulative_args (cum_v);
  if (!TYPE_NO_NAMED_ARGS_STDARG_P (TREE_TYPE (current_function_decl))
      || arg.type != NULL_TREE)
    funnyarch_function_arg_advance (pack_cumulative_args (&local_cum), arg);

  int nargregs = 8;
  int nregs = nargregs - local_cum;

  *pretend_size = nregs < 0 ? 0 : GET_MODE_SIZE (SImode) * nregs;
}

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
funnyarch_init_machine_status (void)
{
  return ggc_cleared_alloc<machine_function> ();
}

/* The TARGET_OPTION_OVERRIDE worker.  */
static void
funnyarch_option_override (void)
{
  /* Set the per-function-data initializer.  */
  init_machine_status = funnyarch_init_machine_status;
}

/* Implement TARGET_CSTORE_MODE.  */

static scalar_int_mode
funnyarch_cstore_mode (enum insn_code icode ATTRIBUTE_UNUSED)
{
  return SImode;
}

/* Implement TARGET_LEGITIMATE_CONSTANT_P. */
static bool
funnyarch_legitimate_constant_p (machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  return true;
}

/* Implement TARGET_LEGITIMIZE_ADDRESS.

   Try machine-dependent ways of modifying an illegitimate address X
   to be legitimate.  If we find one, return the new, valid address.

   OLDX is the address as it was before break_out_memory_refs was called.
   In some cases it is useful to look at this to decide what needs to be done.

   MODE is the mode of the operand pointed to by X.
 */

static rtx
funnyarch_legitimize_address (rtx x, rtx oldx ATTRIBUTE_UNUSED,
			 machine_mode mode ATTRIBUTE_UNUSED)
{
  rtx constant_term = const0_rtx;

  x = eliminate_constant_term (x, &constant_term);

  if (GET_CODE (x) == PLUS)
  {
    if (GET_CODE (XEXP (x, 0)) == REG)
	  {
	    rtx temp = gen_reg_rtx (Pmode);
	    rtx val  = force_operand (XEXP (x, 1), temp);
	    if (val != temp)
	      emit_move_insn (temp, val);

	    x = gen_rtx_PLUS (Pmode, XEXP (x, 0), temp);
	  }
    else if (GET_CODE (XEXP (x, 1)) == REG)
	  {
	    rtx temp = gen_reg_rtx (Pmode);
	    rtx val  = force_operand (XEXP (x, 0), temp);
	    if (val != temp)
	      emit_move_insn (temp, val);

	    x = gen_rtx_PLUS (Pmode, temp, XEXP (x, 1));
	  }
  }

  if (constant_term != const0_rtx)
  {
    x = gen_rtx_PLUS (Pmode, x, constant_term);
  }

  return x;
}

/* Implement TARGET_LIBGCC_CMP_RETURN_MODE. */
static scalar_int_mode
funnnyarch_libgcc_cmp_return_mode (void)
{
  return SImode;
}

/* Implement TARGET_CANNOT_FORCE_CONST_MEM.  */

static bool
funnyarch_cannot_force_const_mem (machine_mode mode ATTRIBUTE_UNUSED, rtx x)
{
  return false;
}

/* The PRINT_OPERAND worker.  */

static void
funnyarch_print_operand (FILE *file, rtx x, int code)
{
  rtx operand = x;

  /* New code entries should just be added to the switch below.  If
     handling is finished, just return.  If handling was just a
     modification of the operand, the modified operand should be put in
     "operand", and then do a break to let default handling
     (zero-modifier) output the operand.  */

  switch (code)
    {
    case 0:
      /* No code, print as usual.  */
      break;
    case 'h':
      /* 32-bit value shifted to the right by 16 bits (lower 16 bits unset) */
      if (CONST_INT_P (x)) {
        unsigned HOST_WIDE_INT val = UINTVAL (x) & 0xffffffff;
        if ((val & 0xffff) != 0) {
          LOSE_AND_RETURN ("lower 16 bits set on %H operand", x);
        }
        unsigned HOST_WIDE_INT shifted_val = val >> 16;
        // output right here, if we feed this back into operand using say GEN_INT it'll sign extend and explode
        fprintf (file, "#" HOST_WIDE_INT_PRINT_UNSIGNED, shifted_val);
        return;
      } else {
        LOSE_AND_RETURN ("invalid operand for modifier %h", x);
      }
      break;
    case 'L':
      /* lower 16 bits of 32-bit value */
      if (CONST_INT_P (x)) {
        unsigned HOST_WIDE_INT val = UINTVAL (x) & 0xffffffff;
        unsigned HOST_WIDE_INT masked_val = val & 0xffff;
        // output right here, if we feed this back into operand using say GEN_INT it'll sign extend and explode
        fprintf (file, "#" HOST_WIDE_INT_PRINT_UNSIGNED, masked_val);
        return;
      } else {
        LOSE_AND_RETURN ("invalid operand for modifier %L", x);
      }
      break;
    case 'H':
      /* upper 16 bits of 32-bit value */
      if (CONST_INT_P (x)) {
        unsigned HOST_WIDE_INT val = UINTVAL (x) & 0xffffffff;
        unsigned HOST_WIDE_INT masked_val = val >> 16;
        // output right here, if we feed this back into operand using say GEN_INT it'll sign extend and explode
        fprintf (file, "#" HOST_WIDE_INT_PRINT_UNSIGNED, masked_val);
        return;
      } else {
        LOSE_AND_RETURN ("invalid operand for modifier %H", x);
      }
      break;
    default:
      LOSE_AND_RETURN ("invalid operand modifier letter", x);
    }

  /* Print an operand as without a modifier letter.  */
  switch (GET_CODE (operand))
    {
    case REG:
      if (REGNO (operand) > FUNNYARCH_RF) {
        internal_error ("internal error: bad register: %d", REGNO (operand));
      }
      fprintf (file, "%s", reg_names[REGNO (operand)]);
      return;

    case MEM:
      output_address (GET_MODE (XEXP (operand, 0)), XEXP (operand, 0));
      return;
    
    case CONST_INT:
      fprintf (file, "#");
      output_addr_const (file, operand);
      return;

    default:
      /* No need to handle all strange variants, let output_addr_const
	 do it for us.  */
      if (CONSTANT_P (operand))
	{
	  output_addr_const (file, operand);
	  return;
	}

      LOSE_AND_RETURN ("unexpected operand", x);
    }
}

/* The PRINT_OPERAND_ADDRESS worker.  */

static void
funnyarch_print_operand_address (FILE *file, machine_mode, rtx x)
{
  switch (GET_CODE (x))
    {
    case REG:
      fprintf (file, "%s", reg_names[REGNO (x)]);
      break;

    case PLUS:
      switch (GET_CODE (XEXP (x, 1)))
	{
	case CONST_INT:
	  fprintf (file, "%ld(%s)",
		   INTVAL(XEXP (x, 1)), reg_names[REGNO (XEXP (x, 0))]);
	  break;
	case SYMBOL_REF:
	  output_addr_const (file, XEXP (x, 1));
	  fprintf (file, "(%s)", reg_names[REGNO (XEXP (x, 0))]);
	  break;
	case CONST:
	  {
	    rtx plus = XEXP (XEXP (x, 1), 0);
	    if (GET_CODE (XEXP (plus, 0)) == SYMBOL_REF
		&& CONST_INT_P (XEXP (plus, 1)))
	      {
		output_addr_const(file, XEXP (plus, 0));
		fprintf (file,"+%ld(%s)", INTVAL (XEXP (plus, 1)),
			 reg_names[REGNO (XEXP (x, 0))]);
	      }
	    else
	      abort();
	  }
	  break;
	default:
	  abort();
	}
      break;

    default:
      output_addr_const (file, x);
      break;
    }
}

/* Implement TARGET_HARD_REGNO_MODE_OK. */
static bool
funnyarch_hard_regno_mode_ok (unsigned int regno, machine_mode mode)
{
  if (regno == FUNNYARCH_RF)
  {
    return mode == CCmode || mode == CCSIGNEDmode;
  }

  return GET_MODE_SIZE (mode) <= UNITS_PER_WORD;
}

bool
funnyarch_can_eliminate (const int from, const int to)
{
  if (from == FRAME_POINTER_REGNUM || from == ARG_POINTER_REGNUM)
  {
    if (to == HARD_FRAME_POINTER_REGNUM) {
      return frame_pointer_needed;
    }

    if (to == STACK_POINTER_REGNUM)
    {
      return !frame_pointer_needed;
    }
  }

  return false;
}

void
funnyarch_asm_named_section (const char *name, unsigned int flags, tree decl)
{
  char flagchars[11], *f = flagchars;
  unsigned int numeric_value = 0;

  fprintf (asm_out_file, ".section %s\n", name);
}

static bool
funnyarch_assemble_integer (rtx value, unsigned int size, int aligned_p)
{
  if (CONST_INT_P (value))
    {
      const char *op = integer_asm_op (size, aligned_p);
      if (op) {
        fputs (op, asm_out_file);
        unsigned HOST_WIDE_INT pval = INTVAL (value);
        pval &= (1ul << (size * BITS_PER_UNIT)) - 1;
        fprintf (asm_out_file, HOST_WIDE_INT_PRINT_HEX "\n", pval);
        return true;
      }
    }
  return default_assemble_integer (value, size, aligned_p);
}

static bool
funnyarch_asm_output_addr_const_extra (FILE *file, rtx x)
{
  return false;
}

/* The Global `targetm' Variable.  */

/* Initialize the GCC target structure.  */

#undef  TARGET_PROMOTE_PROTOTYPES
#define TARGET_PROMOTE_PROTOTYPES	hook_bool_const_tree_true

#undef TARGET_PROMOTE_FUNCTION_MODE
#define TARGET_PROMOTE_FUNCTION_MODE funnyarch_promote_function_mode

#undef  TARGET_RETURN_IN_MEMORY
#define TARGET_RETURN_IN_MEMORY		funnyarch_return_in_memory
#undef  TARGET_MUST_PASS_IN_STACK
#define TARGET_MUST_PASS_IN_STACK	must_pass_in_stack_var_size
#undef  TARGET_PASS_BY_REFERENCE
#define TARGET_PASS_BY_REFERENCE        funnyarch_pass_by_reference
#undef  TARGET_ARG_PARTIAL_BYTES
#define TARGET_ARG_PARTIAL_BYTES        funnyarch_arg_partial_bytes
#undef  TARGET_FUNCTION_ARG
#define TARGET_FUNCTION_ARG		funnyarch_function_arg
#undef  TARGET_FUNCTION_ARG_ADVANCE
#define TARGET_FUNCTION_ARG_ADVANCE	funnyarch_function_arg_advance

#undef  TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P
#define TARGET_ADDR_SPACE_LEGITIMATE_ADDRESS_P	funnyarch_legitimate_address_p

#undef  TARGET_SETUP_INCOMING_VARARGS
#define TARGET_SETUP_INCOMING_VARARGS 	funnyarch_setup_incoming_varargs

#undef	TARGET_FIXED_CONDITION_CODE_REGS
#define	TARGET_FIXED_CONDITION_CODE_REGS funnyarch_fixed_condition_code_regs

#undef TARGET_FUNCTION_VALUE
#define TARGET_FUNCTION_VALUE funnyarch_function_value
#undef TARGET_LIBCALL_VALUE
#define TARGET_LIBCALL_VALUE funnyarch_libcall_value
#undef TARGET_FUNCTION_VALUE_REGNO_P
#define TARGET_FUNCTION_VALUE_REGNO_P funnyarch_function_value_regno_p

#undef TARGET_FRAME_POINTER_REQUIRED
#define TARGET_FRAME_POINTER_REQUIRED funnyarch_frame_pointer_required

#undef TARGET_COMPUTE_FRAME_LAYOUT
#define TARGET_COMPUTE_FRAME_LAYOUT funnyarch_compute_frame

#undef TARGET_STATIC_CHAIN
#define TARGET_STATIC_CHAIN funnyarch_static_chain
// #undef TARGET_ASM_TRAMPOLINE_TEMPLATE
// #define TARGET_ASM_TRAMPOLINE_TEMPLATE funnyarch_asm_trampoline_template
// #undef TARGET_TRAMPOLINE_INIT
// #define TARGET_TRAMPOLINE_INIT funnyarch_trampoline_init

#undef TARGET_OPTION_OVERRIDE
#define TARGET_OPTION_OVERRIDE funnyarch_option_override

#undef TARGET_LEGITIMATE_CONSTANT_P
#define TARGET_LEGITIMATE_CONSTANT_P funnyarch_legitimate_constant_p

#undef TARGET_LEGITIMIZE_ADDRESS
#define TARGET_LEGITIMIZE_ADDRESS funnyarch_legitimize_address

#undef TARGET_LIBGCC_CMP_RETURN_MODE
#define TARGET_LIBGCC_CMP_RETURN_MODE funnnyarch_libgcc_cmp_return_mode

#undef TARGET_CSTORE_MODE
#define TARGET_CSTORE_MODE funnyarch_cstore_mode

#undef TARGET_CANNOT_FORCE_CONST_MEM
#define TARGET_CANNOT_FORCE_CONST_MEM funnyarch_cannot_force_const_mem

#undef  TARGET_PRINT_OPERAND
#define TARGET_PRINT_OPERAND funnyarch_print_operand
#undef  TARGET_PRINT_OPERAND_ADDRESS
#define TARGET_PRINT_OPERAND_ADDRESS funnyarch_print_operand_address

#undef TARGET_HARD_REGNO_MODE_OK
#define TARGET_HARD_REGNO_MODE_OK funnyarch_hard_regno_mode_ok

#undef  TARGET_CONSTANT_ALIGNMENT
#define TARGET_CONSTANT_ALIGNMENT constant_alignment_word_strings

#undef TARGET_ASM_BYTE_OP
#define TARGET_ASM_BYTE_OP ".byte "
#undef TARGET_ASM_ALIGNED_HI_OP
#define TARGET_ASM_ALIGNED_HI_OP ".halfword "
#undef TARGET_ASM_ALIGNED_SI_OP
#define TARGET_ASM_ALIGNED_SI_OP ".word "
#undef TARGET_ASM_UNALIGNED_HI_OP
#define TARGET_ASM_UNALIGNED_HI_OP TARGET_ASM_ALIGNED_HI_OP
#undef TARGET_ASM_UNALIGNED_SI_OP
#define TARGET_ASM_UNALIGNED_SI_OP TARGET_ASM_ALIGNED_SI_OP
#undef TARGET_ASM_NAMED_SECTION
#define TARGET_ASM_NAMED_SECTION funnyarch_asm_named_section
#undef TARGET_ASM_INTEGER
#define TARGET_ASM_INTEGER funnyarch_assemble_integer
#undef TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA
#define TARGET_ASM_OUTPUT_ADDR_CONST_EXTRA funnyarch_asm_output_addr_const_extra

#undef TARGET_CAN_ELIMINATE
#define TARGET_CAN_ELIMINATE funnyarch_can_eliminate

struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-funnyarch.h"
