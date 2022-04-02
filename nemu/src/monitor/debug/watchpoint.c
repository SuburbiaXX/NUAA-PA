#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(){
	if(head==NULL){
		init_wp_pool();
		head=free_;
		free_=free_->next;
		head->next=NULL;
		return head;	
	}else{
		assert(free_!=NULL);
		WP *p=free_;
		free_=free_->next;
		p->next=head;
		head=p;
		return p;
	}
}

void free_wp(WP *wp){
	WP *p=head,*q=p->next;
	if(p==wp){//delete the head
		head=q;
		p->next=free_;
		free_=p;
		printf("Watchpoint %d has been deleted!!!\n",wp->NO);
	}else{
		while(q){
			if(q==wp){
				p->next=q->next;
				q->next=free_;
				free_=q;
				printf("Watchpoint %d has been deleted!!!\n",wp->NO);
				break;
			}
			p=q;
			q=q->next;		
		}
	}
}

int set_watchpoint(char *e){
	return 0;
}

bool delete_watchpoint(int NO){
	return true;
}

void list_watchpoint(){
	
}

WP* scan_watchpoint(){
	WP *p=head;
	return p;
}
