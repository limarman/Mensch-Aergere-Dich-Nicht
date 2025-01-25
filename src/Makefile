# Compiler and flags
CXX = g++
BASE_CXXFLAGS = -O3 -std=c++11 -pthread
BASE_LDFLAGS =

# Python paths (manually controlled to avoid extra flags)
PYTHON_CFLAGS = $(shell python3-config --includes)
# PYTHON_LDFLAGS = -L/usr/lib/python3.12/config-3.12-x86_64-linux-gnu -L/usr/lib/x86_64-linux-gnu -lpython3.12
PYTHON_LDFLAGS = -lpython3.12

# Pybind11 path (installed via apt)
#PYBIND11_CFLAGS = -I/usr/include/pybind11
#PYBIND11_CFLAGS = $(shell python3 -m pybind11 --includes)

# Check if Python is available
PYTHON_INSTALLED = $(shell python3-config --cflags >/dev/null 2>&1 && echo 1 || echo 0)

ifeq ($(PYTHON_INSTALLED), 1)
    CXXFLAGS = $(BASE_CXXFLAGS) $(PYTHON_CFLAGS)
    LDFLAGS = $(BASE_LDFLAGS) $(PYTHON_LDFLAGS)
    USE_PYTHON = 1
else
    CXXFLAGS = $(BASE_CXXFLAGS)
    LDFLAGS = $(BASE_LDFLAGS)
    USE_PYTHON = 0
endif


# Source files and output
SOURCES = MadN.cpp Calculator.cpp IOResults.cpp Player.cpp Position.cpp RatioAnalyser.cpp \
          GamePlayer.cpp Game.cpp Tournament.cpp RandomPlayer.cpp SimplePlayer.cpp \
          ImprovedSimplePlayer.cpp OptimalPlayer.cpp OptimalPlayerBM.cpp PythonPlayer.cpp \
          FastOptimalPlayerBMS.cpp Compressor.cpp CompressorR6.cpp Analyser.cpp Main.cpp
OUTPUT = madn.out

# Directories
DIRS = log results backup py_strategies

# Targets
all: $(OUTPUT) setup_dirs

$(OUTPUT): $(SOURCES)
	$(CXX) $(CXXFLAGS) -DUSE_PYTHON=$(USE_PYTHON) $(SOURCES) -o $@ $(LDFLAGS)

setup_dirs:
	@mkdir -p $(DIRS)