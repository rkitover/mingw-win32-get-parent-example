.PHONY: clean

all: show-parent.exe show-process.exe

ifeq ($(CC), cl)

%.exe: %.cpp Makefile
	cl .\$< /Fe:$@ ole32.lib oleaut32.lib wbemuuid.lib

clean:
	del show-*.exe show-*.o* .*.un~

else

%.exe: %.cpp Makefile
	g++ -fpermissive -Wno-write-strings -O2 -isystem third_party/mingw64/include $< -o $@ -lole32 -loleaut32 -lwbemuuid

clean:
	rm -f show-*.exe show-*.o* .*.un~

endif
