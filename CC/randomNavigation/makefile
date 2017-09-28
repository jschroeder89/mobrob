install: randomNavigation.o librandomnav.a export clean

randomNavigation.o:
	g++ randomNavigation.cc -c

librandomnav.a:
	ar crs librandomnav.a randomNavigation.o

export:
	sudo cp librandomnav.a /usr/lib
	sudo cp randomNavigation.hpp /usr/include

clean:
	rm -f randomNavigation.o librandomnav.a
