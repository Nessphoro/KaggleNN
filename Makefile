BOOST_LIB = /cshome/malynin/boost_1_55_0/stage/lib
BOOST_INC = /cshome/malynin/boost_1_55_0
all: prog

prog:obj/main.o
	g++ -o prog obj/main.o -L $(BOOST_LIB) -pthread

obj/main.o: main.cpp
	g++ -c main.cpp -o obj/main.o -Ilib/ -I $(BOOST_INC)


