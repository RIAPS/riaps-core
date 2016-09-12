Publish-Subscribe Test
======================

Test Dependency
----------------

* Install protobuf with -fPIC flag

```bash
$ git clone https://github.com/google/protobuf
$ cd protobuf
$ ./autogen.sh
$ ./configure --prefix=/usr
$ `# Open src/Makefile and add -fPIC to CXXFLAGS`
$ make
$ make check
$ sudo make install
$ sudo ldconfig
```

Running this Test
-----------------

```bash
$ make
$ make run
```