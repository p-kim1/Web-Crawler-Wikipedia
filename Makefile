OPTS = -Wall -I/usr/include/libxml2 -std=c++11
TARGETS = serial_crawler parallel_crawler

all: $(TARGETS)	

serial_crawler: serial_crawler.cc crawlerFun.o
	g++ ${OPTS} -o serial_crawler serial_crawler.cc crawlerFun.o -lcurl -lxml2

parallel_crawler: parallel_crawler.cc crawlerFun.o
	g++ ${OPTS} -o parallel_crawler parallel_crawler.cc crawlerFun.o -fopenmp -lcurl -lxml2

crawlerFun.o: crawlerFun.cc crawlerFun.h
	g++ ${OPTS} -c crawlerFun.cc  

clean: 
	rm -f $(TARGETS) *.o
