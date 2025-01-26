# Compiler and flags
CXX = g++
BASE_CXXFLAGS = -O3 -std=c++11 -pthread
BASE_LDFLAGS =

PYTHON_CFLAGS = $(shell python3-config --includes)
PYTHON_LDFLAGS = -lpython3.12
# PYTHON_LDFLAGS = $(shell python3-config --ldflags)

# Check if Python is available
#PYTHON_INSTALLED = $(shell python3-config --cflags >/dev/null 2>&1 && echo 1 || echo 0)
PYTHON_INSTALLED = 0

ifeq ($(PYTHON_INSTALLED), 1)
    CXXFLAGS = $(BASE_CXXFLAGS) $(PYTHON_CFLAGS)
    LDFLAGS = $(BASE_LDFLAGS) $(PYTHON_LDFLAGS)
    USE_PYTHON = $(-DUSE_PYTHON)
else
    CXXFLAGS = $(BASE_CXXFLAGS)
    LDFLAGS = $(BASE_LDFLAGS)
    USE_PYTHON = 
endif


# Source files and output
SOURCES = MadN.cpp Calculator.cpp IOResults.cpp Player.cpp Position.cpp RatioAnalyser.cpp \
          GamePlayer.cpp Game.cpp Tournament.cpp RandomPlayer.cpp SimplePlayer.cpp \
          ImprovedSimplePlayer.cpp OptimalPlayer.cpp OptimalPlayerBM.cpp \
          FastOptimalPlayerBMS.cpp Compressor.cpp CompressorR6.cpp Analyser.cpp Main.cpp
OUTPUT = madn.out

# Directories
DIRS = log results backup

# Add Python directories and files if Python is installed
ifeq ($(PYTHON_INSTALLED), 1)
    SOURCES += PythonPlayer.cpp
    DIRS += py_strategies
endif

# Targets
all: $(OUTPUT) setup_dirs

$(OUTPUT): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(USE_PYTHON) $(SOURCES) -o $@ $(LDFLAGS)

setup_dirs:
	@mkdir -p $(DIRS)