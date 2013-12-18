CXX = g++
SKETCH = best

ga: ga.cpp
	${CXX} ga.cpp -o ga

run: ga
	./ga ../Data/2000HopperM/smallbins.csv ../Data/2000HopperM/smallitems.csv ${SKETCH}.sps 2> tmp 

visualize: run
	./sketchps <${SKETCH}.sps >${SKETCH}.eps
