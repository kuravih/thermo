CXX = g++
CC = gcc

LIB_DIR = lib
# =====================================================================================================================




# ==== kato source file ===============================================================================================
KATO_DIR = $(LIB_DIR)/kato

KATO_SOURCES = $(KATO_DIR)/src/function.cpp $(KATO_DIR)/include/kato/serial_link.cpp
KATO_HEADERS = $(KATO_DIR)/include/kato/function.h $(KATO_DIR)/include/kato/serial_link.h $(KATO_DIR)/include/kato/log.h
KATO_OBJS = $(addprefix .kato_, $(addsuffix .o, $(basename $(notdir $(KATO_SOURCES)))))

KATO_CXXFLAGS = -I$(KATO_DIR)/include
# ==== kato source file ===============================================================================================





# ==== thermo source files ==========================================================================================
THERMO_EXE = runner
THERMO_SOURCES = runner.cpp thermo.cpp
THERMO_HEADERS = thermo.h
THERMO_OBJS = $(addprefix ., $(addsuffix .o, $(basename $(notdir $(THERMO_SOURCES)))))
# ==== thermo source files ==========================================================================================





# ==== ds2484 source files ============================================================================================
DS2484_DIR = $(LIB_DIR)/ds2484

DS2484_SOURCES = $(DS2484_DIR)/src/ds2484.cpp
DS2484_HEADERS = $(DS2484_DIR)/include/ds2484/ds2484.h
DS2484_OBJS = $(addprefix .ds2484_, $(addsuffix .o, $(basename $(notdir $(DS2484_SOURCES)))))

DS2484_CXXFLAGS = -I$(DS2484_DIR)/include
DS2484_LIBS = -li2c
# ==== ds2484 source files ============================================================================================





# ==== gcc flags ======================================================================================================
CXXFLAGS = -Wall -Wformat -pthread
CXXFLAGS += $(KATO_CXXFLAGS)
CXXFLAGS += $(DS2484_CXXFLAGS)

LIBS = -lpthread 
LIBS += $(KATO_LIBS)
LIBS += $(DS2484_LIBS)

CFLAGS = $(CXXFLAGS)
CFLAGS_STD = $(CFLAGS) -std=c17
CXXFLAGS_STD = $(CXXFLAGS) -std=c++17
# ==== gcc flags ======================================================================================================





# ==== build rules ====================================================================================================
.kato_%.o:$(KATO_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

.ds2484_%.o: $(DS2484_DIR)/src/%.cpp
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

.%.o:%.cpp $(THERMO_HEADERS)
	$(CXX) $(CXXFLAGS_STD) -c -o $@ $<

all: $(THERMO_EXE)
	@echo Build complete

$(THERMO_EXE): $(KATO_OBJS) $(THERMO_OBJS) $(DS2484_OBJS)
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

clean_thermo:
	rm -f $(THERMO_EXE) $(THERMO_OBJS)

clean_ds2484:
	rm -f $(THERMO_EXE) $(DS2484_OBJS)

clean_kato:
	rm -f $(CAMLIVE_EXE) $(KATO_OBJS)

clean: clean_thermo clean_kato clean_ds2484
# ==== build rules ====================================================================================================
