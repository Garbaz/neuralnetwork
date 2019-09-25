progname=neuralnetwork
srcfiles=src/utilities.c src/neuralnetwork.c src/main.c 
libs=-pthread
flags=
buildfile=build/${progname}
default:
	if [ ! -e "build" ];then mkdir build;fi
	gcc -O2 ${libs} ${flags} -o ${buildfile} ${srcfiles}

optimized:
	if [ ! -e "build" ];then mkdir build;fi
	gcc ${libs} ${flags} -O3 -o ${buildfile} ${srcfiles}

debug:
	if [ ! -e "build" ];then mkdir build;fi
	gcc ${libs} ${flags} -Wall -g -o ${buildfile} ${srcfiles}

run:
	@${buildfile}
