CXXFLAGS+=--std=c++17 -Wall -Wextra -O0 -g -D_GLIBCXX_DEBUG
CXXFLAGS+=-fdiagnostics-color=auto
LDFLAGS+=-lutil -lboost_iostreams -lboost_system -lboost_filesystem
INCLUDE_FLAGS = -I../../../Libs/DSPFilters/include/ -I../../../../../dpf/distrho -I../../../../../dpf/dgl -I../../../Common/Structures -I../../../Common/Widgets -I../../../Common/Utils -I../../../Resources -I../../../Config -I../../../Libs/inih

all: TestRCFilter.cpp RCFilter.o
	clang++ $^ $(CXXFLAGS) $(LDFLAGS) $(INCLUDE_FLAGS) -o TestRCFilter

RCFilter.o: ../src/RCFilter.cpp
	clang++ -c ../src/RCFilter.cpp $(CXXFLAGS) $(LDFLAGS) $(INCLUDE_FLAGS) -o RCFilter.o

clean:
	rm -f TestRCFilter.o TestRCFilter
