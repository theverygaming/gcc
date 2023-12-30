;; Machine description for funnyarch
;; Copyright (C) 2015-2023 Free Software Foundation, Inc.

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

;; -------------------------------------------------------------------------
;; funnyarch specific constraints, predicates and attributes
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; nop instruction
;; -------------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;; -------------------------------------------------------------------------
;; Arithmetic instructions
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Logical operators
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Shifters
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Move instructions
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Compare instructions
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Branch instructions
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Call and Jump instructions
;; -------------------------------------------------------------------------

;; -------------------------------------------------------------------------
;; Prologue & Epilogue
;; -------------------------------------------------------------------------

(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
{
  funnyarch_expand_prologue ();
  DONE;
})

(define_expand "epilogue"
  [(return)]
  ""
{
  funnyarch_expand_epilogue ();
  DONE;
})

(define_insn "returner"
  [(return)]
  ""
  "mov rip, lr")
