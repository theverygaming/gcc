(define_constraint "I08nQI"
  "A 8-bit constant"
  (and (match_code "const_int")
       (match_test "ival == (HOST_WIDE_INT)(int8_t)ival")))

(define_constraint "I13nSI"
  "A 13-bit constant"
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, 0, 0x1fff)")))

(define_constraint "I16nHI"
  "A 16-bit constant"
  (and (match_code "const_int")
       (match_test "ival == (HOST_WIDE_INT)(int16_t)ival")))

(define_constraint "I16nSI"
  "A 16-bit constant"
  (and (match_code "const_int")
       (match_test "IN_RANGE (ival, 0, 0xffff)")))

(define_constraint "I32nSI"
  "A 32-bit constant"
  (and (match_code "const_int")
       (match_test "ival == (HOST_WIDE_INT)(int32_t)ival")))

(define_constraint "I32sSI"
  "A 32-bit constant with the bottom 16 bits set to 0"
  (and (match_code "const_int")
       (match_test "(ival & 0xffff) == 0 && ival == (HOST_WIDE_INT)(int32_t)ival")))

(define_memory_constraint "WMO13"
  "A memory address with a signed 13-bit offset"
  (and (match_code "mem")
       (ior (match_test "REG_P (XEXP (op, 0))")
            (and (match_test "GET_CODE (XEXP (op, 0)) == PLUS")
                 (match_test "REG_P (XEXP (XEXP (op, 0), 0))")
                 (match_test "CONST_INT_P (XEXP (XEXP (op, 0), 1))")
                 (match_test "IN_RANGE (INTVAL (XEXP (XEXP (op, 0), 1)), -4096, 4095)")))))
