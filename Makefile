CPP = mpic++
INC1 = ./
INC2 = ./wrappers/
INC3 = ./solvers/minisat22/
INC4 = ./search
INCDIRS = -I${INC1} -I${INC2} -I${INC3} -I${INC4} 
CPPFLAGS = -O5 -g  -std=c++11 ${INCDIRS}

utils.o: utils.cpp
	${CPP} ${CPPFLAGS} utils.cpp -c

minisat22.o: wrappers/minisat22.cpp
	${CPP} ${CPPFLAGS} wrappers/minisat22.cpp -c -o wrappers/minisat22.o

taboo.o: search/taboo.cpp
	${CPP} ${CPPFLAGS} search/taboo.cpp -c -o search/taboo.o

test_cnf_readwrite.o: tests/test_cnf_readwrite.cpp
	${CPP} ${CPPFLAGS} tests/test_cnf_readwrite.cpp -c -o tests/test_cnf_readwrite.o

test_cnf_readwrite: test_cnf_readwrite.o utils.o
	${CPP} ${CPPFLAGS} tests/test_cnf_readwrite.o utils.o -o bin/test_cnf_readwrite

test_solve_minisat22.o: tests/test_solve_minisat22.cpp
	${CPP} ${CPPFLAGS} tests/test_solve_minisat22.cpp -c -o tests/test_solve_minisat22.o

test_solve_minisat22: test_solve_minisat22.o utils.o minisat22.o
	${CPP} ${CPPFLAGS} tests/test_solve_minisat22.o utils.o wrappers/minisat22.o solvers/minisat22/core/Solver.o -o bin/test_solve_minisat22

test_taboo.o: tests/test_taboo.cpp
	${CPP} ${CPPFLAGS} tests/test_taboo.cpp -c -o tests/test_taboo.o

test_taboo: test_taboo.o utils.o minisat22.o taboo.o
	${CPP} ${CPPFLAGS} tests/test_taboo.o utils.o wrappers/minisat22.o solvers/minisat22/core/Solver.o search/taboo.o -o bin/test_taboo

peer.o: peer.cpp
	${CPP} ${CPPFLAGS} peer.cpp -c -o peer.o

main.o: main.cpp
	${CPP} ${CPPFLAGS} main.cpp -c -o main.o

fhtagnsat: main.o utils.o peer.o taboo.o
	${CPP} ${CPPFLAGS} main.o utils.o peer.o wrappers/minisat22.o solvers/minisat22/core/Solver.o search/taboo.o -o bin/fhtagnsat


clean:
	rm  *.o
	rm  ${INC1}*.o
	rm  ${INC2}*.o
	rm  ${INC4}*.o
	rm  tests/*.o
	rm bin/*

