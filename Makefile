all:
	gcc -m32 -static -g -o schedule schedule.c context_switch.S
clean:
	rm -rf schedule

