#include "cpu/exec.h"

make_EHelper(mov) {
  operand_write(id_dest, &id_src->val);
  print_asm_template2(mov);
}

make_EHelper(push) {
  rtl_push(&id_dest->val);
  print_asm_template1(push);
}

make_EHelper(pop) {
	rtlreg_t temp;
	rtl_pop(&temp);
	operand_write(id_dest,&temp);
  print_asm_template1(pop);
}

make_EHelper(pusha) {
  TODO();

  print_asm("pusha");
}

make_EHelper(popa) {
  TODO();

  print_asm("popa");
}

make_EHelper(leave) {
  rtl_mv(&cpu.esp,&cpu.ebp);
	rtl_pop(&cpu.ebp);
  print_asm("leave");
}

make_EHelper(cltd) {
//	rtlreg_t temp;
//  if (decoding.is_operand_size_16) {
//    rtl_lr_w(&temp,R_AX);
//		rtl_sext(&temp,&temp,2);
//		rtl_sari(&temp,&temp,16);
//		rtl_sr_w(R_DX,&temp);	
//  }
//  else {
//    rtl_lr_l(&temp,R_EAX);
//		rtl_sari(&temp,&temp,31);
//		rtl_sari(&temp,&temp,1);
//		rtl_sr_l(R_EDX,&temp);
//  }
//
//  print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");

if (decoding.is_operand_size_16) {
    rtl_lr_w(&t0,R_AX);
	if((int32_t)(uint32_t)(int16_t)t0<0)
		reg_w(R_DX)=0xffff;
	else
		reg_w(R_DX)=0;
  }
  else {
    rtl_lr_l(&t0,R_EAX);
	if((int32_t)t0<0)
		 reg_l(R_EDX)=0xffffffff;
	else
		reg_l(R_EDX)=0;
  }
print_asm(decoding.is_operand_size_16 ? "cwtl" : "cltd");

}

make_EHelper(cwtl) {
//	rtlreg_t temp;
//  if (decoding.is_operand_size_16) {
//    rtl_lr_b(&temp,R_AX);
//		rtl_sext(&temp,&temp,1);
//		rtl_sr_w(R_AX,&temp);
//	}
//  else {
//		rtl_lr_w(&temp,R_AX);
//		rtl_sext(&temp,&temp,2);
//		rtl_sr_l(R_EAX,&temp); 
//  }
//  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");

 if (decoding.is_operand_size_16) {
    rtl_lr_b(&t0,R_AL);
	rtl_sext(&t0,&t0,1);
	reg_w(R_AX)=t0;
  }
  else {
    rtl_lr_w(&t0,R_AX);
    rtl_sext(&t0,&t0,2);
	reg_l(R_EAX)=t0;
  }

  print_asm(decoding.is_operand_size_16 ? "cbtw" : "cwtl");
}

make_EHelper(movsx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  rtl_sext(&t2, &id_src->val, id_src->width);
  operand_write(id_dest, &t2);
  print_asm_template2(movsx);
}

make_EHelper(movzx) {
  id_dest->width = decoding.is_operand_size_16 ? 2 : 4;
  operand_write(id_dest, &id_src->val);
  print_asm_template2(movzx);
}

make_EHelper(lea) {
  rtl_li(&t2, id_src->addr);
  operand_write(id_dest, &t2);
  print_asm_template2(lea);
}
