CFLAGS=-g -Wall -O2
all: offlinebackup
jbackup: clean
	tar -jcf - . | jbackup src.offlinebackup.tar.bz2
dance: clean
	scp -pr * dance:src/offlinebackup/
offlinebackup: main.o yesno.o options.o srctodest.o dirlist.o desttree.o destbyname.o destbytime.o srctree.o dirtree.o spacestats.o backupfiles.o fileio.o performance.o common/utils.o common/spool.o common/fastmem.o
	gcc -o offlinebackup $^
clean:
	rm -f core *.o offlinebackup common/*.o
.PHONY: jbackup clean
