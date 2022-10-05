clean:
	rm -rf ping.dSYM utils.dSYM dist
	mkdir ./dist

build: clean
	/usr/bin/clang++ -fcolor-diagnostics -fansi-escape-codes\
	 -Wall -std=c++11 -g\
	 ./src/**.cpp\
	 ./src/ping/**.cpp\
	 -o ./dist/ping
	