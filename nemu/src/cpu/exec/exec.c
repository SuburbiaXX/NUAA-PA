#include "cpu/exec.h"
#include "all-instr.h"

typedef struct {
  DHelper decode;
  EHelper execute;
  int width;
} opcode_entry;

#define IDEXW(id, ex, w)   {concat(decode_, id), concat(exec_, ex), w}
#define IDEX(id, ex)       IDEXW(id, ex, 0)
#define EXW(ex, w)         {NULL, concat(exec_, ex), w}
#define EX(ex)             EXW(ex, 0)
#define EMPTY              EX(inv)

static inline void set_width(int width) {
  if (width == 0) {
    width = decoding.is_operand_size_16 ? 2 : 4;
  }
  decoding.src.width = decoding.dest.width = decoding.src2.width = width;
}

/* Instruction Decode and EXecute */
static inline void idex(vaddr_t *eip, opcode_entry *e) {
  /* eip is pointing to the byte next to opcode */
  if (e->decode)
    e->decode(eip);
  e->execute(eip);
}

static make_EHelper(2byte_esc);

#define make_group(name, item0, item1, item2, item3, item4, item5, item6, item7) \
  static opcode_entry concat(opcode_table_, name) [8] = { \
    /* 0x00 */	item0, item1, item2, item3, \
    /* 0x04 */	item4, item5, item6, item7  \
  }; \
static make_EHelper(name) { \
  idex(eip, &concat(opcode_table_, name)[decoding.ext_opcode]); \
}

/* 0x80, 0x81, 0x83 */
make_group(gp1,
    EX(add), EX(or), EX(adc), EX(sbb),
    EX(and), EX(sub), EX(xor), EX(cmp))

  /* 0xc0, 0xc1, 0xd0, 0xd1, 0xd2, 0xd3 */
make_group(gp2,
    EX(rol), EMPTY, EMPTY, EMPTY,
    EX(shl), EX(shr), EMPTY, EX(sar))

  /* 0xf6, 0xf7 */
make_group(gp3,
    IDEXW(test_I,test,0), EMPTY, EX(not), EX(neg),
    EX(mul), EX(imul1), EX(div), EX(idiv))

  /* 0xfe */
make_group(gp4,
    EXW(inc,1), EXW(dec,1), EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY)

  /* 0xff */
make_group(gp5,
    EX(inc), EX(dec), EX(call_rm), EX(call),
   	EX(jmp_rm), EX(jmp), EX(push), EMPTY)

  /* 0x0f 0x01*/
make_group(gp7,
    EMPTY, EMPTY, EMPTY, EMPTY,
    EMPTY, EMPTY, EMPTY, EMPTY)

/* TODO: Add more instructions!!! */

opcode_entry opcode_table [512] = {
  /* 0x00 */	IDEXW(G2E,add,1), IDEXW(G2E,add,0), IDEXW(E2G,add,1), IDEXW(E2G,add,0),
  /* 0x04 */	IDEXW(I2a,add,1), IDEXW(I2a,add,0), EMPTY, EMPTY,
  /* 0x08 */	IDEXW(G2E,or,1), IDEXW(G2E,or,0), IDEXW(E2G,or,1), IDEXW(E2G,or,0),
  /* 0x0c */	IDEXW(I2a,or,1), IDEXW(I2a,or,0), EMPTY, EX(2byte_esc),
  /* 0x10 */	IDEXW(G2E,adc,1), IDEXW(G2E,adc,0), IDEXW(E2G,adc,1), IDEXW(E2G,adc,0),
  /* 0x14 */	IDEXW(I2a,adc,1), IDEXW(I2a,adc,0), EMPTY, EMPTY,
  /* 0x18 */	IDEXW(G2E,sbb,1), IDEXW(G2E,sbb,0), IDEXW(E2G,sbb,1), IDEXW(E2G,sbb,0),
  /* 0x1c */	IDEXW(I2a,sbb,1), IDEXW(I2a,sbb,0), EMPTY, EMPTY,
  /* 0x20 */	IDEXW(G2E,and,1), IDEXW(G2E,and,0), IDEXW(E2G,and,1), IDEXW(E2G,and,0),
  /* 0x24 */	IDEXW(I2a,and,1), IDEXW(I2a,and,0), EMPTY, EMPTY,
  /* 0x28 */	IDEXW(G2E,sub,1), IDEXW(G2E,sub,0), IDEXW(E2G,sub,1), IDEXW(E2G,sub,0),
  /* 0x2c */	IDEXW(I2a,sub,1), IDEXW(I2a,sub,0), EMPTY, EMPTY,
  /* 0x30 */	IDEXW(G2E,xor,0), IDEXW(G2E,xor,0), IDEXW(E2G,xor,0), IDEXW(E2G,xor,0),
  /* 0x34 */	IDEXW(I2a,xor,0), IDEXW(I2a,xor,0), EMPTY, EMPTY,
  /* 0x38 */	IDEXW(G2E,cmp,1), IDEXW(G2E,cmp,0), IDEXW(E2G,cmp,1), IDEXW(E2G,cmp,0),
  /* 0x3c */	IDEXW(I2a,cmp,1), IDEXW(I2a,cmp,0), EMPTY, EMPTY,
  /* 0x40 */	IDEXW(r,inc,0), IDEXW(r,inc,0), IDEXW(r,inc,0), IDEXW(r,inc,0),
  /* 0x44 */	IDEXW(r,inc,0), IDEXW(r,inc,0), IDEXW(r,inc,0), IDEXW(r,inc,0),
  /* 0x48 */	IDEXW(r,dec,0), IDEXW(r,dec,0), IDEXW(r,dec,0), IDEXW(r,dec,0),
  /* 0x4c */	IDEXW(r,dec,0), IDEXW(r,dec,0), IDEXW(r,dec,0), IDEXW(r,dec,0),
  /* 0x50 */	IDEXW(r,push,4), IDEXW(r,push,4), IDEXW(r,push,4), IDEXW(r,push,4),
  /* 0x54 */	IDEXW(r,push,4), IDEXW(r,push,4), IDEXW(r,push,4), IDEXW(r,push,4),
  /* 0x58 */	IDEXW(r,pop,4), IDEXW(r,pop,4), IDEXW(r,pop,4), IDEXW(r,pop,4),
  /* 0x5c */	IDEXW(r,pop,4), IDEXW(r,pop,4), IDEXW(r,pop,4), IDEXW(r,pop,4),
  /* 0x60 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EX(operand_size), EMPTY,
  /* 0x68 */	IDEXW(push_SI,push,0), EMPTY, IDEXW(push_SI,push,1), EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x70 */	IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1),
  /* 0x74 */	IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1),
  /* 0x78 */	IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1),
  /* 0x7c */	IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1), IDEXW(J,jcc,1),
  /* 0x80 */	IDEXW(I2E, gp1, 1), IDEX(I2E, gp1), EMPTY, IDEX(SI2E, gp1),
  /* 0x84 */	IDEXW(G2E,test,1), IDEXW(G2E,test,0), EMPTY, EMPTY,
  /* 0x88 */	IDEXW(mov_G2E, mov, 1), IDEX(mov_G2E, mov), IDEXW(mov_E2G, mov, 1), IDEX(mov_E2G, mov),
  /* 0x8c */	EMPTY, IDEXW(lea_M2G,lea,0), EMPTY, IDEXW(E,pop,0),
  /* 0x90 */	EX(nop), EMPTY, EMPTY, EMPTY,
  /* 0x94 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x98 */	EX(cwtl), EX(cltd), EMPTY, EMPTY,
  /* 0x9c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa0 */	IDEXW(O2a, mov, 1), IDEX(O2a, mov), IDEXW(a2O, mov, 1), IDEX(a2O, mov),
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	IDEXW(I2a,test,1), IDEXW(I2a,test,0), EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb0 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb4 */	IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1), IDEXW(mov_I2r, mov, 1),
  /* 0xb8 */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xbc */	IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov), IDEX(mov_I2r, mov),
  /* 0xc0 */	IDEXW(gp2_Ib2E, gp2, 1), IDEX(gp2_Ib2E, gp2), IDEXW(I,ret,2), EX(ret),
  /* 0xc4 */	EMPTY, EMPTY, IDEXW(mov_I2E, mov, 1), IDEX(mov_I2E, mov),
  /* 0xc8 */	EMPTY, EX(leave), IDEXW(I,ret,2), EX(ret),
  /* 0xcc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd0 */	IDEXW(gp2_1_E, gp2, 1), IDEX(gp2_1_E, gp2), IDEXW(gp2_cl2E, gp2, 1), IDEX(gp2_cl2E, gp2),
  /* 0xd4 */	EMPTY, EMPTY, EX(nemu_trap), EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	IDEXW(in_I2a,in,1), IDEXW(in_I2a,in,0), IDEXW(out_a2I,out,1), IDEXW(out_a2I,out,0),
  /* 0xe8 */	IDEXW(I,call,0), IDEXW(J,jmp,0), EMPTY, IDEXW(J,jmp,1),
  /* 0xec */	IDEXW(in_dx2a,in,1), IDEXW(in_dx2a,in,0), IDEXW(out_a2dx,out,1), IDEXW(out_a2dx,out,0),
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, IDEXW(E, gp3, 1), IDEX(E, gp3),
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, IDEXW(E, gp4, 1), IDEX(E, gp5),

  /*2 byte_opcode_table */

  /* 0x00 */	EMPTY, IDEX(gp7_E, gp7), EMPTY, EMPTY,
  /* 0x04 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x08 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x0c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x10 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x14 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x18 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x1c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x20 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x24 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x28 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x2c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x30 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x34 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x38 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x3c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x40 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x44 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x48 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x4c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x50 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x54 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x58 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x5c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x60 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x64 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x68 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x6c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x70 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x74 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x78 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x7c */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0x80 */	IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc),
  /* 0x84 */	IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc),
  /* 0x88 */	IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc),
  /* 0x8c */	IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc), IDEX(J,jcc),
  /* 0x90 */	IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1),
  /* 0x94 */	IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1),
  /* 0x98 */	IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1),
  /* 0x9c */	IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1), IDEXW(E,setcc,1),
  /* 0xa0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xa8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xac */	EMPTY, EMPTY, EMPTY, IDEXW(E2G,imul2,0),
  /* 0xb0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xb4 */	EMPTY, EMPTY, IDEXW(mov_E2G,movzx,1), IDEXW(mov_E2G,movzx,2),
  /* 0xb8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xbc */	EMPTY, EMPTY, IDEXW(mov_E2G,movsx,1), IDEXW(mov_E2G,movsx,2),
  /* 0xc0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xc8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xcc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xd8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xdc */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xe8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xec */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf0 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf4 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xf8 */	EMPTY, EMPTY, EMPTY, EMPTY,
  /* 0xfc */	EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(2byte_esc) {
  uint32_t opcode = instr_fetch(eip, 1) | 0x100;
  decoding.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(eip, &opcode_table[opcode]);
}

make_EHelper(real) {
  uint32_t opcode = instr_fetch(eip, 1);
  decoding.opcode = opcode;
  set_width(opcode_table[opcode].width);
  idex(eip, &opcode_table[opcode]);
}

static inline void update_eip(void) {
  cpu.eip = (decoding.is_jmp ? (decoding.is_jmp = 0, decoding.jmp_eip) : decoding.seq_eip);
}

void exec_wrapper(bool print_flag) {
#ifdef DEBUG
  decoding.p = decoding.asm_buf;
  decoding.p += sprintf(decoding.p, "%8x:   ", cpu.eip);
#endif

  decoding.seq_eip = cpu.eip;
  exec_real(&decoding.seq_eip);

#ifdef DEBUG
  int instr_len = decoding.seq_eip - cpu.eip;
  sprintf(decoding.p, "%*.s", 50 - (12 + 3 * instr_len), "");
  char strbuf[512];
  strcpy(strbuf, decoding.asm_buf);
  // strcat(decoding.asm_buf, decoding.assembly);
  strcat(strbuf, decoding.assembly);
  strcpy(decoding.asm_buf, strbuf);
  Log_write("%s\n", decoding.asm_buf);
  if (print_flag) {
    puts(decoding.asm_buf);
  }
#endif

#ifdef DIFF_TEST
  uint32_t eip = cpu.eip;
#endif

  update_eip();

#ifdef DIFF_TEST
  void difftest_step(uint32_t);
  difftest_step(eip);
#endif
}
