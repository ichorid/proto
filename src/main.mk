TARGET := fhsat

CXX=mpic++

SOURCES := \
	search/taboo.cpp \
	wrappers/minisat22.cpp \
	utils.cpp \
	peer.cpp \
	../solvers/minisat22/core/Solver.cc \
	main.cpp

BUILD_DIR := ../build
CXXFLAGS := -g -std=c++11 -O0 

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS 

INCDIRS := ./ ../include ../solvers/minisat22 ../solvers/minisat22/core

TARGET_DIR := ../bin

