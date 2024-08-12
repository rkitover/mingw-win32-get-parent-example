show-parent.exe: show-parent.cpp Makefile
	g++ -fpermissive -O2 -isystem third_party/mingw64/include show-parent.cpp -o show-parent.exe -lole32 -loleaut32 -lwbemuuid
