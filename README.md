php-bloomfilter
===============

A php extension for bloom filter


You need to have php-devel installed, to build this extension.

git clone https://github.com/ujwalendu/php-bloomfilter.git

cd php-bloomfilter
phpize
./configure
make
make install

This will copy the php-bloomfilter module (bloomfilter.so) into the lib directory.

cp bloomfilter.ini /path/to/php/conf.d/bloomfilter.ini

change the following values

bloomfilter.data_file = provide absolute path of your data file which contains the strings to be added to bloom filter. One string per line.
bloomfilter.entries_count = This is total number of entries you want to be present in bloom filter.
bloomfilter.error_prob = This is the probability of error, for false positives, you want. 

