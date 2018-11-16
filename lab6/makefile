THREAD = -lpthread

%:%.c
	g++ -std=c++11 $< -o $@ $(THREAD)
SEM:
	g++ -std=c++11 race.c -o race $(THREAD) -D USE_SEM
clean:
	rm overcook
