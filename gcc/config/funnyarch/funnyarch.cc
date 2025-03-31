/* Target code for funnyarch.
   Copyright (C) 2015-2025 Free Software Foundation, Inc.
   Contributed by theverygaming

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
#include "memmodel.h"
#include "tm_p.h"
#include "regs.h"
#include "emit-rtl.h"
#include "diagnostic-core.h"
#include "output.h"
#include "stor-layout.h"
#include "calls.h"
#include "expr.h"
#include "builtins.h"
#include "print-tree.h"

/* This file should be included last.  */
#include "target-def.h"

#include <stdint.h>

/* Compute the size of the local area and the size to be adjusted by the
 * prologue and epilogue.  */

static void
funnyarch_compute_frame (void)
{
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

  cfun->machine->callee_saved_reg_size = 0;

  /* Save callee-saved registers.  */
  for (regno = 0; regno < FIRST_PSEUDO_REGISTER; regno++)
    if (df_regs_ever_live_p (regno) && !call_used_or_fixed_reg_p (regno))
      cfun->machine->callee_saved_reg_size += 4;

  cfun->machine->size_for_adjusting_sp =
    0 // crtl->args.pretend_args_size
    + cfun->machine->local_vars_size
    + (ACCUMULATE_OUTGOING_ARGS
      ? (HOST_WIDE_INT) crtl->outgoing_args_size : 0);
}

/* Implements the macro INITIAL_ELIMINATION_OFFSET */
int
funnyarch_initial_elimination_offset (int from, int to)
{
  funnyarch_compute_frame ();

  if (from == ARG_POINTER_REGNUM && to == FRAME_POINTER_REGNUM)
  {
    return cfun->machine->callee_saved_reg_size + 2 * UNITS_PER_WORD;
  }

  if (from == ARG_POINTER_REGNUM && to == STACK_POINTER_REGNUM)
  {
    int arg_offset;
    arg_offset = must_link ()? 2 : 1;
    return ((cfun->machine->callee_saved_reg_size
              + arg_offset * UNITS_PER_WORD)
            + cfun->machine->size_for_adjusting_sp);
  }

  if ((from == FRAME_POINTER_REGNUM) && (to == STACK_POINTER_REGNUM))
  {
    return cfun->machine->size_for_adjusting_sp;
  }

  gcc_unreachable ();
}

void
funnyarch_print_operand (FILE * file, rtx x, int code)
{
  // FIXME: broken!
  rtx operand = x;
  fprintf (file, "funnyarch_print_operand");
  return;
  //LOSE_AND_RETURN ("unexpected operand", x);
}

void
funnyarch_print_operand_address (FILE * file, rtx x)
{
  // FIXME: broken!
  fprintf (file, "funnyarch_print_operand_address");
}

#undef TARGET_LEGITIMATE_ADDRESS_P
#define TARGET_LEGITIMATE_ADDRESS_P	funnyarch_legitimate_address_p

/* Return true if X is a legitimate address for values of mode MODE.
   STRICT_P says whether strict checking is needed.  */
bool
funnyarch_legitimate_address_p (machine_mode mode, rtx x, bool strict_p, code_helper)
{
  return true; /* FIXME: stuff */
}

struct gcc_target targetm = TARGET_INITIALIZER;
