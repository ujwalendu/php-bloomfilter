<?php

bloomfilter_add("Hello");
var_dump(bloomfilter_check("Hello"));
var_dump(bloomfilter_check("Hello2"));
var_dump(bloomfilter_check("Hel"));
var_dump(bloomfilter_check("system"));
var_dump(bloomfilter_check("virus"));

echo "\n\n=================Stats===================\n\n";
bloomfilter_stats();
