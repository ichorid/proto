TARGET := fhsat

CXX=mpic++

SOURCES := \
	fitness.cpp \
	search/taboo.cpp \
	wrappers/minisat22.cpp \
	wrappers/lingeling.cpp \
	utils.cpp \
	peer.cpp \
	../solvers/minisat22/core/Solver.cc \
	../solvers/lingeling/lglib.c \
	main.cpp

BUILD_DIR := ../build
CXXFLAGS := -g -std=c++11 -O5 

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS NDBLSCR NLGLOG NDEBUG NCHKSOL NLGLPICOSAT NLGLDRUPLIG NLGLYALSAT

INCDIRS := ./ ../include ../solvers/minisat22 ../solvers/minisat22/core ../ ../solvers/lingeling

TARGET_DIR := ../bin

