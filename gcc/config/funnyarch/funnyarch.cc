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

/* Per-function machine data.  */
struct GTY (()) machine_function
{
  /* Number of bytes saved on the stack for callee saved registers.  */
  int callee_saved_reg_size;

  /* Number of bytes saved on the stack for local variables.  */
  int local_vars_size;

  /* The sum of 2 sizes: locals vars and padding byte for saving the
   * registers.  Used in expand_prologue () and expand_epilogue ().  */
  int size_for_adjusting_sp;
};

/* Zero initialization is OK for all current fields.  */

static struct machine_function *
funnyarch_init_machine_status (void)
{
  return ggc_cleared_alloc < machine_function > ();
}


/* The TARGET_OPTION_OVERRIDE worker.
   All this curently does is set init_machine_status.  */
static void
funnyarch_option_override (void)
{
  /* Set the per-function-data initializer.  */
  init_machine_status = funnyarch_init_machine_status;
}

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

// Must use LINK/UNLINK when...
// the frame is bigger than 512 bytes so cannot just "SUB" from SP
// the function actually uses $fp

static int
must_link (void)
{
  int bigframe = (cfun->machine->size_for_adjusting_sp >= 512);
  return (bigframe || frame_pointer_needed || df_regs_ever_live_p (FUNNYARCH_RFP)
          || df_regs_ever_live_p (FUNNYARCH_RFP));
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


// https://www.cse.iitb.ac.in/grc/intdocs/gcc-writing-md.html

/************************************************************/

/* This  function takes care  of destination operand of  a `move'
operation. */

int move_dest_operand(rtx op, enum machine_mode mode){
  rtx x0;
  rtx x1, x2;
  int retVal = FALSE;

  /* If the  mode of operand is not as required  and not equal to
     `VOIDmode' then return FALSE. */

  if(GET_MODE(op) != mode && mode != VOIDmode)
    retVal =  FALSE;

  /* If operand is an memory operand. */
  if(GET_CODE(op) == MEM) {
    x0 = XEXP(op,0);
    switch(GET_CODE(x0)) {

      /* If the operand is  an `PLUS' operand and its first child
         is an `REG' and second one is an `INT', then return
         TRUE. */

    case PLUS:
      x1 = XEXP(x0,0);
      x2 = XEXP(x0,1);
      if((GET_CODE(x2) == CONST_INT) && (GET_CODE(x1) == REG)) 
	retVal = TRUE;
      break;
				
      /*  If the  destination  is either  `REG'  or `SYMBOL'  and
          contain the address of some memory location, then
          return TRUE. */	 
      /*		case SYMBOL_REF:*/
    case REG:
      retVal = TRUE;		
      break;
			
    default:
      retVal = FALSE;
    }
  }			
  /*  If the  destination is  either `REG'  or  `SYMBOL_REF' then
      return TRUE. */

    else if (GET_CODE(op) == REG || GET_CODE(op) == SYMBOL_REF)
      retVal =  TRUE;
	
  return retVal;		
}

/************************************************************/
/*  This  function takes  care  of  source  operand of  a  `move'
    operation. */

int move_src_operand(rtx op,enum machine_mode mode){
  rtx x0;
  rtx x1,x2;
  int retVal= FALSE;
	
  /* If the  mode of operand is not as required  and not equal to
     `VOIDmode' then return FALSE. */
  if(GET_MODE(op) != mode && mode != VOIDmode)
    retVal =  FALSE;

  /* If operand is an memory operand. */
  if(GET_CODE(op) == MEM){
    x0 = XEXP(op,0);
    switch(GET_CODE(x0)){

      /* If the operand is  an `PLUS' operand and its first child
         is an `REG' and second one is an `INT', then return
         TRUE. */ 
    case PLUS:
      x1 = XEXP(x0,0);
      x2 = XEXP(x0,1);
      if((GET_CODE(x2) == CONST_INT) && (GET_CODE(x1) == REG)) 
	retVal = TRUE;
      break;
					
      /* If the destination is `REG' and contain the the address 
	 of some memory lacation, then return TRUE. */	
    case REG:
      retVal = TRUE;		
      break;

    default:
      retVal  = FALSE;
    }
  }			
  /* If the destination is `REG' or `SYMBOL_REF' or `CONST_INT',
     then return TRUE. */
  else{ 
    if(GET_CODE(op) == CC0 || 
       GET_CODE(op) == REG || 
       GET_CODE(op) == SYMBOL_REF || 
       GET_CODE(op) == CONST_INT){  	
      retVal = TRUE;
    }
  }

  return retVal;
}

/************************************************************/
/*  Function  to  generate  the  assembly  code  for  the  `move'
    instruction.*/
const char* toy_move_insn(rtx dest, rtx src){
  /* If destination is an `REG' operand. */	
  if (GET_CODE(dest) == REG) {
    if (GET_CODE(src) == MEM)
       return "mov\t %M1\t %0\t\t;; MOVE Dest = REG, Src = MEM";
    else
       return "mov\t %1\t %0\t\t;; MOVE Dest = REG, Src = REG";
  }
	
  if (GET_CODE(dest) == MEM) {
    if (GET_CODE(src) == MEM) 
      return "mov\t %M1\t %M0\t\t;; MOVE Dest = MEM, Src = MEM";
    else
      return "mov\t %1\t %M0\t\t;; MOVE Dest = MEM, Src = REG";
  }
}


struct gcc_target targetm = TARGET_INITIALIZER;

#include "gt-funnyarch.h"
