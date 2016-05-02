MINISAT := minisat22
TARGET := fhsat

CXX=mpic++

SOURCES := \
	fitness.cpp \
	search/taboo.cpp \
	wrappers/minisat22.cpp \
	wrappers/lingeling.cpp \
	utils.cpp \
	peer.cpp \
	../solvers/$(MINISAT)/core/Solver.cc \
	../solvers/lingeling/lglib.c \
	../solvers/lingeling/lglopts.c \
	main.cpp

BUILD_DIR := ../build
CXXFLAGS := -g -std=c++11 -O5 

DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS NDBLSCR NLGLOG NCHKSOL NLGLPICOSAT NLGLDRUPLIG NLGLYALSAT
#DEFS := __STDC_LIMIT_MACROS __STDC_FORMAT_MACROS NDBLSCR NLGLOG NDEBUG NCHKSOL NLGLPICOSAT NLGLDRUPLIG NLGLYALSAT

INCDIRS := ./ ../include ../solvers/$(MINISAT) ../solvers/$(MINISAT)/core ../ ../solvers/lingeling

TARGET_DIR := ../bin

