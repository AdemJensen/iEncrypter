all : ien

ien : main.o
	g++ -o ien main.o -L./lib -lcrypto -std=gnu++14
	
main.o : main.cpp functions.h
	g++ -c main.cpp -std=gnu++14

clean :
	rm main.o ien