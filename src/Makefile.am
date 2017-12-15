bin_PROGRAMS = tune

# set the include path found by configure
INCLUDES= $(all_includes)

AM_CXXFLAGS=-O0 -fsigned-char
CFLAGS=-O0 -g
CXXFLAGS=-O0 -g

tune_SOURCES = tune.cpp parse_tune.cpp client.cpp netmes.cpp

tune_LDFLAGS = $(all_libraries) 
tune_LDFLAGS += -lpthread 

tune_CPPFLAGS = $(all_includes) 
tune_CPPFLAGS += -I. -I./include -fpack-struct=1
#-D_ATS_

noinst_HEADERS = include/cfg.h include/unipar.h include/lib.h include/type.h include/client.h include/parse_tune.h include/tune.h include/netmes.h

EXTRA_DIST = README
