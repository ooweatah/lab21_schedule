
// 스케줄러와 컨텍스트 스위칭과 스레드 스택관리 스터디용 
// 유저레벨 스케줄러이지만 대부분 OS 스케줄러의 기본 구조가 비슷
// 모든 OS의 context switch와  스레드 스택 관리가  유사함(RTOS도 비슷한 작전)
// 실제 유저레벨 스레드는 코루틴에서 활용됨
// 리눅스 스케줄러 공부전 몸풀기

// 코드는 아래 사이트에서 참조함
// https://pdos.csail.mit.edu/6.828/2018/homework/xv6-uthread.html

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/user.h>
#include <stdio.h>
#include <stdlib.h>

/* 스레드 상태 */
#define FREE        0x0 // 초기 상태
#define RUNNING     0x1 // 동작
#define RUNNABLE    0x2 // 대기 상태

#define STACK_SIZE  8192
#define MAX_THREAD  4

typedef struct task_struct task_struct_t, *task_struct_p;
typedef struct mutex mutex_t, *mutex_p;

struct task_struct {
  int        sp;                /* 스택 포인터 */
  char stack[STACK_SIZE];       /* 현재 스레드의 스택 */
  int        state;             /* FREE, RUNNING, RUNNABLE */
};
static task_struct_t all_thread[MAX_THREAD];
task_struct_p current_thread;
task_struct_p next_thread;
extern void context_switch(void);

void thread_init(void)
{
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

static void __schedule(void)
{
  task_struct_p t;

  /* 다음 스레드를 찾음 */
  next_thread = 0;
  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == RUNNABLE && t != current_thread) {
      next_thread = t;
      break;
    }
  }

  // 만약 다음 스레드를 못 찾으면 현재 스레드를 돌려줌
  if (t >= all_thread + MAX_THREAD && current_thread->state == RUNNABLE) {
    next_thread = current_thread;
  }

  if (next_thread == 0) {
    printf("다음 스레드가 없습니다. 종료합니다.\n");
    exit(1);
  }

  if (current_thread != next_thread) {         /* 다르면 context switch */
    next_thread->state = RUNNING;
    context_switch();
  } else
    next_thread = 0;
}

void thread_create(void (*func)())
{
  task_struct_p t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE) break;
  }
  t->sp = (int) (t->stack + STACK_SIZE);   // 스택 포인터는 각 스레드 가장 위쪽에 설정
  t->sp -= 4;                              // 리턴 주소 저장용으로 4 byte(32bit 머신) 빼기, 트릭
  * (int *) (t->sp) = (int)func;           // 현재 함수의 주소를 스택에 저장(방금전에 뺀)
  t->sp -= 32;                             // context switch를 위해 CPU 레지스터 저장공간을 비워둠
  t->state = RUNNABLE;
}

void schedule(void)
{
  current_thread->state = RUNNABLE;
  __schedule();
}

static void  mythread(void)
{
  int i;

  for (i = 0; i < 100; i++) {
    printf("thread 0x%x\n", (int) current_thread);
    schedule();
  }
  printf("thread: exit\n");
  current_thread->state = FREE;
  __schedule();
}


int main(int argc, char *argv[])
{
  thread_init();
  thread_create(mythread);
  thread_create(mythread);
  __schedule();

  return 0;
}

