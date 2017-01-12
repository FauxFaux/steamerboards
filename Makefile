.all: steamerboards

SDK=$(HOME)/ins/steam-sdk
CXXFLAGS=-I$(SDK)/public
LDFLAGS=-L$(SDK)/redistributable_bin/linux64 -lsteam_api

steamerboards: main.o boards.o
	$(CXX) -o $@ main.o boards.o $(LDFLAGS)

run:
	env LD_LIBRARY_PATH=$(SDK)/redistributable_bin/linux64 ./steamerboards
