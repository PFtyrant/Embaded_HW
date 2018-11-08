G++PXA = arm-unknown-linux-gnu-g++ 
-L = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
-I1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
-I2 = /home/pface/microtime/linux/include/
THREAD = -lpthread
all:
	g++ -std=c++11 -o overcook overcook.cpp $(THREAD)
non_thread:
	g++ -std=c++11 -o overcook overcook.cpp
clean:
	rm overcook
