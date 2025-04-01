/* Prototypes for funnyarch.cc functions used in the md file & elsewhere.
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

extern int  funnyarch_initial_elimination_offset (int, int);
extern void funnyarch_print_operand (FILE *, rtx, int);
extern void funnyarch_print_operand_address (FILE *, rtx);

extern int move_dest_operand(rtx op, enum machine_mode mode);
extern int move_src_operand(rtx op,enum machine_mode mode);
extern const char* toy_move_insn(rtx dest, rtx src);
