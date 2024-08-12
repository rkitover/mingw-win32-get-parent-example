show-parent.exe: show-parent.cpp
	g++ -O3 -fpermissive -isystem third_party/mingw64/include show-parent.cpp -o show-parent.exe -lole32 -loleaut32 -lwbemuuid
