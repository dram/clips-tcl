include clips/makefile

CLIPS_TCL_OBJS = $(patsubst %, clips/%, $(filter-out userfunctions.o, $(OBJS)))

OS = $(shell uname -s)

ifeq ($(OS), FreeBSD)
LDFLAGS = -lm -ltcl86
else
LDFLAGS = -lm -ltcl8.6
endif

clips-tcl: clips_tcl.o interface.o clips/main.o
	gcc -o $@ $^ $(CLIPS_TCL_OBJS) $(LDFLAGS)

tcl-clips: tcl_clips.o interface.o
	gcc -o $@ $^ $(CLIPS_TCL_OBJS) $(LDFLAGS)

clips_tcl.o: clips_tcl.c

interface.o: interface.c

tcl_clips.o: tcl_clips.c

ifeq ($(OS), FreeBSD)
clips_tcl.o interface.o tcl_clips.o: CFLAGS = -I/usr/local/include/tcl8.6
endif

ifeq ($(OS), Linux)
clips_tcl.o interface.o tcl_clips.o: CFLAGS = -I/usr/include/tcl8.6
endif

.PHONY: clean-clips-tcl

clean-clips-tcl:
	rm -f clips-tcl clips_tcl.o interface.o
