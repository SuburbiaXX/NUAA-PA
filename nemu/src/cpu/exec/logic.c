#include "cpu/exec.h"

make_EHelper(test) {
//  rtlreg_t temp=0;
//	rtl_set_CF(&temp);
//	rtl_set_OF(&temp);
//	rtl_and(&temp,&id_dest->val,&id_src->val);
//	rtl_update_ZFSF(&temp,id_dest->width);
//	print_asm_template2(test);

  rtl_and(&t0,&id_dest->val,&id_src->val);
  rtlreg_t k=0;
  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_OF(&k);
  rtl_set_CF(&k);
  print_asm_template2(test);
}

make_EHelper(and) {
//  rtlreg_t temp;
//	rtl_and(&temp,&id_dest->val,&id_src->val);
//	operand_write(id_dest,&temp);
//	rtl_update_ZFSF(&temp,id_dest->width);
//	temp=0;
//	rtl_set_OF(&temp);
//	rtl_set_CF(&temp);
//  print_asm_template2(and);

	
    rtl_and(&t1, &id_dest->val, &id_src->val);
    operand_write(id_dest, &t1);
    rtl_update_ZFSF(&t1, id_dest->width); 
    rtlreg_t k=0;
    rtl_set_OF(&k); 
    rtl_set_CF(&k); 
    print_asm_template2(and);
}

make_EHelper(xor) {
//	rtlreg_t temp=0;
//  rtl_set_CF(&temp);
//	rtl_set_OF(&temp);
//	rtl_xor(&temp,&id_dest->val,&id_src->val);
//	operand_write(id_dest,&temp);
//	rtl_update_ZFSF(&id_dest->val,id_dest->width);
//  print_asm_template2(xor);

  rtl_xor(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtlreg_t k=0;
  rtl_set_OF(&k);
  rtl_set_CF(&k);
  rtl_update_ZFSF(&id_dest->val,id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
//  rtlreg_t temp1;
//	rtl_or(&temp1,&id_dest->val,&id_src->val);
//	operand_write(id_dest,&temp1);
//	rtl_update_ZFSF(&temp1,id_dest->width);
//	temp1=0;
//	rtl_set_OF(&temp1);
//	rtl_set_CF(&temp1);
//  print_asm_template2(or);

  rtl_or(&t0,&id_dest->val,&id_src->val);
  operand_write(id_dest,&t0);
  rtlreg_t k=0;
  rtl_update_ZFSF(&t0,id_dest->width);
  rtl_set_OF(&k);
  rtl_set_CF(&k);
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
  rtlreg_t temp;
	rtl_shl(&temp,&id_dest->val,&id_src->val);
	rtl_update_ZFSF(&temp,id_dest->width);
	operand_write(id_dest,&temp);
  // unnecessary to update CF and OF in NEMU
  print_asm_template2(shl);
}

make_EHelper(shr) {
//  // unnecessary to update CF and OF in NEMU
	rtlreg_t temp;
	rtl_shr(&temp,&id_dest->val,&id_src->val);
	operand_write(id_dest,&temp);
	rtl_update_ZFSF(&temp,id_dest->width);
  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint8_t subcode = decoding.opcode & 0xf;
  rtl_setcc(&t2, subcode);
  operand_write(id_dest, &t2);

  print_asm("set%s %s", get_cc_name(subcode), id_dest->str);
}

make_EHelper(not) {
  rtlreg_t temp=id_dest->val;
	rtl_not(&temp);
	operand_write(id_dest,&temp);
  print_asm_template1(not);
}

make_EHelper(rol) {
// 	rtlreg_t temp1,temp2,temp3;
//	for(temp1=0;temp1<id_src->val;temp1++){
//		rtl_shri(&temp2,&id_dest->val,id_dest->width*8-1);//Highest position
//		rtl_shli(&temp3,&id_dest->val,1);
//		id_dest->val=temp1+temp2;
//	}
//	rtl_set_CF(&temp1);
//	operand_write(id_dest,&id_dest->val);
//  print_asm_template2(rol);

  int i=0;
  rtlreg_t q;
  while(i<id_src->val){
	  rtl_shri(&t1,&id_dest->val,id_dest->width*8-1);
      rtl_shli(&q,&id_dest->val,1);
      id_dest->val=t1+q;
	  i++;
  }
  rtl_set_CF(&t1);
  operand_write(id_dest,&id_dest->val);
  print_asm_template2(rol);
}
