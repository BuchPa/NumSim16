if [ -f CMakeCache.txt ]
then
	rm CMakeCache.txt
fi

if [ -f cmake_install.cmake ]
then
	rm cmake_install.cmake
fi

if [ -f Makefile ]
then
	rm Makefile
fi

if [ -f .sconsign.dblite ]
then
	rm .sconsign.dblite
fi

if [ -f numsim ]
then
	rm numsim
fi

rm -rf CMakeFiles/
rm -rf build/