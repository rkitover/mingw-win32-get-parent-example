show-parent.exe: show-parent.cpp Makefile
	g++ -fpermissive -Wno-write-strings -O2 -isystem third_party/mingw64/include show-parent.cpp -o show-parent.exe -lole32 -loleaut32 -lwbemuuid

.PHONY: clean

clean:
	rm -f show-parent.exe .*.un~
