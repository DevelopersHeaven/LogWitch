#/bin/sh

set -e
set -x
echo "This script helps you to build a package for ubuntu from scratch."
echo "We will create a directory called 'buildUnuntuPackage' for you"
echo "and we will initialize it to the ubuntu defaults."
echo "After these steps, we will starting make and run the package generator."

if [ -d "buildUbuntuPackage" ]; then
  rm -r buildUbuntuPackage
fi

CODENAME=`lsb_release -c -s`

mkdir buildUbuntuPackage
cd buildUbuntuPackage
cmake .. -DCMAKE_BUILD_TYPE=release -DCMAKE_INSTALL_PREFIX:PATH=/usr -DTARGET_SYSTEM="${CODENAME}"
make $1
# Set umask to produce a correct permission debian file.
umask 022
cpack -G DEB

if [ -n "${BUILD_LW_POSTFIX}" ]; then
	for i in *.deb; do mv -v $i `basename $i .deb`.${BUILD_LW_POSTFIX}.deb; done
fi

cp *.deb ..

echo "Cleaning up!"
cd ..
rm -r buildUbuntuPackage
