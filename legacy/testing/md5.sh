#!/bin/sh

MD5_1=$(../acencode < gcc.man | ../acdecode | md5sum)
MD5_2=$(md5sum < gcc.man)

if [ "$MD5_1" != "$MD5_2" ]; then 
echo MD5 mismatch!
echo $MD5_1
echo $MD5_2
else
    echo Checksums match
fi
