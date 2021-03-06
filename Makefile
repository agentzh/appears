all: appears appears2 appears1 t/gensample

appears: appears.cpp
	#rm -f *.o
	g++ -O2 -Wall -Wno-unused-value -g $< -o $@

appears2: appears.cpp
	#rm -f *.o
	g++ -D 'APPEARS_SEQSIZE=2L' -O2 -Wall -Wno-unused-value -g $< -o $@

appears1: appears.cpp
	#rm -f *.o
	g++ -D 'APPEARS_SEQSIZE=1L' -O2 -Wall -Wno-unused-value -g $< -o $@

appears appears2: ddebug.h

t/gensample: t/gensample.c
	gcc -O2 -Wall -o $@ $<

test: all
	-./appears
	@echo ===========================
	-./appears no-such-file
	@echo ===========================
	-./appears samples/invalid.txt
	@echo ===========================
	-./appears2 samples/good2.txt
	@echo ===========================
	-./appears1 samples/good1.txt
	@echo ===========================
	-./appears1 samples/good2.txt

