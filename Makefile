BOOST_LIB = /cshome/malynin/boost_1_55_0/stage/lib
ZLIB=/cshome/malynin/zlib/lib
BOOST_INC = /cshome/malynin/boost_1_55_0
all: prog

prog:obj/main.o
	g++ -o prog obj/main.o $(BOOST_LIB)/libboost_iostreams.a $(ZLIB)/libz.a -pthread

obj/main.o: main.cpp
	g++ -c main.cpp -std=c++0x -o obj/main.o -Ilib/ -I $(BOOST_INC)


