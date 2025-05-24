CXX = g++
CC = gcc

LIB_DIR = lib
# =====================================================================================================================




# ==== kato source file ===============================================================================================
KATO_HEADERS = $(LIB_DIR)/kato/function.hpp $(LIB_DIR)/kato/log.hpp
KATO_CXXFLAGS = -I$(LIB_DIR)
# ==== kato source file ===============================================================================================




# ==== link source file ===============================================================================================
LINK_HEADERS = $(LIB_DIR)/link/serial_link.hpp
LINK_CXXFLAGS = -I$(LIB_DIR)
# ==== link source file ===============================================================================================




# ==== ds2484 source file =============================================================================================
DS2484_HEADERS = $(LIB_DIR)/ds2484/ds2484.hpp
DS2484_CXXFLAGS = -I$(LIB_DIR)
DS2484_LIBS = -li2c
# ==== ds2484 source file =============================================================================================





# ==== thermo source files ============================================================================================
THERMO_EXE = runner
THERMO_SOURCES = runner.cpp thermo.cpp
THERMO_HEADERS = thermo.h
THERMO_OBJS = $(addprefix ., $(addsuffix .o, $(basename $(notdir $(THERMO_SOURCES)))))
# ==== thermo source files ============================================================================================





# ==== gcc flags ======================================================================================================
CXXFLAGS = -Wall -Wformat -pthread
CXXFLAGS += $(KATO_CXXFLAGS)
CXXFLAGS += $(DS2484_CXXFLAGS)
CXXFLAGS += $(LINK_CXXFLAGS)

LIBS = -lpthread
LIBS += $(DS2484_LIBS)

CFLAGS = $(CXXFLAGS)
CFLAGS_STD = $(CFLAGS) -std=c17
CXXFLAGS_STD = $(CXXFLAGS) -std=c++17
# ==== gcc flags ======================================================================================================





# ==== build rules ====================================================================================================
.%.o:%.cpp $(THERMO_HEADERS)
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

all: $(THERMO_EXE)
	@echo Build complete

$(THERMO_EXE): $(THERMO_OBJS) $(KATO_HEADERS) $(LINK_HEADERS) $(DS2484_HEADERS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean_thermo:
	rm -f $(THERMO_EXE) $(THERMO_OBJS)

clean: clean_thermo
# ==== build rules ====================================================================================================
