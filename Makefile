CXX = g++
SKETCH = best

compile: ga.cpp
	${CXX} ga.cpp -o ga

run: ga
	./ga ../Data/2000HopperM/smallbins.csv ../Data/2000HopperM/smallitems.csv ${SKETCH}.sps 2> tmp 

visualize:
	./sketchps <${SKETCH}.sps >${SKETCH}.eps
