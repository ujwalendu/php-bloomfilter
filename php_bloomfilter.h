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

#ifndef PHP_BLOOMFILTER_H
#define PHP_BLOOMFILTER_H

extern zend_module_entry bloomfilter_module_entry;
#define phpext_bloomfilter_ptr &bloomfilter_module_entry

#ifdef PHP_WIN32
#	define PHP_BLOOMFILTER_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_BLOOMFILTER_API __attribute__ ((visibility("default")))
#else
#	define PHP_BLOOMFILTER_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

#include <stdint.h>

PHP_MINIT_FUNCTION(bloomfilter);
PHP_MSHUTDOWN_FUNCTION(bloomfilter);
PHP_RINIT_FUNCTION(bloomfilter);
PHP_RSHUTDOWN_FUNCTION(bloomfilter);
PHP_MINFO_FUNCTION(bloomfilter);

PHP_FUNCTION(confirm_bloomfilter_compiled);	/* For testing, remove later. */
PHP_FUNCTION(bloomfilter_add);
PHP_FUNCTION(bloomfilter_check);
PHP_FUNCTION(bloomfilter_stats);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(bloomfilter)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(bloomfilter)
*/

/* In every utility function you add that needs to use variables 
   in php_bloomfilter_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as BLOOMFILTER_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define BLOOMFILTER_G(v) TSRMG(bloomfilter_globals_id, zend_bloomfilter_globals *, v)
#else
#define BLOOMFILTER_G(v) (bloomfilter_globals.v)
#endif



#define DEFAULT_ENTRIES_COUNT "100"
#define DEFAULT_ERROR_PROB "0.001"

#define DEBUG_LOG 1
#define debug_log(x) \
    if(DEBUG_LOG)   \
        printf(x);   \

struct bloomfilter
{
  long entries; // Number of entries in the bloom filter
  double error; // Error probability
  int num_bits; // Size of bit array
  int num_uint32_values;
  int num_hashes; // Number of hash functions

  double bits_per_entry; // bytes per entry
  uint32_t * buffer; // Actual bit array
  char is_ready; // is bloomfilter ready
};

/*
 * Optimal number of element in bit array is:
 *     bits = (entries * ln(error)) / ln(2)^2
 *
 * Optimal number of hash functions is:
 *     hashes = bpe * ln(2)
 */
static int _bloomfilter_init(struct bloomfilter * bloomfilter, long entries, double error);

static int _bloomfilter_check(struct bloomfilter * bloomfilter, const void * buffer, int len);

static int _bloomfilter_add(struct bloomfilter * bloomfilter, const void * buffer, int len);

static void _bloomfilter_stats(struct bloomfilter * bloomfilter);

static void _bloomfilter_free(struct bloomfilter * bloomfilter);


#endif	/* PHP_BLOOMFILTER_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
