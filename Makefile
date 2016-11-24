progname=neuralnetwork
srcfiles=src/utilities.c src/neuralnetwork.c src/main.c 
libs=-pthread
buildfile=build/${progname}
default:
	if [ ! -e "build" ];then mkdir build;fi
	gcc -O2 ${libs} -o ${buildfile} ${srcfiles}

optimized:
	if [ ! -e "build" ];then mkdir build;fi
	gcc ${libs} -O3 -o ${buildfile} ${srcfiles}

debug:
	if [ ! -e "build" ];then mkdir build;fi
	gcc ${libs} -Wall -g -o ${buildfile} ${srcfiles}

run:
	@${buildfile}
