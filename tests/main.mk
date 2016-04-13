BUILD_DIR := ../build
CXXFLAGS := -g -std=c++11 -O5 -pg

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS 

INCDIRS :=  ./ ../include ../src ../solvers/minisat22 ../solvers/minisat22/core ../solvers/lingeling ../

TARGET_DIR := ../bin

CXX=mpic++

TARGET := test_cnf_readwrite
SOURCES := \
	test_cnf_readwrite.cpp \
	../src/wrappers/minisat22.cpp \
	../src/utils.cpp \
	../solvers/minisat22/core/Solver.cc


TARGET := test_solve_minisat22

SOURCES := \
	test_solve_minisat22.cpp \
	../src/wrappers/minisat22.cpp \
	../src/utils.cpp \
	../solvers/minisat22/core/Solver.cc


#CC=mpiCC
CCFLAGS := -g -std=c++11 -O5 -pg

TARGET := test_solve_lingeling

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS NDBLSCR NLGLOG NDEBUG NCHKSOL NLGLPICOSAT NLGLDRUPLIG NLGLYALSAT

SOURCES := \
	test_solve_lingeling.cpp \
	../src/wrappers/minisat22.cpp \
	../src/utils.cpp \
	../solvers/minisat22/core/Solver.cc \
	../solvers/lingeling/lglib.c \
	../solvers/lingeling/lglopts.c \
	../src/wrappers/lingeling.cpp

