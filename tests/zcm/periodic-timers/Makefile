ZCM_HEADERS=/usr/local/include/zcm
ZCM_LIBRARY=/usr/local/lib

TEST_INCLUDE_DIR=include
TEST_SOURCE_DIR=src
TEST_BUILD_DIR=build
TEST_CONFIG_DIR=config

all:
	mkdir -p $(TEST_BUILD_DIR)
	cp -r $(TEST_CONFIG_DIR)/* $(TEST_BUILD_DIR)/.
	g++ -std=c++11 -fPIC -shared $(TEST_SOURCE_DIR)/periodic_timers.cpp -o $(TEST_BUILD_DIR)/periodic_timers.so -I$(TEST_INCLUDE_DIR)  -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq
	g++ -std=c++11 $(TEST_SOURCE_DIR)/main.cpp -o $(TEST_BUILD_DIR)/actor -I$(TEST_INCLUDE_DIR) -I$(TEST_PROTO_DIR) -ldl -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq
run:
	cd $(TEST_BUILD_DIR) && export LD_LIBRARY_PATH=$(PWD):$(LD_LIBRARY_PATH) && ./actor
clean:
	rm -rf $(TEST_BUILD_DIR)
