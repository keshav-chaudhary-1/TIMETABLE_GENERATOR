CXX      = g++
CXXFLAGS = -std=c++17 -Wall -O2
TARGET   = timetable_cse2yr

SRCS = main.cpp \
       src/input_module.cpp \
       src/conflict_graph.cpp \
       src/scheduler.cpp \
       src/checker.cpp \
       src/optimizer.cpp \
       src/output_module.cpp

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) output_v*.csv output_latest*.csv

run: all
	./$(TARGET)

web:
	node server.js
