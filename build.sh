set -e

source version.sh
rm -rf build-amd64
mkdir build-amd64
pushd  build-amd64
cmake -Darch=amd64 ..
make VEBOSE=1 -j2
make DESTDIR=../amd64-opt/ install
popd

echo "doing the armhf build"
rm -rf build-armhf
mkdir build-armhf
pushd  build-armhf
cmake -Darch=armhf ..
make VERBOSE=1 -j2
make DESTDIR=../armhf-opt/ install
popd

echo "doing the armhf build"
rm -rf build-arm64
mkdir build-arm64
pushd  build-arm64
cmake -Darch=arm64 ..
make VERBOSE=1 -j2
make DESTDIR=../arm64-opt/ install
popd
