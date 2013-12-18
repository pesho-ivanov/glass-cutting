CXX = g++

compile: ga
	${CXX} ga.cpp -o ga

run: 
	./ga ../Data/2000HopperM/smallbins.csv ../Data/2000HopperM/smallitems.csv
