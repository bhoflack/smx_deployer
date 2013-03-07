CC=gcc
CFLAGS=

smx_deployer: smx_deployer.o

clean:
	rm -f *.o
	rm -f smx_deployer
