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


/* Implements the macro INITIAL_ELIMINATION_OFFSET */
int
funnyarch_initial_elimination_offset (int from, int to)
{
  // FIXME: broken!
  return 4;

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

struct gcc_target targetm = TARGET_INITIALIZER;
