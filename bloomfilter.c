/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Ujwalendu Prakash <ujwalendu@gmail.com>                      |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_bloomfilter.h"

#include <stdio.h>
#include <stdint.h>

/* If you declare any globals in php_bloomfilter.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(bloomfilter)
*/

/* True global resources - no need for thread safety here */
static int le_bloomfilter;
static struct bloomfilter bloomfilter;

/* {{{ bloomfilter_functions[]
 *
 * Every user visible function must have an entry in bloomfilter_functions[].
 */
const zend_function_entry bloomfilter_functions[] = {
	PHP_FE(confirm_bloomfilter_compiled,	NULL)		/* For testing, remove later. */
    PHP_FE(bloomfilter_add, NULL)
    PHP_FE(bloomfilter_check, NULL)
    PHP_FE(bloomfilter_stats, NULL)
	PHP_FE_END	/* Must be the last line in bloomfilter_functions[] */
};
/* }}} */

/* {{{ bloomfilter_module_entry
 */
zend_module_entry bloomfilter_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"bloomfilter",
	bloomfilter_functions,
	PHP_MINIT(bloomfilter),
	PHP_MSHUTDOWN(bloomfilter),
	PHP_RINIT(bloomfilter),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(bloomfilter),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(bloomfilter),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BLOOMFILTER
ZEND_GET_MODULE(bloomfilter)
#endif

/* {{{ PHP_INI
 */
PHP_INI_BEGIN()
    //STD_PHP_INI_ENTRY("bloomfilter.data_file",NULL, PHP_INI_ALL, OnUpdateLong, global_value, zend_bloomfilter_globals, bloomfilter_globals)
    PHP_INI_ENTRY("bloomfilter.data_file", NULL, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("bloomfilter.entries_count", DEFAULT_ENTRIES_COUNT, PHP_INI_ALL, NULL)
    PHP_INI_ENTRY("bloomfilter.error_prob", DEFAULT_ERROR_PROB, PHP_INI_ALL, NULL)
PHP_INI_END()
/* }}} */

/* {{{ php_bloomfilter_init_globals
 *//*
static void php_bloomfilter_init_globals(zend_bloomfilter_globals *bloomfilter_globals)
{
	bloomfilter_globals->global_string = NULL;
}*/
/* }}} */

static void load_data_to_bloom_filter(struct bloomfilter *bloomfilter, const char *data_file) {
    char word[1024];
    FILE *fp = fopen(data_file, "r");
    if(fp == NULL) {
        fprintf(stderr, "Error in opening data file %s\n", data_file);
        exit(1);
    }

    while (!feof(fp)) {
        int count = fscanf(fp, "%s", word);
        _bloomfilter_add(bloomfilter, word, strlen(word));
    }

    fclose(fp);
}

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(bloomfilter)
{
	REGISTER_INI_ENTRIES();
    
    const char *data_file = INI_STR("bloomfilter.data_file");
    long entries_count = INI_INT("bloomfilter.entries_count");
    double error_prob = INI_FLT("bloomfilter.error_prob");
    _bloomfilter_init(&bloomfilter, entries_count, error_prob);
    load_data_to_bloom_filter(&bloomfilter, data_file);
    //bloom_print(&bloomfilter);
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(bloomfilter)
{
	UNREGISTER_INI_ENTRIES();

    bloom_free(&bloomfilter);
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(bloomfilter)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(bloomfilter)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(bloomfilter)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "bloomfilter support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}
/* }}} */


/* Remove the following function when you have successfully modified config.m4
   so that your module can be compiled into PHP, it exists only for testing
   purposes. */

/* Every user-visible function in PHP should document itself in the source */
/* {{{ proto string confirm_bloomfilter_compiled(string arg)
   Return a string to confirm that the module is compiled in */
PHP_FUNCTION(confirm_bloomfilter_compiled)
{
	char *arg = NULL;
	int arg_len, len;
	char *strg;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &arg, &arg_len) == FAILURE) {
		return;
	}

	len = spprintf(&strg, 0, "Congratulations! You have successfully modified ext/%.78s/config.m4. Module %.78s is now compiled into PHP.", "bloomfilter", arg);
	RETURN_STRINGL(strg, len, 0);
}

/* }}} */
/* The previous line is meant for vim and emacs, so it can correctly fold and 
   unfold functions in source code. See the corresponding marks just before 
   function definition, where the functions purpose is also documented. Please 
   follow this convention for the convenience of others editing your code.
*/
/**
*
* This code has been taken from Wikipedia
* http://en.wikipedia.org/wiki/MurmurHash#Implementations
**/ 
static uint32_t murmur3_32(const char *key, uint32_t len, uint32_t seed) {
	static const uint32_t c1 = 0xcc9e2d51;
	static const uint32_t c2 = 0x1b873593;
	static const uint32_t r1 = 15;
	static const uint32_t r2 = 13;
	static const uint32_t m = 5;
	static const uint32_t n = 0xe6546b64;
 
	uint32_t hash = seed;
 
	const int nblocks = len / 4;
	const uint32_t *blocks = (const uint32_t *) key;
	int i;
	for (i = 0; i < nblocks; i++) {
		uint32_t k = blocks[i];
		k *= c1;
		k = (k << r1) | (k >> (32 - r1));
		k *= c2;
 
		hash ^= k;
		hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;
	}
 
	const uint8_t *tail = (const uint8_t *) (key + nblocks * 4);
	uint32_t k1 = 0;
 
	switch (len & 3) {
	case 3:
		k1 ^= tail[2] << 16;
	case 2:
		k1 ^= tail[1] << 8;
	case 1:
		k1 ^= tail[0];
 
		k1 *= c1;
		k1 = (k1 << r1) | (k1 >> (32 - r1));
		k1 *= c2;
		hash ^= k1;
	}
 
	hash ^= len;
	hash ^= (hash >> 16);
	hash *= 0x85ebca6b;
	hash ^= (hash >> 13);
	hash *= 0xc2b2ae35;
	hash ^= (hash >> 16);
 
	return hash;
}

static int _bloomfilter_check_add(struct bloomfilter * bloomfilter, const void * buffer, uint32_t len, int add)
{
  if (bloomfilter->is_ready == 0) {
    (void)printf("bloom at %p not initialized!\n", (void *)bloomfilter);
    return -1;
  }

  int hits = 0;
  register uint32_t h1 = murmur3_32(buffer, len, 0x9747b28c);
  register uint32_t h2 = murmur3_32(buffer, len, h1);
  register uint32_t bit_index;
  register unsigned int h_count;
  register unsigned int uint32_index;
  register uint32_t flag;
  register uint32_t uint32_value;

  for (h_count = 0; h_count < bloomfilter->num_hashes; h_count++) {
    bit_index = (h1 + h_count*h2) % bloomfilter->num_bits;
    uint32_index = bit_index / sizeof(uint32_t);     
    uint32_value = bloomfilter->buffer[uint32_index];        // Expensive memory access
    flag = 1 << (bit_index % sizeof(uint32_t));

    if (uint32_value & flag) {
      hits++;
    } else {
      if (add) {
        bloomfilter->buffer[uint32_index] = uint32_value | flag;
      }
    }
  }

  if (hits == bloomfilter->num_hashes) {
    return 1;                  
  }

  return 0;
}


static int _bloomfilter_init(struct bloomfilter * bloomfilter, long entries, double error)
{
  bloomfilter->is_ready = 0;

  if (entries < 1 || error == 0) {
    return 1;
  }

  bloomfilter->entries = entries;
  bloomfilter->error = error;

  double num = log(bloomfilter->error);
  double denom = 0.480453013918201; // ln(2)^2
  bloomfilter->bits_per_entry = -(num / denom);

  double dentries = (double)entries;
  bloomfilter->num_bits = (int)(dentries * bloomfilter->bits_per_entry);

  if (bloomfilter->num_bits % sizeof(uint32_t)) {
    bloomfilter->num_uint32_values = (bloomfilter->num_bits / sizeof(uint32_t)) + 1;
  } else {
    bloomfilter->num_uint32_values = bloomfilter->num_bits / sizeof(uint32_t);
  }

  bloomfilter->num_hashes = (int)ceil(0.693147180559945 * bloomfilter->bits_per_entry);  // ln(2)

  bloomfilter->buffer = (uint32_t *)calloc(bloomfilter->num_uint32_values, sizeof(uint32_t));
  if (bloomfilter->buffer == NULL) {
    return 1;
  }

  bloomfilter->is_ready = 1;
  return 0;
}


static int _bloomfilter_check(struct bloomfilter* bloom, const void * buffer, int len)
{
  return _bloomfilter_check_add(bloom, buffer, len, 0);
}


static int _bloomfilter_add(struct bloomfilter * bloom, const void * buffer, int len)
{
  return _bloomfilter_check_add(bloom, buffer, len, 1);
}


static void _bloomfilter_stats(struct bloomfilter * bloom)
{
  
  php_printf("bloomfilter at %p\n", (void *)bloom);
  php_printf(" -> total count of entries    = %ld\n", bloom->entries);
  php_printf(" -> error probability         = %f\n", bloom->error);
  php_printf(" -> size of bit array         = %d\n", bloom->num_bits);
  php_printf(" -> bits per entry            = %f\n", bloom->bits_per_entry);
  php_printf(" -> total uint32 elem         = %d\n", bloom->num_uint32_values);
  php_printf(" -> count of hashes           = %d\n", bloom->num_hashes);
  
}


static void _bloomfilter_free(struct bloomfilter * bloom)
{
  if (bloom->is_ready) {
    free(bloom->buffer);
  }
  bloom->is_ready = 0;
}

PHP_FUNCTION(bloomfilter_add) {
    char *word;
    int word_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &word, &word_len) == FAILURE) {
        RETURN_NULL();
    }

    if(_bloomfilter_add(&bloomfilter, word, word_len)) {
        RETVAL_TRUE;
        return;
    } else {
        RETVAL_FALSE;
        return;
    }
}

PHP_FUNCTION(bloomfilter_check) {

    char *word;
    int word_len;

    if(zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s", &word, &word_len) == FAILURE) {
        RETURN_NULL();
    }

    if(_bloomfilter_check(&bloomfilter, word, word_len)) {
        RETVAL_TRUE;
        return;
    } else {
        RETVAL_FALSE;
        return;
    }

}

PHP_FUNCTION(bloomfilter_stats) {

    _bloomfilter_stats(&bloomfilter);
}




/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
