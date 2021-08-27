default:	build

build:	clean
	g++ -Wall -o main -l curl main.cpp 

clean:
	rm -rf main

test: build
	./curl https://freegeoip.app/json/ 