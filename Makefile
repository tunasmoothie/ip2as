ip2as: ip2as.o
	g++ ip2as.o -o ip2as
	
ip2as.o: ip2as.cpp
	g++ -c ip2as.cpp

