CPP_FLAGS := -std=c++0x


all: prog

cworker :obj/stdafx.o obj/CWorker.o obj/DatabaseBuilder.o obj/GraphManager.o obj/Tester.o
	g++ -o cworker obj/stdafx.o obj/CWorker.o obj/DatabaseBuilder.o obj/GraphManager.o obj/Tester.o

obj/stdafx.o: stdafx.cpp
	g++ -c $< $(CPP_FLAGS) -o $@

obj/CWorker.o: CWorker.cpp
	g++ -c $< $(CPP_FLAGS) -o $@

obj/DatabaseBuilder.o: DatabaseBuilder.cpp
	g++ -c $< $(CPP_FLAGS) -o $@

obj/GraphManager.o: GraphManager.cpp
	g++ -c $< $(CPP_FLAGS) -o $@

obj/Tester.o: Tester.cpp
	g++ -c $< $(CPP_FLAGS) -o $@
