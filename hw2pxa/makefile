G++PXA = arm-unknown-linux-gnu-g++ 
-L = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
-I1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
-I2 = /home/pface/microtime/linux/include/

all:
	g++ -std=c++11 -o overcook overcook.cpp
	g++pxa -o arm_overcook overcook.cpp
window:
	g++ -std=c++11 -o overcook overcook.cpp
pxa:
	g++pxa  -o arm_overcook overcook.cpp -L $(-L) -I $(-I1) -I $(-I2)
clean:
	rm overcook
