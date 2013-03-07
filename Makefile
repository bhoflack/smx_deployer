CC=gcc
CFLAGS=-g

smx_deployer: smx_deployer.o

clean:
	rm -f *.o
	rm -f smx_deployer
