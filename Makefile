gcc_options = -std=c++17 -Wall -O2 --pedantic-errors

conv_time: conv_time.o delta_t.o time.o file.o common.o
	g++92 $(gcc_options) -o $@ $^

conv_time.o : conv_time.cpp
	g++92 $(gcc_options) -c $<

delta_t.o : delta_t.cpp
	g++92 $(gcc_options) -c $<

time.o : time.cpp
	g++92 $(gcc_options) -c $<

file.o : file.cpp
	g++92 $(gcc_options) -c $<

common.o : common.cpp
	g++92 $(gcc_options) -c $<

run : conv_time
	./conv_time

clean :
	rm -f ./conv_time
	rm -f ./*.o

.PHONY : run clean

