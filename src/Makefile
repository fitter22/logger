CXX = g++
CXXFLAGS = -pthread -std=c++11
INCS = -I./
OBJS = 

test: test.cpp $(OBJS)
	$(CXX) $(CXXFLAGS) -o test test.cpp $(OBJS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@ $(INCS)

clean:
	rm *.o test gmon.out
