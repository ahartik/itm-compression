echo "Original:"
wc -c  < gcc.man
echo "Compressed:"
../acencode < gcc.man | wc -c
echo "gzip:"
gzip < gcc.man | wc -c 

echo "bzip2:"
bzip2 < gcc.man | wc -c 
