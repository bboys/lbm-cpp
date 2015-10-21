wget http://www.multicorebsp.com/downloads/c/1.2.0/MulticoreBSP-for-C.tar.gz
tar xfz MulticoreBSP-for-C.tar.gz
mv MulticoreBSP-for-C lib/
rm MulticoreBSP-for-C.tar.gz
cd lib/MulticoreBSP-for-C
make
make compat

cd ../../
cp lib/MulticoreBSP-for-C/include/* include/