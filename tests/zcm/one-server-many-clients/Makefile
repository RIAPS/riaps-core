ZCM_HEADERS=/usr/local/include/zcm
ZCM_LIBRARY=/usr/local/lib

TEST_INCLUDE_DIR=include
TEST_SOURCE_DIR=src
TEST_BUILD_DIR=build
TEST_CONFIG_DIR=config

all:
	mkdir -p $(TEST_BUILD_DIR)
	cp -r $(TEST_CONFIG_DIR)/* $(TEST_BUILD_DIR)/.
	g++ -fPIC -shared $(TEST_SOURCE_DIR)/server_1.cpp -o $(TEST_BUILD_DIR)/server_1.so -I$(TEST_INCLUDE_DIR) -std=c++11 -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq -lboost_filesystem
	g++ -fPIC -shared $(TEST_SOURCE_DIR)/client_1.cpp -o $(TEST_BUILD_DIR)/client_1.so -I$(TEST_INCLUDE_DIR) -std=c++11 -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq -lboost_filesystem
	g++ -fPIC -shared $(TEST_SOURCE_DIR)/client_2.cpp -o $(TEST_BUILD_DIR)/client_2.so -I$(TEST_INCLUDE_DIR) -std=c++11 -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq -lboost_filesystem
	g++ -fPIC -shared $(TEST_SOURCE_DIR)/client_3.cpp -o $(TEST_BUILD_DIR)/client_3.so -I$(TEST_INCLUDE_DIR) -std=c++11 -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq -lboost_filesystem
	g++ $(TEST_SOURCE_DIR)/main.cpp -o $(TEST_BUILD_DIR)/actor -I$(TEST_INCLUDE_DIR) -std=c++11 -ldl -pthread -I$(ZCM_HEADERS) -L$(ZCM_LIBRARY) -lzcm -lzmq
clean:
	rm -rf $(TEST_BUILD_DIR)
