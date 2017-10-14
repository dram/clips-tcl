clips-tcl: clips_tcl.o clips/main.o
	gcc -o $@ $^ $(CLIPS_TCL_OBJS) -lm -ltcl8.6

include clips/makefile

CLIPS_TCL_OBJS = $(patsubst %, clips/%, $(filter-out userfunctions.o, $(OBJS)))

clips_tcl.o: clips_tcl.c

clips_tcl.o: CFLAGS = -I/usr/include/tcl8.6

.PHONY: clean-clips-tcl

clean-clips-tcl:
	rm -f clips-tcl clips_tcl.o
