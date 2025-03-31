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
  "movdrm %0, %1")
