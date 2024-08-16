.PHONY: clean

ifeq ($(CC), cl)

show-parent.exe: show-parent.cpp Makefile
	cl .\show-parent.cpp /Fe:show-parent.exe ole32.lib oleaut32.lib wbemuuid.lib

clean:
	del show-parent.exe show-parent.o* .*.un~

else

show-parent.exe: show-parent.cpp Makefile
	g++ -fpermissive -Wno-write-strings -O2 -isystem third_party/mingw64/include show-parent.cpp -o show-parent.exe -lole32 -loleaut32 -lwbemuuid

clean:
	rm -f show-parent.exe show-parent.o* .*.un~

endif
