.all: steamerboards

SDK=$(HOME)/ins/steam-sdk
CXXFLAGS=-I$(SDK)/public
LDFLAGS=-L$(SDK)/redistributable_bin/linux64 -lsteam_api
DEST_HOST=blind:/srv/b.goeswhere.com/

steamerboards: main.o boards.o
	$(CXX) -o $@ main.o boards.o $(LDFLAGS)

run: steamerboards
	env LD_LIBRARY_PATH=$(SDK)/redistributable_bin/linux64 ./steamerboards > polybridge-scores.lst
	python3 parse.py polybridge-scores.lst > polybridge-table.txt
	scp polybridge-table.txt polybridge-scores.lst $(DEST_HOST)

