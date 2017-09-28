install: requestHandler.o libreqhandler.a export clean

requestHandler.o:
	g++ requestHandler.cc -c

libreqhandler.a: requestHandler.o
	ar crs libreqhandler.a requestHandler.o

export:
	sudo cp libreqhandler.a /usr/lib
	sudo cp requestHandler.hpp /usr/include

clean:
	rm -f requestHandler.o libreqhandler.a
