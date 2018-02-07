#!/bin/bash  

#Edit bash profile 
echo "Modifying your .bash_profile..."
echo "Here's a cat I made for you:" 
cat programs/misc/niceCat.txt
echo ""

cat ./bash_profile_template.txt > ~/.bash_profile

echo "Loading modified .bash_profile..."
echo ""


#Install programs 
echo "Installing programs..."
mkdir -p ~/local
mkdir -p ~/local/src
cp programs/src/* ~/local/src/

mkdir -p ~/local/bin
cp programs/bin/* ~/local/bin/

echo ""
echo "###IMPORTANT: Now type \"source ~/.bash_profile\" exactly to load these changes.###"
echo ""
echo "To test if it worked correctly, type \"muscle\", if your computer returns an error (\"Command not found\"), please ask your TA for help."
