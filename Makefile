all: appears appears2 appears1

appears: appears.cpp
	#rm -f *.o
	g++ -Wall -Wno-unused-value -g $< -o $@

appears2: appears.cpp
	#rm -f *.o
	g++ -D 'APPEARS_SEQSIZE=2L' -Wall -Wno-unused-value -g $< -o $@

appears1: appears.cpp
	#rm -f *.o
	g++ -D 'APPEARS_SEQSIZE=1L' -Wall -Wno-unused-value -g $< -o $@

appears appears2: ddebug.h

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

