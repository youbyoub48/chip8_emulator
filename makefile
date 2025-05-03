main: chip8.o main.cpp
	g++ main.cpp chip8.o -o main -lSDL2main -lSDL2

chip8.o : chip8.cpp
	g++ -c chip8.cpp -lSDL2main -lSDL2

clean:
	rm *.o
	rm main