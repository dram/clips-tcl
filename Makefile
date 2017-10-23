OS = $(shell uname -s)

ifeq ($(OS), Darwin)
OS = Darwin
else
OS = Linux
endif

all:
	$(MAKE) PLATFORM=$(OS) -C sources/clips libclips.a main.o
	$(MAKE) PLATFORM=$(OS) -C sources clips-tcl tcl-clips

shell:
	@ sources/clips-tcl

clean:
	$(MAKE) -C sources/clips clean
	$(MAKE) -C sources clean-clips-tcl
