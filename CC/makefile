build: requestHandler.o libreqhandler.a randomNavigation.o librandomnav.a export clean app.o

requestHandler.o:
	g++ lib/requestHandler/requestHandler.cc -c -o lib/requestHandler/requestHandler.o -std=c++11

libreqhandler.a: requestHandler.o
	ar crs lib/requestHandler/libreqhandler.a lib/requestHandler/requestHandler.o
	
randomNavigation.o:
	g++ lib/randomNavigation/randomNavigation.cc -o lib/randomNavigation/randomNavigation.o -c -std=c++11	

librandomnav.a: randomNavigation.o
	ar crs lib/randomNavigation/librandomnav.a lib/randomNavigation/randomNavigation.o

export:
	sudo cp lib/requestHandler/requestHandler.hpp /usr/include 
	sudo cp lib/randomNavigation/randomNavigation.hpp /usr/include
	sudo cp lib/requestHandler/libreqhandler.a /usr/lib
	sudo cp lib/randomNavigation/librandomnav.a /usr/lib

clean:
	rm -f lib/reqeustHandler/requestHandler.o lib/requestHandler/libreqhandler.a
	rm -f lib/randomNavigation/randomNavigation.o lib/randomNavigation/librandomnav.a

app.o:
	g++ src/app.cc -o src/app.o -lreqhandler -lrandomnav -lzmq -std=c++11

run:
	./src/app.o