#include "cpu/exec.h"

make_EHelper(add) {
//	rtlreg_t temp1,temp2;
//	rtl_add(&temp1,&id_dest->val,&id_src->val);
//	operand_write(id_dest,&temp1);
//	rtl_update_ZFSF(&temp1,id_dest->width);
//	rtl_sltu(&temp2,&temp1,&id_dest->val);
//	rtl_set_CF(&temp2);
//	rtl_xor(&temp2,&id_dest->val,&id_src->val);
//	rtl_not(&temp2);
//	rtl_xor(&temp1,&id_dest->val,&temp1);
//	rtl_and(&temp1,&temp1,&temp2);
//	rtl_msb(&temp1,&temp1,id_dest->width);
//	rtl_set_OF(&temp1);
//  print_asm_template2(add);

rtlreg_t k;
    rtl_add(&t2, &id_dest->val, &id_src->val);
//	 printf("dest=%#x,src=%#x,t2=%#x\n",id_dest->val,id_src->val,t2);
    rtl_sltu(&k, &t2, &id_dest->val);
    operand_write(id_dest, &t2);
 
    rtl_update_ZFSF(&t2, id_dest->width);
 
    rtl_sltu(&t0, &t2, &id_dest->val);
    rtl_or(&t0, &k, &t0);
    rtl_set_CF(&t0);
 
    rtl_xor(&t0, &id_dest->val, &id_src->val);
    rtl_not(&t0);
    rtl_xor(&t1, &id_dest->val, &t2);
    rtl_and(&t0, &t0, &t1);
    rtl_msb(&t0, &t0, id_dest->width);
    rtl_set_OF(&t0);
    print_asm_template2(add);
}

make_EHelper(sub) {
//  rtlreg_t temp1,temp2,temp3;
//	rtl_sub(&temp1,&id_dest->val,&id_src->val);
//	operand_write(id_dest,&temp1);
//	//update eflags status
//	rtl_update_ZFSF(&temp1,id_dest->width);
//	rtl_sltu(&temp2,&id_dest->val,&temp1);
//	rtl_set_CF(&temp2);
//	rtl_xor(&temp2,&id_dest->val,&id_src->val);
//	rtl_xor(&temp3,&id_dest->val,&temp1);
//	rtl_and(&temp2,&temp2,&temp3);
//	rtl_msb(&temp2,&temp2,id_dest->width);
//	rtl_set_OF(&temp2);
//  print_asm_template2(sub);

rtlreg_t k;
    rtl_sub(&t2, &id_dest->val, &id_src->val);
//	printf("dest=%#x,src=%#x,t2=%#x\n",id_dest->val,id_src->val,t2);
	rtl_sltu(&k, &id_dest->val, &id_src->val);
//	printf("dest=%#x,src=%#x,k=%#x\n",id_dest->val,id_src->val,k);
    operand_write(id_dest, &t2);
 
    rtl_update_ZFSF(&t2, id_dest->width);
//	printf("cf=%#x\n",cpu.eflags.CF);
    rtl_set_CF(&k);
//    printf("set涔嬪悗 cf'=%#x\n",cpu.eflags.CF);
    rtl_xor(&t0, &id_dest->val, &id_src->val);
    rtl_xor(&t1, &id_dest->val, &t2);
    rtl_and(&t0, &t0, &t1);
    rtl_msb(&t0, &t0, id_dest->width);
    rtl_set_OF(&t0);
//    printf("set涔嬪悗 cf=%#x\n",cpu.eflags.CF);
  print_asm_template2(sub);
}

make_EHelper(cmp) {
//  rtlreg_t temp1,temp2;
//	rtl_sub(&temp1,&id_dest->val,&id_src->val);
//	rtl_update_ZFSF(&temp1,id_dest->width);
//	rtl_sltu(&temp2,&id_dest->val,&temp1);
//	rtl_set_CF(&temp2);
//	rtl_xor(&temp2,&id_dest->val,&id_src->val);
//	rtl_xor(&temp1,&id_dest->val,&temp1);
//	rtl_and(&temp1,&temp1,&temp2);
//	rtl_msb(&temp1,&temp1,id_dest->width);
//	rtl_set_OF(&temp1);
//  print_asm_template2(cmp);

rtlreg_t k,o;
	//printf("eax=%#x ",cpu.eax);
    rtl_sub(&t2, &id_dest->val, &id_src->val);
    rtl_sltu(&k, &id_dest->val, &t2);
	//printf("eax'=%#x\n",cpu.eax); 
    rtl_update_ZFSF(&t2, id_dest->width);
 
    rtl_sltu(&o, &id_dest->val, &t2);
    rtl_or(&o, &k, &o);
    rtl_set_CF(&o);
    rtl_xor(&o, &id_dest->val, &id_src->val);
	//printf("eax'=%#x\n",cpu.eax);
    rtl_xor(&t1, &id_dest->val, &t2);
    rtl_and(&o, &o, &t1);
    rtl_msb(&o, &o, id_dest->width);
    rtl_set_OF(&o);
	//printf("eax'=%#x\n",cpu.eax);
  print_asm_template2(cmp);
}

make_EHelper(inc) {
//	rtlreg_t temp1=id_dest->val,temp2;
//	temp1++;
//	rtl_xor(&temp2,&temp1,&id_dest->val);
//	rtl_msb(&temp2,&temp2,id_dest->width);
//	rtl_set_OF(&temp2);
//	operand_write(id_dest,&temp1);
//	rtl_update_ZFSF(&temp1,id_dest->width);
//  print_asm_template1(inc);

rtl_addi(&t0,&id_dest->val,1);
  operand_write(id_dest,&t0);
  rtl_xor(&t1,&id_dest->val,&t0);
  rtl_msb(&t1,&t1, id_dest->width);
  rtl_set_OF(&t1);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template1(inc);
}

make_EHelper(dec) {
//  rtlreg_t temp1,temp2,temp3=1;
//	rtl_subi(&temp1,&id_dest->val,1);
//	rtl_update_ZFSF(&temp1,id_dest->width);
//	rtl_xor(&temp2,&id_dest->val,&temp3);
//	rtl_xor(&temp3,&id_dest->val,&temp1);
//	rtl_and(&temp3,&temp2,&temp3);
//	rtl_msb(&temp3,&temp3,id_dest->width);
//	rtl_set_OF(&temp3);
//	operand_write(id_dest,&temp1);
//  print_asm_template1(dec);

rtl_subi(&t0,&id_dest->val,1);
  operand_write(id_dest,&t0);
  rtlreg_t i;
  rtl_xor(&i,&id_dest->val,&t0);
  rtl_msb(&i,&i,id_dest->width);
  rtl_set_OF(&i);
  rtl_update_ZFSF(&t0, id_dest->width);
  print_asm_template1(dec);
}

make_EHelper(neg) {
//  rtlreg_t temp1,temp2;
//	temp1=(id_dest->val==0)?0:1;
//	rtl_set_CF(&temp1);
//	temp2=-id_dest->val;
//	operand_write(id_dest,&temp2);
//	rtl_update_ZFSF(&temp2,id_dest->width);
//	
//	rtl_xor(&temp1,&temp2,&id_dest->val);
//	rtl_not(&temp1);
//	rtl_msb(&temp1,&temp1,id_dest->width);
//	rtl_set_OF(&temp1);
//  print_asm_template1(neg);

rtlreg_t k,i,j;
  k=(id_dest->val==0)?0:1;
  rtl_set_CF(&k);
  i=-id_dest->val;
  operand_write(id_dest,&i);
  rtl_update_ZFSF(&i, id_dest->width);
  rtl_xor(&j, &id_dest->val, &id_src->val);
  rtl_xor(&i, &id_dest->val, &t2);
  rtl_and(&j,&i,&j);
  rtl_msb(&j,&j,id_dest->width);
  rtl_set_OF(&j);
  print_asm_template1(neg);
}

make_EHelper(adc) {
  rtl_add(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &t2, &id_dest->val);
  rtl_get_CF(&t1);
  rtl_add(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &t2, &id_dest->val);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_not(&t0);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(adc);
}

make_EHelper(sbb) {
  rtl_sub(&t2, &id_dest->val, &id_src->val);
  rtl_sltu(&t3, &id_dest->val, &t2);
  rtl_get_CF(&t1);
  rtl_sub(&t2, &t2, &t1);
  operand_write(id_dest, &t2);

  rtl_update_ZFSF(&t2, id_dest->width);

  rtl_sltu(&t0, &id_dest->val, &t2);
  rtl_or(&t0, &t3, &t0);
  rtl_set_CF(&t0);

  rtl_xor(&t0, &id_dest->val, &id_src->val);
  rtl_xor(&t1, &id_dest->val, &t2);
  rtl_and(&t0, &t0, &t1);
  rtl_msb(&t0, &t0, id_dest->width);
  rtl_set_OF(&t0);

  print_asm_template2(sbb);
}

make_EHelper(mul) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_mul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(mul);
}

// imul with one operand
make_EHelper(imul1) {
  rtl_lr(&t0, R_EAX, id_dest->width);
  rtl_imul(&t0, &t1, &id_dest->val, &t0);

  switch (id_dest->width) {
    case 1:
      rtl_sr_w(R_AX, &t1);
      break;
    case 2:
      rtl_sr_w(R_AX, &t1);
      rtl_shri(&t1, &t1, 16);
      rtl_sr_w(R_DX, &t1);
      break;
    case 4:
      rtl_sr_l(R_EDX, &t0);
      rtl_sr_l(R_EAX, &t1);
      break;
    default: assert(0);
  }

  print_asm_template1(imul);
}

// imul with two operands
make_EHelper(imul2) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_dest->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template2(imul);
}

// imul with three operands
make_EHelper(imul3) {
  rtl_sext(&id_src->val, &id_src->val, id_src->width);
  rtl_sext(&id_src2->val, &id_src2->val, id_src->width);
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  rtl_imul(&t0, &t1, &id_src2->val, &id_src->val);
  operand_write(id_dest, &t1);

  print_asm_template3(imul);
}

make_EHelper(div) {
  switch (id_dest->width) {
    case 1:
      rtl_li(&t1, 0);
      rtl_lr_w(&t0, R_AX);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_li(&t1, 0);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_div(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(div);
}

make_EHelper(idiv) {
  rtl_sext(&id_dest->val, &id_dest->val, id_dest->width);

  switch (id_dest->width) {
    case 1:
      rtl_lr_w(&t0, R_AX);
      rtl_sext(&t0, &t0, 2);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 2:
      rtl_lr_w(&t0, R_AX);
      rtl_lr_w(&t1, R_DX);
      rtl_shli(&t1, &t1, 16);
      rtl_or(&t0, &t0, &t1);
      rtl_msb(&t1, &t0, 4);
      rtl_sub(&t1, &tzero, &t1);
      break;
    case 4:
      rtl_lr_l(&t0, R_EAX);
      rtl_lr_l(&t1, R_EDX);
      break;
    default: assert(0);
  }

  rtl_idiv(&t2, &t3, &t1, &t0, &id_dest->val);

  rtl_sr(R_EAX, id_dest->width, &t2);
  if (id_dest->width == 1) {
    rtl_sr_b(R_AH, &t3);
  }
  else {
    rtl_sr(R_EDX, id_dest->width, &t3);
  }

  print_asm_template1(idiv);
}
