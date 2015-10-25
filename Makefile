CPP = g++
INC1 = ./
INC2 = ./wrappers/
INC3 = ./solvers/minisat22/
INCDIRS = -I${INC1} -I${INC2} -I${INC3}
CPPFLAGS = -O0 -g -std=c++11 ${INCDIRS}

utils.o: utils.cpp
	${CPP} ${CPPFLAGS} utils.cpp -c

minisat22.o: wrappers/minisat22.cpp
	${CPP} ${CPPFLAGS} wrappers/minisat22.cpp -c -o wrappers/minisat22.o

test_cnf_readwrite.o: tests/test_cnf_readwrite.cpp
	${CPP} ${CPPFLAGS} tests/test_cnf_readwrite.cpp -c -o tests/test_cnf_readwrite.o

test_solve_minisat22.o: tests/test_solve_minisat22.cpp
	${CPP} ${CPPFLAGS} tests/test_solve_minisat22.cpp -c -o tests/test_solve_minisat22.o

test_cnf_readwrite: test_cnf_readwrite.o utils.o
	${CPP} ${CPPFLAGS} tests/test_cnf_readwrite.o utils.o -o bin/test_cnf_readwrite

test_solve_minisat22: test_solve_minisat22.o utils.o minisat22.o
	${CPP} ${CPPFLAGS} tests/test_solve_minisat22.o utils.o wrappers/minisat22.o solvers/minisat22/core/Solver.o -o bin/test_solve_minisat22

clean:
	rm  *.o
	rm  ${INC1}*.o
	rm  ${INC2}*.o
	rm  tests/*.o
	rm bin/test_cnf_readwrite
	rm bin/test_solve_minisat22

