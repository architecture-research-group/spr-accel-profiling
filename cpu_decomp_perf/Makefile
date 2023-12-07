IAA_INSTALL=../qpl/install
EXAMPLE_INC=../qpl/examples/low-level-api
ISAL_INC=isa-l/include
INCLUDES= -I$(IAA_INSTALL)/include -I$(EXAMPLE_INC) -I$(ISAL_INC)
FLAGS= -fpermissive 
LIBS= -L$(IAA_INSTALL)/lib

.PHONY: clean

all: decomp_latency_isal harness

decomp_latency: decomp_latency.cpp
	g++ $(FLAGS) $(INCLUDES) $(LIBS) -o decomp_latency decomp_latency.cpp -L$(IAA_INSTALL)/lib64/libqpl.a -ldl -lqpl -lz -lisal

decomp_latency_isal: decomp_latency_isal.cpp
	g++ $(FLAGS) $(INCLUDES) $(LIBS) -o decomp_latency_isal decomp_latency_isal.cpp -lisal -lz

harness: harness.cpp
	$(CXX) $(FLAGS) $(INCLUDES) $(LIBS) -o harness harness.cpp -lisal -lz

clean:
	rm -f  decomp_latency decomp_latency_isal harness