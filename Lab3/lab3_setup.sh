#!/bin/bash 

cd ~/Desktop/ConsGen2018/Lab3 

#Edit bash profile 
echo "Modifying your .bash_profile..."
echo "Here's a cat I made for you:" 
cat programs/misc/niceCat.txt
echo ""

cat ./bash_profile_template.txt > ~/.bash_profile

echo "Loading modified .bash_profile..."
echo ""
source ~/.bash_profile

#Install programs 
echo "Installing programs..."
if [ -d ~/local ]; then
  true
else
  mkdir ~/local
fi

if [ -d ~/local/src ]; then 
  cp programs/src/* ~/local/src/.
else
  mkdir ~/local/src
  cp programs/src/* ~/local/src/.
fi

if [ -d ~/local/bin ]; then
  cp programs/bin/* ~/local/bin/.
else
  mkdir ~/local/src
  cp programs/bin/* ~/local/bin/.
fi

echo "Programs should be installed. To test, type \"muscle\", if your computer returns an error (\"Command not found\"), please ask your TA for help."
