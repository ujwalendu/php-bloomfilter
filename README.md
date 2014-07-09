php-bloomfilter
===============

A php extension for bloom filter

You need to have php-devel installed, to build this extension.
```
git clone https://github.com/ujwalendu/php-bloomfilter.git
cd php-bloomfilter
phpize
./configure
make
make install
```

This will copy the php-bloomfilter module (bloomfilter.so) into the lib directory.
```
cp bloomfilter.ini /path/to/php/conf.d/bloomfilter.ini
```
change the following values in bloomfilter.ini file
```
bloomfilter.data_file = provide absolute path of your data file which contains the strings to be added to bloom filter. One string per line.
bloomfilter.entries_count = This is total number of entries you want to be present in bloom filter.
bloomfilter.error_prob = This is the probability of error, for false positives, you want. 
```


The following functions are currently exposed 

```php
<?php

// This is to add to the bloom filter
bloomfilter_add("Hello");
bloomfilter_add("World");
bloomfilter_add("System");
bloomfilter_add("Virus");
bloomfilter_add("aiodo");


// This is to check

bloomfilter_check("hello"); // returns false
bloomfilter_check("Hello"); // returns true. But this may be a false positive
bloomfilter_check("sys");   // returns false
```
