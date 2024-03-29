# Test mode
TEST = off

# Debug mode
DBG = off

# Optimization switch
OPT = on

# Include dir : -I//diretório
INCLUDES =

# Objects : nome.o
OBJS =

# Libraries : -L//diretório -nomelib
LIB =

# Programs
PAB_OBJ = pab.o
PAB_EXE = pab

CXX = g++

USER_FLAGS = -std=c++14

# Compiler flags for debugging
ifeq ($(OPT), off)
	USER_FLAGS += -ggdb3 -fexceptions -fno-omit-frame-pointer \
		-fno-optimize-sibling-calls -fno-inline
else
	USER_FLAGS += -O3 -fomit-frame-pointer -funroll-loops
	ifeq ($(CXX), g++)
		USER_FLAGS += -ftracer -fpeel-loops -fprefetch-loop-arrays
	endif
endif


# Enabling debug mode (printf)
ifeq ($(DBG), on)
	USER_FLAGS += -D DEBUG
endif

# Enabling test mode
ifeq ($(TEST), on)
	USER_FLAGS += -D TEST
endif


# Include parallel stuff
#USER_FLAGS += -fopenmp


# Warning flags
USER_FLAGS += -Wall -Wextra -Wcast-align -Wcast-qual -Wdisabled-optimization \
	-Wformat=2 -Winit-self -Wmissing-format-attribute -Wshadow \
	-Wpointer-arith -Wredundant-decls -Wstrict-aliasing=2 \
	-Wfloat-equal -Weffc++

ifeq ($(CXX), g++)
	USER_FLAGS += -Wunsafe-loop-optimizations
endif

CXXFLAGS = $(USER_FLAGS)

.PHONY: all debug
.SUFFIXES: .cpp .o

all: pab

pab: $(OBJS) $(PAB_OBJ)
	@echo "--> Linking objects... "
	$(CXX) $(CXXFLAGS) $(OBJS) $(PAB_OBJ) -o $(PAB_EXE) $(LIB)
	@echo

.cpp.o:
	@echo "--> Compiling $<..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(USER_DEFINES) -c $< -o $@
	@echo

clean:
	@echo "--> Cleaning compiled..."
	rm -rf $(PAB_EXE) $(SOLUCAO_EXE)
	rm -rf *.o
