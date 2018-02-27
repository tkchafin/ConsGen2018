#! /bin/bash

mkdir -p $HOME/local $HOME/local/src $HOME/local/bin

#remove any existing files
rm -r $HOME/local/src/raxml
rm $HOME/local/bin/raxml

#cd into $HOME/local/src
cd $HOME/local/src/

echo "Fetching RAxML source code..."
#copy the raxml code to the directory
git clone https://github.com/stamatak/standard-RAxML.git

#cd into RAxML directory
cd $HOME/local/src/standard-RAxML/

echo "Compiling RAxML..."
#compile the two versions of RAxML
make -f Makefile.SSE3.MPI.gcc
make -f Makefile.SSE3.PTHREADS.gcc

#make symbolic links for each in $HOME/local/bin
cd $HOME/local/bin
ln -s $HOME/local/src/standard-RAxML/raxmlHPC-PTHREADS-SSE3 raxml
ln -s $HOME/local/src/standard-RAxML/raxmlHPC-MPI-SSE3 raxmlMPI

echo "Fetching jModelTest..."
cd $HOME/local/src
curl -OL https://github.com/ddarriba/jmodeltest2/files/157117/jmodeltest-2.1.10.tar.gz
tar -xvzf jmodeltest-2.1.10.tar.gz
cd jmodeltest-2.1.10
ln -s $HOME/local/src/jmodeltest-2.1.10/jModelTest.jar $HOME/local/bin/.

cd $HOME/local/bin
touch jmodeltest
chmod 777 jmodeltest
echo '#!/bin/bash' > jmodeltest
echo "java -jar $HOME/local/bin/jModelTest.jar" >> jmodeltest

echo "Done!"
exit
