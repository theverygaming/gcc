(include "constraints.md")
(include "predicates.md")

(define_constants
  [
    (RSP_REGNUM 29)
    (RF_REGNUM 31)
  ]
)

;; -------------------------------------------------------------------------
;; nop instruction
;; -------------------------------------------------------------------------

(define_insn "nop"
  [(const_int 0)]
  ""
  "nop")

;; -------------------------------------------------------------------------
;; Move instructions
;; -------------------------------------------------------------------------

(define_insn "movsi"
  [(set (match_operand:SI 0 "funnyarch_move_dest_operand" "=r,r,r,WMO13,r,r")
        (match_operand:SI 1 "funnyarch_move_source_operand" "r,I16nSI,I32nSI,r,WMO13,s"))]
  ""
  {
    switch (which_alternative)
      {
	case 0:
	  return "mov %0, %1";
	case 1:
	  return "mov %0, %1";
	case 2:
	  return "mov %0, %L1\n\tmovh %0, %H1";
  case 3:
	  {
        rtx addr = XEXP (operands[0], 0);
        if (REG_P (addr)) {
            return "str %0, %1, #0";
        } else if (GET_CODE (addr) == PLUS) {
            operands[2] = XEXP (addr, 0); // base register
            operands[3] = XEXP (addr, 1); // immediate offset
            return "str %2, %1, %3";
        }
        gcc_unreachable ();
      }
  case 4:
	  {
        rtx addr = XEXP (operands[1], 0);
        if (REG_P (addr)) {
            return "ldr %0, %1, #0";
        } else if (GET_CODE (addr) == PLUS) {
            operands[2] = XEXP (addr, 0); // base register
            operands[3] = XEXP (addr, 1); // immediate offset
            return "ldr %0, %2, %3";
        }
        gcc_unreachable ();
      }
  case 5:
	  return "mov %0, %1.low\n\tmovh %0, %1.high";
	default:
	  gcc_unreachable ();
      }
  })

(define_expand "movhi"
  [(set (match_operand:HI 0 "funnyarch_move_dest_operand")
        (match_operand:HI 1 "funnyarch_move_source_operand"))]
  ""
  {
    /* emulate 16-bit memory store with aligned 32-bit accesses */
    if (MEM_P (operands[0])) {
      rtx addr = XEXP (operands[0], 0);
      if (GET_CODE (operands[1]) != REG) {
        operands[1] = force_reg (HImode, operands[1]);
      }
      rtx val = gen_lowpart (SImode, operands[1]);

      if (TARGET_UNALIGNED_ACCESS || MEM_ALIGN (operands[0]) >= 32)
      {
        /* 32-bit load */
        rtx tmpvalreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpvalreg, gen_rtx_MEM (SImode, addr));

        /* prepare value */
        rtx tmpmaskreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpmaskreg, GEN_INT ((int32_t)0xFFFF0000));
        emit_insn (gen_andsi3 (tmpvalreg, tmpvalreg, tmpmaskreg));
        emit_insn (gen_iorsi3 (tmpvalreg, tmpvalreg, val));

        /* 32-bit store */
        emit_move_insn (gen_rtx_MEM (SImode, addr), tmpvalreg);
      }
      else
      {
        /* not supported rn. Would need to adapt to 2x load/store in that case.. */
        if (MEM_ALIGN (operands[0]) < 16)
        {
          gcc_unreachable ();
        }

        if (GET_CODE (addr) != REG) {
          addr = force_reg (SImode, addr);
        }

        rtx mask_reg = gen_reg_rtx (SImode);
        emit_move_insn (mask_reg, GEN_INT (0b11));

        /* calculate the misalignment */
        rtx misalignment = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (misalignment, addr, mask_reg));

        /* align the address */
        rtx addr_aligned = gen_reg_rtx (SImode);
        emit_insn (gen_subsi3 (addr_aligned, addr, misalignment));

        /* convert the misalignment to bits (shl by 3 == multiply by 8) */
        emit_insn (gen_ashlsi3 (misalignment, misalignment, GEN_INT (3)));

        /* prepare mask */
        rtx tmpmaskreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpmaskreg, GEN_INT (0xFFFF));
        emit_insn (gen_ashlsi3 (tmpmaskreg, tmpmaskreg, misalignment));

        /* prepare value */
        rtx tmpvalreg2 = gen_reg_rtx (SImode);
        emit_insn (gen_ashlsi3 (tmpvalreg2, val, misalignment));
        emit_insn (gen_andsi3 (tmpvalreg2, tmpvalreg2, tmpmaskreg));

        /* 32-bit load */
        rtx tmpvalreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpvalreg, gen_rtx_MEM (SImode, addr_aligned));
        emit_insn (gen_one_cmplsi2 (tmpmaskreg, tmpmaskreg)); // invert the mask for the read (unset the bits we wanna set later)
        emit_insn (gen_andsi3 (tmpvalreg, tmpvalreg, tmpmaskreg));

        /* or values together */
        emit_insn (gen_iorsi3 (tmpvalreg, tmpvalreg, tmpvalreg2));

        /* 32-bit store */
        emit_move_insn (gen_rtx_MEM (SImode, addr_aligned), tmpvalreg);
      }

      DONE;
    }
    /* emulate 16-bit memory load with aligned 32-bit accesses */
    else if (MEM_P (operands[1])) {
      rtx addr = XEXP (operands[1], 0);
      rtx val = gen_lowpart(SImode, operands[0]);

      if (TARGET_UNALIGNED_ACCESS || MEM_ALIGN (operands[1]) >= 32)
      {
        /* 32-bit load */
        emit_move_insn (val, gen_rtx_MEM (SImode, addr));

        /* mask */
        emit_insn (gen_andsi3 (val, val, GEN_INT (0xFFFF)));
      }
      else
      {
        /* not supported rn. Would need to adapt to 2x load/store in that case.. */
        if (MEM_ALIGN (operands[1]) < 16)
        {
          gcc_unreachable ();
        }

        if (GET_CODE (addr) != REG) {
          addr = force_reg (SImode, addr);
        }

        rtx mask_reg = gen_reg_rtx (SImode);
        emit_move_insn (mask_reg, GEN_INT (0b11));

        /* calculate the misalignment */
        rtx misalignment = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (misalignment, addr, mask_reg));

        /* align the address */
        rtx addr_aligned = gen_reg_rtx (SImode);
        emit_insn (gen_subsi3 (addr_aligned, addr, misalignment));

        /* convert the misalignment to bits (shl by 3 == multiply by 8) */
        emit_insn (gen_ashlsi3 (misalignment, misalignment, GEN_INT (3)));

        /* 32-bit load */
        emit_move_insn (val, gen_rtx_MEM (SImode, addr_aligned));

        /* shift */
        emit_insn (gen_ashrsi3 (val, val, misalignment));

        /* mask */
        emit_insn (gen_andsi3 (val, val, GEN_INT (0xFFFF)));
      }

      DONE;
    }
    /* in the remaining cases we can just do a 32-bit move */
    else if (REG_P (operands[0]) || REG_P (operands[1]))
    {
      emit_insn (gen_movsi (gen_lowpart (SImode, operands[0]), gen_lowpart (SImode, operands[1])));
      DONE;
    }
  })

(define_expand "movqi"
  [(set (match_operand:QI 0 "funnyarch_move_dest_operand")
        (match_operand:QI 1 "funnyarch_move_source_operand"))]
  ""
  {
    /* emulate 8-bit memory store with aligned 32-bit accesses */
    if (MEM_P (operands[0])) {
      rtx addr = XEXP (operands[0], 0);
      if (GET_CODE (operands[1]) != REG) {
        operands[1] = force_reg (QImode, operands[1]);
      }
      rtx val = gen_lowpart (SImode, operands[1]);

      if (TARGET_UNALIGNED_ACCESS || MEM_ALIGN (operands[0]) >= 32)
      {
        /* 32-bit load */
        rtx tmpvalreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpvalreg, gen_rtx_MEM (SImode, addr));

        /* prepare value */
        rtx tmpmaskreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpmaskreg, GEN_INT ((int32_t)0xFFFFFF00));
        emit_insn (gen_andsi3 (tmpvalreg, tmpvalreg, tmpmaskreg));
        emit_insn (gen_iorsi3 (tmpvalreg, tmpvalreg, val));

        /* 32-bit store */
        emit_move_insn (gen_rtx_MEM (SImode, addr), tmpvalreg);
      }
      else
      {
        if (GET_CODE (addr) != REG) {
          addr = force_reg (SImode, addr);
        }

        rtx mask_reg = gen_reg_rtx (SImode);
        emit_move_insn (mask_reg, GEN_INT (0b11));

        /* calculate the misalignment */
        rtx misalignment = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (misalignment, addr, mask_reg));

        /* align the address */
        rtx addr_aligned = gen_reg_rtx (SImode);
        emit_insn (gen_subsi3 (addr_aligned, addr, misalignment));

        /* convert the misalignment to bits (shl by 3 == multiply by 8) */
        emit_insn (gen_ashlsi3 (misalignment, misalignment, GEN_INT (3)));

        /* prepare mask */
        rtx tmpmaskreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpmaskreg, GEN_INT (0xFF));
        emit_insn (gen_ashlsi3 (tmpmaskreg, tmpmaskreg, misalignment));

        /* prepare value */
        rtx tmpvalreg2 = gen_reg_rtx (SImode);
        emit_insn (gen_ashlsi3 (tmpvalreg2, val, misalignment));
        emit_insn (gen_andsi3 (tmpvalreg2, tmpvalreg2, tmpmaskreg));

        /* 32-bit load */
        rtx tmpvalreg = gen_reg_rtx (SImode);
        emit_move_insn (tmpvalreg, gen_rtx_MEM (SImode, addr_aligned));
        emit_insn (gen_one_cmplsi2 (tmpmaskreg, tmpmaskreg)); // invert the mask for the read (unset the bits we wanna set later)
        emit_insn (gen_andsi3 (tmpvalreg, tmpvalreg, tmpmaskreg));

        /* or values together */
        emit_insn (gen_iorsi3 (tmpvalreg, tmpvalreg, tmpvalreg2));

        /* 32-bit store */
        emit_move_insn (gen_rtx_MEM (SImode, addr_aligned), tmpvalreg);
      }

      DONE;
    }
    /* emulate 8-bit memory load with aligned 32-bit accesses */
    else if (MEM_P (operands[1])) {
      rtx addr = XEXP (operands[1], 0);
      rtx val = gen_lowpart(SImode, operands[0]);

      if (TARGET_UNALIGNED_ACCESS || MEM_ALIGN (operands[1]) >= 32)
      {
        /* 32-bit load */
        emit_move_insn (val, gen_rtx_MEM (SImode, addr));

        /* mask */
        emit_insn (gen_andsi3 (val, val, GEN_INT (0xFF)));
      }
      else
      {
        if (GET_CODE (addr) != REG) {
          addr = force_reg (SImode, addr);
        }

        rtx mask_reg = gen_reg_rtx (SImode);
        emit_move_insn (mask_reg, GEN_INT (0b11));

        /* calculate the misalignment */
        rtx misalignment = gen_reg_rtx (SImode);
        emit_insn (gen_andsi3 (misalignment, addr, mask_reg));

        /* align the address */
        rtx addr_aligned = gen_reg_rtx (SImode);
        emit_insn (gen_subsi3 (addr_aligned, addr, misalignment));

        /* convert the misalignment to bits (shl by 3 == multiply by 8) */
        emit_insn (gen_ashlsi3 (misalignment, misalignment, GEN_INT (3)));

        /* 32-bit load */
        emit_move_insn (val, gen_rtx_MEM (SImode, addr_aligned));

        /* shift */
        emit_insn (gen_ashrsi3 (val, val, misalignment));

        /* mask */
        emit_insn (gen_andsi3 (val, val, GEN_INT (0xFF)));
      }

      DONE;
    }
    /* in the remaining cases we can just do a 32-bit move */
    else if (REG_P (operands[0]) || REG_P (operands[1]))
    {
      emit_insn (gen_movsi (gen_lowpart (SImode, operands[0]), gen_lowpart (SImode, operands[1])));
      DONE;
    }
  })

;; Push a register onto the stack
(define_insn "movsi_push"
  [(set (mem:SI (pre_dec:SI (reg:SI RSP_REGNUM)))
  	(match_operand:SI 0 "register_operand" "r"))]
  ""
  "strpi rsp, %0, #-4")

;; Pop a register from the stack
(define_insn "movsi_pop"
  [(set (match_operand:SI 0 "register_operand" "=r")
  	(mem:SI (post_inc:SI (reg:SI RSP_REGNUM))))]
  ""
  "ldri %0, rsp, #4")

; TODO: we should somehow support ldri, stri and strpi. I think those could be useful. Look at HAVE_PRE_MODIFY_DISP/HAVE_POST_MODIFY_DISP

;; -------------------------------------------------------------------------
;; zero and sign-extend
;; -------------------------------------------------------------------------

(define_insn "zero_extendhisi2"
  [(set (match_operand:SI		  0 "register_operand" "=r")
	(zero_extend:SI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "
  mov %0, %1
  and %0, #0xFFFF")

(define_insn "extendhisi2"
  [(set (match_operand:SI		  0 "register_operand" "=r")
	(sign_extend:SI (match_operand:HI 1 "register_operand" "r")))]
  ""
  "
  shl %0, %1, #16
  sar %0, %0, #16")

(define_insn "zero_extendqisi2"
  [(set (match_operand:SI		  0 "register_operand" "=r")
	(zero_extend:SI (match_operand:QI 1 "register_operand" "r")))]
  ""
  "and %0, %1, #0xFF"
)

(define_insn "extendqisi2"
  [(set (match_operand:SI		  0 "register_operand" "=r")
	(sign_extend:SI (match_operand:QI 1 "register_operand" "r")))]
  ""
  "
  shl %0, %1, #24
  sar %0, %0, #24")

;; -------------------------------------------------------------------------
;; Compare instructions
;; -------------------------------------------------------------------------

(define_expand "cbranchsi4"
  [(use (match_operator 0 "comparison_operator"
         [(match_operand:SI 1 "general_operand")
          (match_operand:SI 2 "general_operand")]))
   (use (match_operand 3))]
  ""
  {
    /* the first operand *always* has to be a reg */
    if (GET_CODE (operands[1]) != REG) {
      operands[1] = force_reg (SImode, operands[1]);
    }

    enum rtx_code code = GET_CODE (operands[0]);
    machine_mode mode = SELECT_CC_MODE (code, operands[1], operands[2]);
    rtx cc_reg = gen_rtx_REG (mode, RF_REGNUM);
    emit_insn (gen_rtx_SET (cc_reg, gen_rtx_COMPARE (mode, operands[1], operands[2])));
    rtx condition_rtx = gen_rtx_fmt_ee (code, VOIDmode, cc_reg, const0_rtx);
    rtx loc_ref = gen_rtx_LABEL_REF (VOIDmode, operands[3]);
    rtx ite = gen_rtx_IF_THEN_ELSE (VOIDmode, condition_rtx, loc_ref, pc_rtx);
    emit_jump_insn (gen_rtx_SET (pc_rtx, ite));
    DONE;
  })

(define_insn "*cmpsi_unsigned"
  [(set (reg:CC RF_REGNUM)
	(compare:CC
	 (match_operand:SI 0 "register_operand" "r,r")
	 (match_operand:SI 1 "general_operand"	"r,I16nSI")))]
  ""
  "cmp %0, %1")

(define_insn "*cmpsi_signed"
  [(set (reg:CCSIGNED RF_REGNUM)
	(compare:CCSIGNED
	 (match_operand:SI 0 "register_operand" "r,r")
	 (match_operand:SI 1 "general_operand"	"r,I16nHI")))]
  ""
  "cmps %0, %1")

;; -------------------------------------------------------------------------
;; Branch instructions
;; -------------------------------------------------------------------------

(define_code_iterator cond [ne eq lt ltu gt gtu ge geu le leu])
(define_code_attr CC [(ne "neq") (eq "eq") (lt "lt") (ltu "lt") (gt "gt") (gtu "gt") (geu "gteq") (ge "gteq") (le "lteq") (leu "lteq")])
(define_mode_iterator CCANY [CC CCSIGNED])

(define_insn "*b<cond:code>"
  [(set (pc)
	(if_then_else (cond (reg:CCANY RF_REGNUM)
			    (const_int 0))
		      (label_ref (match_operand 0 "" ""))
		      (pc)))]
  ""
  {
    if (flag_pic)
      return "if<CC> rjmp %l0";
    else
      return "if<CC> jmp %l0";
  })

;; -------------------------------------------------------------------------
;; Arithmetic instructions
;; -------------------------------------------------------------------------

(define_insn "addsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r")
	  (plus:SI
	   (match_operand:SI 1 "register_operand" "r,r,0,0")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI,I16nSI,I32sSI")))]
  ""
  "@
  add %0, %1, %2
  add %0, %1, %2
  add %0, %2
  addh %0, %h2")

(define_insn "subsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r")
	  (minus:SI
	   (match_operand:SI 1 "register_operand" "r,r,0,0")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI,I16nSI,I32sSI")))]
  ""
  "@
  sub %0, %1, %2
  sub %0, %1, %2
  sub %0, %2
  subh %0, %h2")

;; -------------------------------------------------------------------------
;; Shifters
;; -------------------------------------------------------------------------

(define_insn "ashlsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (ashift:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI")))]
  ""
  "@
  shl %0, %1, %2
  shl %0, %1, %2")

(define_insn "lshrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (lshiftrt:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI")))]
  ""
  "@
  shr %0, %1, %2
  shr %0, %1, %2")

(define_insn "ashrsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r")
	  (ashiftrt:SI
	   (match_operand:SI 1 "register_operand" "r,r")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI")))]
  ""
  "@
  sar %0, %1, %2
  sar %0, %1, %2")

;; -------------------------------------------------------------------------
;; Logical operators
;; -------------------------------------------------------------------------

(define_insn "andsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r")
	  (and:SI
	   (match_operand:SI 1 "register_operand" "r,r,0,0")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI,I16nSI,I32sSI")))]
  ""
  "@
  and %0, %1, %2
  and %0, %1, %2
  and %0, %2
  andh %0, %h2")

(define_insn "iorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r")
	  (ior:SI
	   (match_operand:SI 1 "register_operand" "r,r,0,0")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI,I16nSI,I32sSI")))]
  ""
  "@
  or %0, %1, %2
  or %0, %1, %2
  or %0, %2
  orh %0, %h2")

(define_insn "xorsi3"
  [(set (match_operand:SI 0 "register_operand" "=r,r,r,r")
	  (xor:SI
	   (match_operand:SI 1 "register_operand" "r,r,0,0")
	   (match_operand:SI 2 "funnyarch_arith_operand" "r,I13nSI,I16nSI,I32sSI")))]
  ""
  "@
  xor %0, %1, %2
  xor %0, %1, %2
  xor %0, %2
  xorh %0, %h2")

;; -------------------------------------------------------------------------
;; Unary arithmetic instructions
;; -------------------------------------------------------------------------

(define_insn "one_cmplsi2"
  [(set (match_operand:SI 0 "register_operand" "=r")
	(not:SI (match_operand:SI 1 "register_operand" "r")))]
  ""
  "not %0, %1")

;; -------------------------------------------------------------------------
;; Call and Jump instructions
;; -------------------------------------------------------------------------

(define_expand "call"
  [(call (match_operand:SI 0 "memory_operand" "")
		(match_operand 1 "general_operand" ""))]
  ""
{
  gcc_assert (MEM_P (operands[0]));
})

(define_insn "*call"
  [(call (mem:SI (match_operand:SI
		  0 "nonmemory_operand" "i,r"))
	 (match_operand 1 "" ""))]
  ""
  {
    switch (which_alternative)
      {
	case 0:
	  return flag_pic ? "rjal %0" : "jal %0";
	case 1:
	  return "add lr, rip, #4\n\tmov rip, %0";
	default:
	  gcc_unreachable ();
      }
  })

(define_expand "call_value"
  [(set (match_operand 0 "" "")
		(call (match_operand:SI 1 "memory_operand" "")
		 (match_operand 2 "" "")))]
  ""
{
  gcc_assert (MEM_P (operands[1]));
})

(define_insn "*call_value"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:SI (match_operand:SI
		       1 "immediate_operand" "i"))
	      (match_operand 2 "" "")))]
  ""
  {
    return flag_pic ? "rjal %1" : "jal %1";
  })

(define_insn "*call_value_indirect"
  [(set (match_operand 0 "register_operand" "=r")
	(call (mem:SI (match_operand:SI
		       1 "register_operand" "r"))
	      (match_operand 2 "" "")))]
  ""
  "add lr, rip, #4\\n\\tmov rip, %1")

(define_insn "indirect_jump"
  [(set (pc) (match_operand:SI 0 "nonimmediate_operand" "r"))]
  ""
  "mov rip, %0")

(define_insn "jump"
  [(set (pc)
	(label_ref (match_operand 0 "" "")))]
  ""
  {
    if (flag_pic)
      return "rjmp %l0";
    else
      return "jmp %l0";
  })

;; -------------------------------------------------------------------------
;; Prologue & Epilogue
;; -------------------------------------------------------------------------

(define_expand "prologue"
  [(clobber (const_int 0))]
  ""
  "
{
  funnyarch_expand_prologue ();
  DONE;
}
")

(define_expand "epilogue"
  [(return)]
  ""
  "
{
  funnyarch_expand_epilogue ();
  DONE;
}
")

(define_insn "returner"
  [(return)]
  "reload_completed"
  "mov rip, lr")
