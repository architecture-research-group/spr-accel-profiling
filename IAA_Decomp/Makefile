IAA_INSTALL=../qpl/install
EXAMPLE_INC=../qpl/examples/low-level-api
INCLUDES= -I$(IAA_INSTALL)/include -I$(EXAMPLE_INC)
LIBS= -L$(IAA_INSTALL)/lib

decompression: decompression.cpp
	g++ $(INCLUDES) $(LIBS) -o decompression decompression.cpp -L$(IAA_INSTALL)/lib64/libqpl.a -ldl -lqpl