#!/bin/bash 


printNice(){
#Array of nice things to say! 
declare -a niceThings=("Very nice, great success!"
"A+ effort"
"Good job today"
"Nice shirt"
"You're a cupcake in a world of muffins"
"GOOD JOB!"
"You can do it! Do the thing!" 
);

echo "Here's a randomly chosen encouraging message, chosen just for you:"
selectedexpression=${niceThings[$RANDOM % ${#niceThings[@]} ]}
echo $selectedexpression
}

cd ~/Desktop/ConsGen2018/Lab3 

#Edit bash profile 
echo "Modifying your .bash_profile!"
echo "Here's a cat I made for you:" 
cat programs/misc/niceCat.txt
echo ""

cat ./bash_profile_template.txt > ~/.bash_profile

echo "Loading modified .bash_profile..."
echo ""
source ~/.bash_profile

printNice


