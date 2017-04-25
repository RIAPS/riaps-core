rm -rf  package
source version.sh
mkdir -p package/riaps-core-amd64/DEBIAN
mkdir -p package/riaps-core-amd64/opt/riaps/armhf/lib/
cp DEBIAN/riaps-core-amd64.control package/riaps-core-amd64/DEBIAN/control
cp -r amd64-opt/* package/riaps-core-amd64/.
cp -r armhf-opt/opt/riaps/armhf/lib/* package/riaps-core-amd64/opt/riaps/armhf/lib/.

sed s/@version@/$coreversion/g -i package/riaps-core-amd64/DEBIAN/control

fakeroot dpkg-deb --build package/riaps-core-amd64/

mkdir -p package/riaps-core-armhf/DEBIAN
mkdir -p package/riaps-core-armhf/opt/riaps/armhf/lib/
cp DEBIAN/riaps-core-armhf.control package/riaps-core-armhf/DEBIAN/control
cp -r armhf-opt/* package/riaps-core-armhf/.
sed s/@version@/$coreversion/g -i package/riaps-core-armhf/DEBIAN/control
fakeroot dpkg-deb --build package/riaps-core-armhf/
