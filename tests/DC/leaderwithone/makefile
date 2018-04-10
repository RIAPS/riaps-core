CMAKE := $(shell which cmake 2> /dev/null)
CLANG-FORMAT := $(shell which clang-format 2> /dev/null)
RM-CMD := $(shell which rm 2> /dev/null)
all: 

realclean:		 
ifndef RM-CMD
	rmdir build /s /q
else
	$(shell rm -rf build)
endif

build/armhf/Makefile: CMakeLists.txt
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	mkdir -p build/armhf/
	cd build/armhf/ && \
	cmake -DCMAKE_TOOLCHAIN_FILE=.toolchain.arm-linux-gnueabihf.cmake -DCMAKE_INSTALL_PREFIX="/"  ../..
	@echo "done"
endif

build/amd64/Makefile: CMakeLists.txt
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	mkdir -p build/amd64/
	cd build/amd64/ && \
	cmake -DCMAKE_TOOLCHAIN_FILE=.toolchain.amd64.cmake  -DCMAKE_INSTALL_PREFIX="/" ../..
	@echo "done"
endif

all-amd64: build/amd64/Makefile
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	make -C build/amd64/ -j2
endif

all-armhf: build/armhf/Makefile
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	make -C build/armhf -j2
endif


clean-armhf: build/armhf/Makefile
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	make -C build/armhf clean -j2
endif

clean-amd64: build/amd64/Makefile
ifndef CMAKE
	$(error "cmake is not available. Please install")
else
	make -C build/amd64 clean -j2
endif

reformat: cpp
ifndef CLANG-FORMAT
	$(error "clang-format is not available. Please install")
else
	clang-format -i -style=file cpp/*.cc
	clang-format -i -style=file cpp/include/*.h
endif


package: package.sh
	chmod +x package.sh
	./package.sh
