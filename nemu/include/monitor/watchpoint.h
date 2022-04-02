#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;
	char expr[64];
	uint32_t new_val;
	uint32_t old_val;
  /* TODO: Add more members if necessary */
	

} WP;

int set_watchpoint(char *e);
bool delete_watchpoint(int NO);
void list_watchpoint();
WP* scan_watchpoint();

#endif
