BUILD_DIR := ../build
CXXFLAGS := -g -std=c++11 -O5 

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS 

INCDIRS := ./ ../include ../src ../solvers/minisat22 ../solvers/minisat22/core

TARGET_DIR := ../bin
TARGET := test_cnf_readwrite

CXX=mpic++

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

