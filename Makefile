progname=neuralnetwork
srcfile=src/${progname}.c
buildfile=build/${progname}
default:
	if [ ! -e "build" ];then mkdir build;fi
	gcc -O2 -o ${buildfile} src/${progname}.c 

debug:
	if [ ! -e "build" ];then mkdir build;fi
	gcc -Wall -O2 -o ${buildfile} src/${progname}.c 

toasm:
	if [ ! -e "build" ];then mkdir build;fi
	gcc -Wall -O2 -o ${buildfile} src/${progname}.c 

run:
	@${buildfile}
