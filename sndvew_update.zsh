#!/usr/bin/env zsh

cd /home/gandalfs_cat/sndvew
git pull && make

if [[ $? == 0 ]]
then
	echo "Update successful!"
else
	echo "Update failed. Please check the output above."
fi

echo "Press enter to close this window."
read line
