(define_predicate "funnyarch_arith_operand"
  (ior (match_operand 0 "register_operand")
       (and (match_code "const_int")
	    (match_test "(INTVAL (op) == (HOST_WIDE_INT)(int8_t)INTVAL (op)) || IN_RANGE (INTVAL (op), 0, 0x1fff) || (INTVAL (op) == (HOST_WIDE_INT)(int16_t)INTVAL (op)) || (INTVAL (op) == (HOST_WIDE_INT)(int32_t)INTVAL (op))"))))

(define_predicate "funnyarch_imm13"
  (and (match_code "const_int")
       (match_test "IN_RANGE (INTVAL (op), 0, 0x1fff)")))

(define_predicate "funnyarch_imm16"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == (HOST_WIDE_INT)(int16_t)INTVAL (op) || IN_RANGE (INTVAL (op), 0, 0xffff)")))

(define_predicate "funnyarch_imm32"
  (and (match_code "const_int")
       (match_test "INTVAL (op) == (HOST_WIDE_INT)(int32_t)INTVAL (op)")))

(define_predicate "funnyarch_imm32_high16"
  (and (match_code "const_int")
       (match_test "(INTVAL (op) & 0xffff) == 0 && INTVAL (op) == (HOST_WIDE_INT)(int32_t)INTVAL (op)")))

(define_predicate "funnyarch_move_mem_reg_offset"
  (match_code "mem")
{
  rtx addr = XEXP (op, 0);
  /* plain register is fine ofc */
  if (REG_P (addr))
  {
    return true;
  }

  /* register + offset if it fits (imm13) */
  if (GET_CODE(addr) == PLUS
      && REG_P (XEXP (addr, 0))
      && CONST_INT_P (XEXP (addr, 1))
      && IN_RANGE (INTVAL (XEXP (addr, 1)), -4096, 4095))
  {
    return true;
  }

  if (CONSTANT_ADDRESS_P (addr))
  {
    return true;
  }

  return false;
})

(define_predicate "funnyarch_move_dest_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "funnyarch_move_mem_reg_offset")))

(define_predicate "funnyarch_move_source_operand"
  (ior (match_operand 0 "register_operand")
       (match_operand 0 "funnyarch_imm13")
       (match_operand 0 "funnyarch_imm16")
       (match_operand 0 "funnyarch_imm32")
       (match_operand 0 "funnyarch_imm32_high16")
       (match_operand 0 "funnyarch_move_mem_reg_offset")
       (match_code "symbol_ref,label_ref,const")))
