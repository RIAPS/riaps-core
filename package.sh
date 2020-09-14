set -e
rm -rf  package
source version.sh

#AMD64
mkdir -p package/riaps-core-amd64/DEBIAN
mkdir -p package/riaps-core-amd64/usr/arm-linux-gnueabihf/lib/
mkdir -p package/riaps-core-amd64/usr/arm-linux-gnueabihf/include/
mkdir -p package/riaps-core-amd64/usr/aarch64-linux-gnu/lib/
mkdir -p package/riaps-core-amd64/usr/aarch64-linux-gnu/include/
cp DEBIAN/riaps-core-amd64.control package/riaps-core-amd64/DEBIAN/control
cp DEBIAN/postinst package/riaps-core-amd64/DEBIAN/postinst
cp DEBIAN/postrm package/riaps-core-amd64/DEBIAN/postrm
cp -r amd64-opt/* package/riaps-core-amd64/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/lib/* package/riaps-core-amd64/usr/arm-linux-gnueabihf/lib/.
cp -r armhf-opt/usr/arm-linux-gnueabihf/include/* package/riaps-core-amd64/usr/arm-linux-gnueabihf/include/.
cp -r arm64-opt/usr/aarch64-linux-gnu/lib/* package/riaps-core-amd64/usr/aarch64-linux-gnu/lib/.
cp -r arm64-opt/usr/aarch64-linux-gnu/include/* package/riaps-core-amd64/usr/aarch64-linux-gnu/include/.
sed s/@version@/$coreversion/g -i package/riaps-core-amd64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-core-amd64/

#ARMHF
mkdir -p package/riaps-core-armhf/DEBIAN
mkdir -p package/riaps-core-armhf/usr/local/
cp DEBIAN/riaps-core-armhf.control package/riaps-core-armhf/DEBIAN/control
cp DEBIAN/postinst package/riaps-core-armhf/DEBIAN/postinst
cp DEBIAN/postrm package/riaps-core-armhf/DEBIAN/postrm
cp -r armhf-opt/usr/arm-linux-gnueabihf/* package/riaps-core-armhf/usr/local/.
sed s/@version@/$coreversion/g -i package/riaps-core-armhf/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-core-armhf/

#ARM64
mkdir -p package/riaps-core-arm64/DEBIAN
mkdir -p package/riaps-core-arm64/usr/local/
cp DEBIAN/riaps-core-arm64.control package/riaps-core-arm64/DEBIAN/control
cp DEBIAN/postinst package/riaps-core-arm64/DEBIAN/postinst
cp DEBIAN/postrm package/riaps-core-arm64/DEBIAN/postrm
cp -r arm64-opt/usr/aarch64-linux-gnu/* package/riaps-core-arm64/usr/local/.
sed s/@version@/$coreversion/g -i package/riaps-core-arm64/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-core-arm64/
