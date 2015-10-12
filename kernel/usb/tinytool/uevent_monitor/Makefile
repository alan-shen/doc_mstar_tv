CC=gcc
#CC=/home/b645/gerrit/4.9/bin/x86_64-linux-gnu-gcc
BINNAME=getuevent_host
MAINFILE=netlink.c
TIME=`date`
#FLAGS=--static -fdiagnostics-color=always

all:
	@rm -f timestamp.c
	@echo "char DATE[] = \"`date +%Y/%m/%d-%H:%M:%S`\";" > timestamp.c
	$(CC) $(MAINFILE) timestamp.c -o $(BINNAME) ${FLAGS}
	@rm -f timestamp.c

clean:
	@rm $(BINNAME)
