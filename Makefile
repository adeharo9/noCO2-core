CXX = g++
CXX_FLAGS = -O3 -Icommon/json/include

core: source/main.cpp
	$(CXX) $(CXX_FLAGS) $< -o $@
