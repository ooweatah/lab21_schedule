	.text

	.globl context_switch
context_switch:
	pushal                    // CPU 레지스터를 스택에 저장
	movl current_thread, %eax // 현재 스레드 주소를 eax 레지스터에 임시 저장 
	movl %esp, (%eax)         // esp 스택 포인터를 eax 주소 값(eax는 current_thread 포인터임)에 저장
	movl next_thread, %eax    // next_thread를 eax에 저장
	movl %eax, current_thread // next_thread를 current_thread로 수정
	movl $0, next_thread      // next thread는 0으로 초기화
	movl current_thread, %eax // 스택 포인터를 변경하기 위해 current_thread를 eax에 저장, task_struct의 최상의 변수가 stack pointer임.
	movl (%eax), %esp
	popal

	ret
