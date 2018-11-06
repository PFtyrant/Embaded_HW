CC = gccpxa
CPP = g++
-L = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/
-I1 = /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/
-I2 = /home/pface/microtime/linux/include/

%: %.cpp
	rm -rf $@
	$(CPP) $< -o $@ -lpthread
