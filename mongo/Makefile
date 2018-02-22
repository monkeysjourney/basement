#sudo apt install libbson-dev
#sudo apt-get install libmongoc-1.0-0


INCLUDE=-I /usr/include/libbson-1.0 -I /usr/include/libmongoc-1.0
LIB= -lmongoc-1.0 -lbson-1.0

all:
	@gcc main.c $(INCLUDE) $(LIB)
