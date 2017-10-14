all:
	make PLATFORM=Linux -C sources/clips libclips.a main.o
	make PLATFORM=Linux -C sources clips-tcl

shell:
	@ sources/clips-tcl

clean:
	make -C sources/clips clean
	make -C sources clean-clips-tcl
