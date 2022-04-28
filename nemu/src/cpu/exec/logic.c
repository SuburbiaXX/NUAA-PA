#include "cpu/exec.h"

make_EHelper(test) {
  rtlreg_t temp=0;
	rtl_set_CF(&temp);
	rtl_set_OF(&temp);
	rtl_and(&temp,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&temp,id_dest->width);
	print_asm_template2(test);
}

make_EHelper(and) {
  rtlreg_t temp;
	rtl_and(&temp,&id_dest->val,&id_src->val);
	operand_write(id_dest,&temp);
	rtl_update_ZFSF(&temp,id_dest->width);
	temp=0;
	rtl_set_OF(&temp);
	rtl_set_CF(&temp);
  print_asm_template2(and);
}

make_EHelper(xor) {
	rtlreg_t temp=0;
  rtl_set_CF(&temp);
	rtl_set_OF(&temp);
	rtl_xor(&temp,&id_dest->val,&id_src->val);
	operand_write(id_dest,&temp);
	rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  rtlreg_t temp1;
	rtl_or(&temp1,&id_dest->val,&id_src->val);
	operand_write(id_dest,&temp1);
	rtl_update_ZFSF(&temp1,id_dest->width);
	temp1=0;
	rtl_set_OF(&temp1);
	rtl_set_CF(&temp1);
  print_asm_template2(or);
}

make_EHelper(sar) {
	rtlreg_t temp;
  rtl_sar(&temp,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&temp,id_dest->width);
	operand_write(id_dest,&temp);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(sar);
}

make_EHelper(shl) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shl);
}

make_EHelper(shr) {
  TODO();
  // unnecessary to update CF and OF in NEMU

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  TODO();

  print_asm_template1(not);
}

make_EHelper(rol) {
  TODO();
  
  print_asm_template2(rol);
}
