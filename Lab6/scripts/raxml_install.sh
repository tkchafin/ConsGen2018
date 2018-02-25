#! /bin/bash

mkdir -p $HOME/local $HOME/local/src $HOME/local/bin

#remove any existing files
rm -r $HOME/local/src/raxml
rm $HOME/local/bin/raxml

#cd into $HOME/local/src
cd $HOME/local/src/

#copy the raxml code to the directory
git clone https://github.com/stamatak/standard-RAxML.git

#cd into RAxML directory
cd $HOME/local/src/standard-RAxML/

#compile the two versions of RAxML
make -f Makefile.SSE3.MPI.gcc
make -f Makefile.SSE3.PTHREADS.gcc

#make symbolic links for each in $HOME/local/bin
cd $HOME/local/bin
ln -s $HOME/local/src/standard-RAxML/raxmlHPC-PTHREADS-SSE3 raxml
ln -s $HOME/local/src/standard-RAxML/raxmlHPC-MPI-SSE3 raxmlMPI

exit
