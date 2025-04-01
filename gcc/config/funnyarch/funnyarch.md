;; Machine description for funnyarch
;; Copyright (C) 2015-2025 Free Software Foundation, Inc.
;; Contributed by theverygaming

;; This file is part of GCC.

;; GCC is free software; you can redistribute it and/or modify it
;; under the terms of the GNU General Public License as published
;; by the Free Software Foundation; either version 3, or (at your
;; option) any later version.

;; GCC is distributed in the hope that it will be useful, but WITHOUT
;; ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
;; or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public
;; License for more details.

;; You should have received a copy of the GNU General Public License
;; along with GCC; see the file COPYING3.  If not see
;; <http://www.gnu.org/licenses/>.

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

(define_predicate "move_dest_operand"
  (match_code "mem,reg,subreg")
{
  rtx x0;
  rtx x1, x2;
  int retVal = FALSE;

  if(GET_MODE(op) != mode && mode != VOIDmode)
    retVal =  FALSE;

  if(GET_CODE(op) == MEM) {
    x0 = XEXP(op,0);
    switch(GET_CODE(x0)) {

    case PLUS:
      x1 = XEXP(x0,0);
      x2 = XEXP(x0,1);
      if((GET_CODE(x2) == CONST_INT) && (GET_CODE(x1) == REG)) 
	retVal = TRUE;
      break;

    case REG:
      retVal = TRUE;		
      break;
			
    default:
      retVal = FALSE;
    }
  }			
    else if (GET_CODE(op) == REG || GET_CODE(op) == SYMBOL_REF)
      retVal =  TRUE;
	
  return retVal;	
})

(define_predicate "move_src_operand"
  (match_code "mem,reg,subreg")
{
  rtx x0;
  rtx x1,x2;
  int retVal= FALSE;
	
  if(GET_MODE(op) != mode && mode != VOIDmode)
    retVal =  FALSE;

  if(GET_CODE(op) == MEM){
    x0 = XEXP(op,0);
    switch(GET_CODE(x0)){

    case PLUS:
      x1 = XEXP(x0,0);
      x2 = XEXP(x0,1);
      if((GET_CODE(x2) == CONST_INT) && (GET_CODE(x1) == REG)) 
	retVal = TRUE;
      break;
					
    case REG:
      retVal = TRUE;		
      break;

    default:
      retVal  = FALSE;
    }
  }			
  else{ 
    if(GET_CODE(op) == CC0 || 
       GET_CODE(op) == REG || 
       GET_CODE(op) == SYMBOL_REF || 
       GET_CODE(op) == CONST_INT){  	
      retVal = TRUE;
    }
  }

  return retVal;
})

(define_expand "movsi"
  [(set (match_operand:SI 0 "general_operand" "")
        (match_operand:SI 1 "general_operand" ""))
  ]
  ""
  "")
		
(define_insn "*movsi_1"
  [(set (match_operand:SI 0 "move_dest_operand" "")
        (match_operand:SI 1 "move_src_operand" ""))]
  ""
  "* 
	toy_move_insn (operands[0], operands[1]);
  ")

(define_insn "*movsi_1"
  [(set (match_operand:SI 0 "" "")
        (subreg:SI (match_operand:SI 1 "" "") 0))]
  ""
  ;;;;; "%0 = %1\t\t// MOV"
  "mov\t %1\t %0\t\t;; MOV"
)

;; Register-to-register move (fixed constraint)
(define_insn "mov_si"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (match_operand:SI 1 "register_operand" "r"))]
  ""
  "movrr %0, %1")

;; Immediate to register
(define_insn "mov_si_imm"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (match_operand:SI 1 "immediate_operand" "n"))]
  ""
  "movir %0, %1")

;; Memory to register
(define_insn "mov_si_mem"
  [(set (match_operand:SI 0 "register_operand" "=r")
        (match_operand:SI 1 "memory_operand" "m"))]
  ""
  "movmr %0, %1")

;; Register to memory
(define_insn "mov_si_store"
  [(set (match_operand:SI 0 "memory_operand" "=m")
        (match_operand:SI 1 "register_operand" "r"))]
  ""
  "movrm %0, %1")

;; Register-to-register move (fixed constraint)
(define_insn "mov_di"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operand:DI 1 "register_operand" "r"))]
  ""
  "movqrr %0, %1")

;; Immediate to register
(define_insn "mov_di_imm"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operand:DI 1 "immediate_operand" "n"))]
  ""
  "movdir %0, %1")

;; Memory to register
(define_insn "mov_di_mem"
  [(set (match_operand:DI 0 "register_operand" "=r")
        (match_operand:DI 1 "memory_operand" "m"))]
  ""
  "movdmr %0, %1")

;; Register to memory
(define_insn "mov_di_store"
  [(set (match_operand:DI 0 "memory_operand" "=m")
        (match_operand:DI 1 "register_operand" "r"))]
  ""
  "movqrm %0, %1")
