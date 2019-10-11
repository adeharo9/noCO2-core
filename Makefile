CXX = g++
CXX_FLAGS = -O3 -Icommon/libs/json/include

core: main.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@
