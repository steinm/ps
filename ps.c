/*
   +----------------------------------------------------------------------+
   | PHP version 4.0                                                      |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997-2001 The PHP Group                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 2.02 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available at through the world-wide-web at                           |
   | http://www.php.net/license/2_02.txt.                                 |
   | If you did not receive a copy of the PHP license and are unable to   |
   | obtain it through the world-wide-web, please send a note to          |
   | license@php.net so we can mail you a copy immediately.               |
   +----------------------------------------------------------------------+
   | Authors: Uwe Steinmann <steinm@debian.org>                           |
   +----------------------------------------------------------------------+
*/

/* $Id: */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_globals.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"
#include "ext/standard/file.h"

#ifdef _HAVE_LIBGD13
/*
#include "ext/gd/php_gd.h"
#if HAVE_GD_BUNDLED
#include "ext/gd/libgd/gd.h"
#else
#include "gd.h"
#endif
*/
#include "gd.h"
static int le_gd;
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif
#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#if HAVE_PS

#include "php_ps.h"

static int le_psdoc;

/* {{{ ps_functions[]
 */
function_entry ps_functions[] = {
	PHP_FE(ps_new, NULL)
	PHP_FE(ps_delete, NULL)
	PHP_FE(ps_open_file, NULL)
//	PHP_FE(ps_get_buffer, NULL)
	PHP_FE(ps_close, NULL)
	PHP_FE(ps_begin_page, NULL)
	PHP_FE(ps_end_page, NULL)
	PHP_FE(ps_get_value, NULL)
	PHP_FE(ps_set_value, NULL)
	PHP_FE(ps_get_parameter, NULL)
	PHP_FE(ps_set_parameter, NULL)
	PHP_FE(ps_findfont, NULL)
	PHP_FE(ps_setfont, NULL)
	PHP_FE(ps_show, NULL)
	PHP_FE(ps_show_xy, NULL)
	PHP_FE(ps_show2, NULL)
	PHP_FE(ps_show_xy2, NULL)
	PHP_FE(ps_continue_text, NULL)
	PHP_FE(ps_show_boxed, NULL)
	PHP_FE(ps_stringwidth, NULL)
	PHP_FE(ps_string_geometry, NULL) /* not available in pdf extension */
	PHP_FE(ps_set_text_pos, NULL)
	PHP_FE(ps_setdash, NULL)
	PHP_FE(ps_setpolydash, NULL)
	PHP_FE(ps_setflat, NULL)
	PHP_FE(ps_setlinejoin, NULL)
	PHP_FE(ps_setlinecap, NULL)
	PHP_FE(ps_setmiterlimit, NULL)
	PHP_FE(ps_setlinewidth, NULL)
	PHP_FE(ps_save, NULL)
	PHP_FE(ps_restore, NULL)
	PHP_FE(ps_translate, NULL)
	PHP_FE(ps_scale, NULL)
	PHP_FE(ps_rotate, NULL)
//	PHP_FE(ps_skew, NULL)
//	PHP_FE(ps_concat, NULL)	/* new function */
	PHP_FE(ps_moveto, NULL)
	PHP_FE(ps_lineto, NULL)
	PHP_FE(ps_curveto, NULL)
	PHP_FE(ps_circle, NULL)
	PHP_FE(ps_arc, NULL)
	PHP_FE(ps_rect, NULL)
	PHP_FE(ps_closepath, NULL)
	PHP_FE(ps_stroke, NULL)
	PHP_FE(ps_closepath_stroke, NULL)
	PHP_FE(ps_fill, NULL)
	PHP_FE(ps_fill_stroke, NULL)
//	PHP_FE(ps_closepath_fill_stroke, NULL)
	PHP_FE(ps_clip, NULL)
//	PHP_FE(ps_endpath, NULL)
	PHP_FE(ps_open_image_file, NULL) 
//	PHP_FE(ps_open_ccitt, NULL)
	PHP_FE(ps_open_image, NULL)
	PHP_FE(ps_close_image, NULL)
	PHP_FE(ps_place_image, NULL)
	PHP_FE(ps_add_bookmark, NULL)
	PHP_FE(ps_set_info, NULL)
//	PHP_FE(ps_attach_file, NULL)
	PHP_FE(ps_add_note, NULL)
	PHP_FE(ps_add_pdflink, NULL)
	PHP_FE(ps_add_locallink, NULL)
	PHP_FE(ps_add_launchlink, NULL)
	PHP_FE(ps_add_weblink, NULL)
	PHP_FE(ps_set_border_style, NULL)
	PHP_FE(ps_set_border_color, NULL)
	PHP_FE(ps_set_border_dash, NULL)
	PHP_FE(ps_setcolor, NULL)
	PHP_FE(ps_begin_pattern, NULL)
	PHP_FE(ps_end_pattern, NULL)
	PHP_FE(ps_begin_template, NULL)
	PHP_FE(ps_end_template, NULL)

#ifdef _HAVE_LIBGD13
	PHP_FE(ps_open_memory_image, NULL)
#endif
	/* Function without an equivalent in pdflib */
	PHP_FE(ps_hyphenate, NULL)
	PHP_FE(ps_symbol, NULL)
	PHP_FE(ps_symbol_name, NULL)
	PHP_FE(ps_symbol_width, NULL)

	{NULL, NULL, NULL}
};
/* }}} */

/* {{{ ps_module_entry
 */
zend_module_entry ps_module_entry = {
	STANDARD_MODULE_HEADER,
	"ps", 
	ps_functions, 
	PHP_MINIT(ps), 
	PHP_MSHUTDOWN(ps), 
	NULL, 
	NULL, 
	PHP_MINFO(ps), 
	NO_VERSION_YET,
	STANDARD_MODULE_PROPERTIES 
};
/* }}} */

#ifdef COMPILE_DL_PS
ZEND_GET_MODULE(ps)
#endif

/* {{{ _free_ps_doc
 */
static void _free_ps_doc(zend_rsrc_list_entry *rsrc)
{
	PSDoc *psdoc = (PSDoc *)rsrc->ptr;
	PS_delete(psdoc);
}
/* }}} */

/* {{{ custom_errorhandler
 */
static void custom_errorhandler(PSDoc *p, int type, const char *shortmsg, void *data)
{
	switch (type){
		case PS_Warning:
				php_error_docref(NULL TSRMLS_CC, E_WARNING,"PSlib warning: %s", shortmsg);
			break;
#ifdef notimplemnetedyet
		case PS_NonfatalError:
			/*
			 * PSlib warings should be visible to the user
			 * If he decides to live with PSlib warnings
			 * he may user the PSlib function
			 * ps_set_parameter($p, "warning" 0) to switch off
			 * the warnings inside PSlib
			 */
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"Internal PSlib warning: %s", shortmsg);
			return;
		case PS_MemoryError: /* give up in all other cases */
		case PS_IOError:
		case PS_RuntimeError:
		case PS_IndexError:
		case PS_TypeError:
		case PS_DivisionByZero:
		case PS_OverflowError:
		case PS_SyntaxError:
		case PS_ValueError:
		case PS_SystemError:
		case PS_UnknownError:
#endif
		default:
			php_error_docref(NULL TSRMLS_CC, E_ERROR,"PSlib error: %s", shortmsg);
		}
}
/* }}} */

/* {{{ ps_emalloc
 */
static void *ps_emalloc(PSDoc *p, size_t size, const char *caller)
{
	return(emalloc(size));
}
/* }}} */

/* {{{ ps_realloc
 */
static void *ps_erealloc(PSDoc *p, void *mem, size_t size, const char *caller)
{
	return(erealloc(mem, size));
}
/* }}} */

/* {{{ ps_efree
 */
static void ps_efree(PSDoc *p, void *mem)
{
	efree(mem);
	mem = NULL;
}
/* }}} */

/* {{{ ps_flushwrite
 */
static size_t ps_flushwrite(PSDoc *p, void *data, size_t size)
{
	return(php_write(data, size));
	return 0;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(ps)
{
	char tmp[32];

	snprintf(tmp, 31, "%d.%d.%d", PS_get_majorversion(), PS_get_minorversion(), PS_get_subminorversion() );
	tmp[31]=0;

	php_info_print_table_start();
	php_info_print_table_row(2, "PS Support", "enabled" );
	php_info_print_table_row(2, "PSlib Version", tmp );
	php_info_print_table_row(2, "Revision", "$Revision$" );
	php_info_print_table_end();

}
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(ps)
{
	le_psdoc = zend_register_list_destructors_ex(_free_ps_doc, NULL, "ps document", module_number);

	REGISTER_LONG_CONSTANT("PS_LINECAP_BUTT", PS_LINECAP_BUTT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINECAP_ROUND", PS_LINECAP_ROUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINECAP_SQUARED", PS_LINECAP_SQUARED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_MITER", PS_LINEJOIN_MITER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_ROUND", PS_LINEJOIN_ROUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_BEVEL", PS_LINEJOIN_BEVEL, CONST_CS | CONST_PERSISTENT);

	PS_boot();
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(ps)
{
	PS_shutdown();
	return SUCCESS;
}
/* }}} */

/* {{{ proto bool ps_set_info(int psdoc, string fieldname, string value)
   Fills an info field of the document */
PHP_FUNCTION(ps_set_info) 
{
	zval *zps;
	char *name, *value;
	long name_len, value_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zps, &name, &name_len, &value, &value_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_info(ps, name, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_close(int psdoc)
   Closes the ps document */
PHP_FUNCTION(ps_close) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_close(ps);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto void ps_begin_page(int psdoc, double width, double height)
   Starts page */
PHP_FUNCTION(ps_begin_page) 
{
	zval *zps;
	double width, height;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &width, &height)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_begin_page(ps, (float) width, (float) height);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_page(int psdoc)
   Ends page */
PHP_FUNCTION(ps_end_page) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_end_page(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_parameter(int psdoc, string key, string value)
   Sets arbitrary parameters */
PHP_FUNCTION(ps_set_parameter)
{
	zval *zps;
	char *name, *value;
	long name_len, value_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zps, &name, &name_len, &value, &value_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_parameter(ps, name, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ps_get_parameter(int psdoc, string key[, mixed modifier])
   Gets arbitrary parameters */
PHP_FUNCTION(ps_get_parameter)
{
	zval *zps, *zmod;
	char *name;
	int name_len;
	PSDoc *ps;
	char *value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zps, &name, &name_len, &zmod)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	if(zmod) {
		value = (char *) PS_get_parameter(ps, name, (float) Z_DVAL_P(zmod));
	} else {
		value = (char *) PS_get_parameter(ps, name, 0.0);
	}

	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto void ps_set_value(int psdoc, string key, float value)
   Sets arbitrary value */
PHP_FUNCTION(ps_set_value)
{
	zval *zps;
	char *name;
	int name_len;
	double value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsd", &zps, &name, &name_len, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_value(ps, name, (float) value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto float ps_get_value(int psdoc, string key[, mixed modifier])
   Gets arbitrary value */
PHP_FUNCTION(ps_get_value)
{
	zval *zps, *zmod;
	char *name;
	int name_len;
	PSDoc *ps;
	double value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zps, &name, &name_len, &zmod)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	if(zmod) {
		value = (double) PS_get_value(ps, name, (float) Z_DVAL_P(zmod));
	} else {
		value = (double) PS_get_value(ps, name, 0.0);
	}

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ proto void ps_show(int psdoc, string text)
   Output text at current position */
PHP_FUNCTION(ps_show) 
{
	zval *zps;
	char *text;
	int text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_show(ps, text);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show2(int psdoc, string text, int len)
   Output text at current position */
PHP_FUNCTION(ps_show2) 
{
	zval *zps;
	char *text;
	int text_len, len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &zps, &text, &text_len, &len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_show2(ps, text, len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy(int psdoc, string text, double x-koor, double y-koor)
   Output text at position */
PHP_FUNCTION(ps_show_xy) 
{
	zval *zps;
	char *text;
	int text_len;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdd", &zps, &text, &text_len, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_show_xy(ps, text, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy2(int psdoc, string text, int len, double x-koor, double y-koor)
   Output text at position */
PHP_FUNCTION(ps_show_xy2) 
{
	zval *zps;
	char *text;
	int text_len, len;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsldd", &zps, &text, &text_len, &len, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_show_xy2(ps, text, len, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_findfont(int psdoc, string fontname, string encoding [, bool embed])
   Prepares the font fontname for later use with ps_setfont() */
PHP_FUNCTION(ps_findfont) {
	zval *zps;
	zend_bool embed;
	char *fontname, *encoding;
	int fontname_len, encoding_len;
	PSDoc *ps;
	int font;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|b", &zps, &fontname, &fontname_len, &encoding, &encoding_len, &embed)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	font = PS_findfont(ps, fontname, encoding, embed);

	if (font == 0) {
		/* pslib will do this for you, will throw some exception
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Font %s not found", fontname);
		*/
		RETURN_FALSE;
	}

	RETURN_LONG(font);
}
/* }}} */

/* {{{ proto void ps_setfont(int psdoc, int font, float fontsize)
   Sets the current font in the fiven fontsize */
PHP_FUNCTION(ps_setfont) {
	zval *zps;
	double fontsize;
	int font;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rld", &zps, &font, &fontsize)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setfont(ps, font, (float) fontsize);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_rotate(int psdoc, double angle)
   Sets rotation */
PHP_FUNCTION(ps_rotate)
{
	zval *zps;
	double angle;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rd", &zps, &angle)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_rotate(ps, (float) angle);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_rect(int psdoc, double x, double y, double width, double height)
   Draws a rectangle */
PHP_FUNCTION(ps_rect)
{
	zval *zps;
	double x, y, width, height;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddd", &zps, &x, &y, &width, &height)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_rect(ps, (float) x, (float) y, (float) width, (float) height);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinewidth(int psdoc, double width)
   Sets line width */
PHP_FUNCTION(ps_setlinewidth)
{
	zval *zps;
	double width;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rd", &zps, &width)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setlinewidth(ps, (float) width);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setdash(int psdoc, float black, float white)
   Sets dash pattern */
PHP_FUNCTION(ps_setdash)
{
	zval *zps;
	double black, white;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &black, &white)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setdash(ps, (float) black, (float) white);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_stroke(int psdoc)
   Draw line along path path */
PHP_FUNCTION(ps_stroke)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_stroke(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_fill(int psdoc)
   Fill current path */
PHP_FUNCTION(ps_fill) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_fill(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_fill_stroke(int psdoc)
   Fill and stroke current path */
PHP_FUNCTION(ps_fill_stroke)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_fill_stroke(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_save(int psdoc)
   Saves current enviroment */
PHP_FUNCTION(ps_save)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_save(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_restore(int psdoc)
   Restores formerly saved enviroment */
PHP_FUNCTION(ps_restore)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_restore(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_lineto(int psdoc, double x, double y)
   Draws a line */
PHP_FUNCTION(ps_lineto)
{
	zval *zps;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_lineto(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_moveto(int psdoc, double x, double y)
   Sets current point */
PHP_FUNCTION(ps_moveto)
{
	zval *zps;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_moveto(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath(int psdoc)
   Close path */
PHP_FUNCTION(ps_closepath)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_closepath(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath_stroke(int psdoc)
   Close path and draw line along path */
PHP_FUNCTION(ps_closepath_stroke)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_closepath_stroke(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_translate(int psdoc, double x, double y)
   Sets origin of coordinate system */
PHP_FUNCTION(ps_translate) 
{
	zval *zps;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_translate(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_scale(int psdoc, double x-scale, double y-scale)
   Sets scaling */
PHP_FUNCTION(ps_scale)
{
	zval *zps;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);


	PS_scale(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinejoin(int psdoc, int value)
   Sets linejoin parameter */
PHP_FUNCTION(ps_setlinejoin) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	/* pslib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 2) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Parameter of ps_setlinejoin() must be between 0 and 2");
		RETURN_FALSE;
	}
	*/

	PS_setlinejoin(ps, Z_LVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinecap(int psdoc, int value)
   Sets linecap parameter */
PHP_FUNCTION(ps_setlinecap) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	/* pslib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 2) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Parameter of ps_setlinecap() must be > 0 and <= 2");
		RETURN_FALSE;
	}
	*/

	PS_setlinecap(ps, Z_LVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setmiterlimit(int psdoc, double value)
   Sets miter limit */
PHP_FUNCTION(ps_setmiterlimit)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	/* pslib will do this for you, will throw some exception
	if(Z_DVAL_PP(arg2) < 1) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Parameter of ps_setmiterlimit() must be >= 1");
		RETURN_FALSE;
	}
	*/

	PS_setmiterlimit(ps, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_curveto(int psdoc, double x1, double y1, double x2, double y2, double x3, double y3)
   Draws a curve */
PHP_FUNCTION(ps_curveto)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PS_curveto(ps, (float) Z_DVAL_PP(arg2),
					 (float) Z_DVAL_PP(arg3),
					 (float) Z_DVAL_PP(arg4),
					 (float) Z_DVAL_PP(arg5),
					 (float) Z_DVAL_PP(arg6),
					 (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_show_boxed(int psdoc, string text, double x-koor, double y-koor, double width, double height, string mode [, string feature])
   Output text formated in a boxed */
PHP_FUNCTION(ps_show_boxed) 
{
	zval **argv[8];
	int argc = ZEND_NUM_ARGS();
	int nr;
	char *feature;
	PSDoc *ps;

	if (((argc < 7) || (argc > 8)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, argv[0], -1, "ps document", le_psdoc);

	convert_to_string_ex(argv[1]);
	convert_to_double_ex(argv[2]);
	convert_to_double_ex(argv[3]);
	convert_to_double_ex(argv[4]);
	convert_to_double_ex(argv[5]);
	convert_to_string_ex(argv[6]);

	if(argc == 8) {
		convert_to_string_ex(argv[7]);
		feature = Z_STRVAL_PP(argv[7]);
	} else {
		feature = NULL;
	}

	nr = PS_show_boxed(ps, Z_STRVAL_PP(argv[1]),
							(float) Z_DVAL_PP(argv[2]),
							(float) Z_DVAL_PP(argv[3]),
							(float) Z_DVAL_PP(argv[4]),
							(float) Z_DVAL_PP(argv[5]),
							Z_STRVAL_PP(argv[6]),
							feature);

	RETURN_LONG(nr);
}
/* }}} */

/* {{{ proto void ps_set_text_pos(int psdoc, double x, double y)
   Sets the position of text for the next ps_show call */
PHP_FUNCTION(ps_set_text_pos) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_set_text_pos(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_continue_text(int psdoc, string text)
   Output text in next line */
PHP_FUNCTION(ps_continue_text)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	PS_continue_text2(ps, Z_STRVAL_PP(arg2), Z_STRLEN_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double ps_stringwidth(int psdoc, string text [, int font, double size])
   Returns width of text in current font */
PHP_FUNCTION(ps_stringwidth)
{
	zval **arg1, **arg2, **arg3, **arg4;
	double width, size;
	PSDoc *ps;
	int font;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	if (ZEND_NUM_ARGS() == 2) {
			font = 0;
			size = 0;
	} else {
		convert_to_long_ex(arg3);
		font = Z_LVAL_PP(arg3);
		convert_to_double_ex(arg4);
		size = Z_DVAL_PP(arg4);
	}
	width = (double) PS_stringwidth2(ps,
		Z_STRVAL_PP(arg2),
		Z_STRLEN_PP(arg2),
		font,
		(float)size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto double ps_string_geometry(int psdoc, string text [, int font, double size])
   Returns geometry of text in current font */
PHP_FUNCTION(ps_string_geometry)
{
	zval **arg1, **arg2, **arg3, **arg4;
	double width, size;
	float dimension[3];
	PSDoc *ps;
	int font;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	if (ZEND_NUM_ARGS() == 2) {
			font = 0;
			size = 0;
	} else {
		convert_to_long_ex(arg3);
		font = Z_LVAL_PP(arg3);
		convert_to_double_ex(arg4);
		size = Z_DVAL_PP(arg4);
	}
	width = (double) PS_string_geometry(ps,
		Z_STRVAL_PP(arg2),
		Z_STRLEN_PP(arg2),
		font,
		(float)size,
		dimension);

	array_init(return_value);
	add_assoc_double(return_value, "width", (double) dimension[0]);
	add_assoc_double(return_value, "descender", (double) dimension[1]);
	add_assoc_double(return_value, "ascender", (double) dimension[2]);
}
/* }}} */

/* {{{ proto void ps_setflat(int psdoc, double value)
   Sets flatness */
PHP_FUNCTION(ps_setflat) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	/* pslib will do this for you, will throw some exception
	if((Z_LVAL_PP(arg2) > 100) && (Z_LVAL_PP(arg2) < 0)) {
		php_error_docref(NULL TSRMLS_CC, E_WARNING,"Parameter of ps_setflat() has to between 0 and 100");
		RETURN_FALSE;
	}
	*/

	PS_setflat(ps, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_circle(int psdoc, double x, double y, double radius)
   Draws a circle */
PHP_FUNCTION(ps_circle)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PS_circle(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_arc(int psdoc, double x, double y, double radius, double start, double end)
   Draws an arc */
PHP_FUNCTION(ps_arc)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);

	PS_arc(ps, (float) Z_DVAL_PP(arg2),
				 (float) Z_DVAL_PP(arg3),
				 (float) Z_DVAL_PP(arg4),
				 (float) Z_DVAL_PP(arg5),
				 (float) Z_DVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_skew(int psdoc, double xangle, double yangle)
   Skew the coordinate system */
PHP_FUNCTION(ps_skew)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_skew(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath_fill_stroke(int psdoc)
   Close, fill and stroke current path */
PHP_FUNCTION(ps_closepath_fill_stroke)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_closepath_fill_stroke(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_endpath(int psdoc)
   Ends current path */
PHP_FUNCTION(ps_endpath) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_endpath(ps);
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_clip(int psdoc)
   Clips to current path */
PHP_FUNCTION(ps_clip)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_clip(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_add_bookmark(int psdoc, string text [, int parent, int open])
   Adds bookmark for current page */
PHP_FUNCTION(ps_add_bookmark)
{
	zval **arg1, **arg2, **arg3, **arg4;
	int parentid, open, id;
	PSDoc *ps;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
	break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);

	if (ZEND_NUM_ARGS() > 2) {
		convert_to_long_ex(arg3);
		parentid = Z_LVAL_PP(arg3);

		if (ZEND_NUM_ARGS() > 3) {
			convert_to_long_ex(arg4);
			open = Z_LVAL_PP(arg4);
		} else {
			open = 0;
		}
	} else {
		parentid = 0;
		open = 0;
	}

	/* will never return 0 */
	id = PS_add_bookmark(ps, Z_STRVAL_PP(arg2), parentid, open);

	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int ps_open_image_file(int ps, string type, string file, string stringparam, int intparam)
   Opens an image file of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image_file)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PSDoc *ps;
	int imageid;
	int argc;
	char *image;

	switch ((argc = ZEND_NUM_ARGS())) {
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 5:
		if (zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg3), &image);
#else
	image = Z_STRVAL_PP(arg3);
#endif

	if (argc == 3) {
		imageid = PS_open_image_file(ps, Z_STRVAL_PP(arg2), image, "", 0);
	} else {
		convert_to_string_ex(arg4);
		convert_to_long_ex(arg5);
		imageid = PS_open_image_file(ps, Z_STRVAL_PP(arg2), image, Z_STRVAL_PP(arg4), Z_LVAL_PP(arg5));
	}

	if (imageid == 0) {
		/* pslib will do this for you, will throw some exception
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open image: %s", image);
		*/
		RETURN_FALSE;
	}
	RETURN_LONG(imageid);
}
/* }}} */

#ifdef _HAVE_LIBGD13
/* {{{ proto int ps_open_memory_image(int ps, int image)
   Takes an GD image and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_memory_image)
{
	zval **arg1, **arg2;
	int i, j, color, count;
	int imageid;
	gdImagePtr im;
	unsigned char *buffer, *ptr;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd");
	if(!le_gd) {
		php_error_docref(NULL TSRMLS_CC, E_ERROR, "Unable to find handle for GD image stream. Please check the GD extension is loaded.");
	}
	ZEND_FETCH_RESOURCE(im, gdImagePtr, arg2, -1, "Image", le_gd);

	count = 3 * im->sx * im->sy;
	if(NULL == (buffer = (unsigned char *) emalloc(count))) {
		RETURN_FALSE;
	}

	ptr = buffer;
	for(i=0; i<im->sy; i++) {
		for(j=0; j<im->sx; j++) {
#ifdef HAVE_LIBGD20
			if(gdImageTrueColor(im)) {
				if (im->tpixels && gdImageBoundsSafe(im, j, i)) {
					color = gdImageTrueColorPixel(im, j, i);
					*ptr++ = (color >> 16) & 0xFF;
					*ptr++ = (color >> 8) & 0xFF;
					*ptr++ = color & 0xFF;
				}
			} else {
#endif
				if (im->pixels && gdImageBoundsSafe(im, j, i)) {
					color = im->pixels[i][j];
					*ptr++ = im->red[color];
					*ptr++ = im->green[color];
					*ptr++ = im->blue[color];
				}
#ifdef HAVE_LIBGD20
			}
#endif
		}
	}

	imageid = PS_open_image(ps, "raw", "memory", buffer, im->sx*im->sy*3, im->sx, im->sy, 3, 8, NULL);
	efree(buffer);

	if(imageid == 0) {
		/* pslib will do this for you, will throw some exception
		php_error_docref(NULL TSRMLS_CC, E_WARNING, "Could not open image");
		*/
		RETURN_FALSE;
	}

	RETURN_LONG(imageid);
}
/* }}} */
#endif /* HAVE_LIBGD13 */

/* {{{ proto void ps_close_image(int ps, int psimage)
   Closes the PS image */
PHP_FUNCTION(ps_close_image)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	convert_to_long_ex(arg2);

	PS_close_image(ps, Z_LVAL_PP(arg2));
}
/* }}} */

/* {{{ proto void ps_place_image(int ps, int psimage, double x, double y, double scale)
   Places image in the PS document */
PHP_FUNCTION(ps_place_image)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);

	PS_place_image(ps, Z_LVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), (float) Z_DVAL_PP(arg5));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_weblink(int psdoc, double llx, double lly, double urx, double ury, string url)
   Adds link to web resource */
PHP_FUNCTION(ps_add_weblink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	PS_add_weblink(ps, (float) Z_DVAL_PP(arg2), 
						 (float) Z_DVAL_PP(arg3), 
						 (float) Z_DVAL_PP(arg4), 
						 (float) Z_DVAL_PP(arg5), 
						 Z_STRVAL_PP(arg6));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_pdflink(int psdoc, double llx, double lly, double urx, double ury, string filename, int page, string dest)
   Adds link to PS document */
PHP_FUNCTION(ps_add_pdflink)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 8 || zend_get_parameters_ex(8, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_string_ex(arg8);
	PS_add_pdflink(ps, (float) Z_DVAL_PP(arg2), 
						 (float) Z_DVAL_PP(arg3), 
						 (float) Z_DVAL_PP(arg4), 
						 (float) Z_DVAL_PP(arg5),
						 Z_STRVAL_PP(arg6), 
						 Z_LVAL_PP(arg7),
						 Z_STRVAL_PP(arg8));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_style(int psdoc, string style, double width)
   Sets style of box surounding all kinds of annotations and link */
PHP_FUNCTION(ps_set_border_style)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	PS_set_border_style(ps, Z_STRVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_color(int psdoc, double red, double green, double blue)
   Sets color of box surounded all kinds of annotations and links */
PHP_FUNCTION(ps_set_border_color)
{
	zval **arg1, **arg2, **arg3, **arg4;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PS_set_border_color(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_dash(int psdoc, double black, double white)
   Sets the border dash style of all kinds of annotations and links */
PHP_FUNCTION(ps_set_border_dash)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_set_border_dash(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_annotation(int psdoc, double xll, double yll, double xur, double xur, string title, string text)
   Sets annotation (depreciated use ps_add_note instead) */
PHP_FUNCTION(ps_add_annotation)
{
	zval **argv[7];
	PSDoc *ps;

	if(ZEND_NUM_ARGS() != 7 || zend_get_parameters_array_ex(7, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ps, PSDoc *, argv[0], -1, "ps document", le_psdoc);

	convert_to_double_ex(argv[1]);
	convert_to_double_ex(argv[2]);
	convert_to_double_ex(argv[3]);
	convert_to_double_ex(argv[4]);
	convert_to_string_ex(argv[5]);
	convert_to_string_ex(argv[6]);

	PS_add_note(ps, (float) Z_DVAL_PP(argv[1]),
					  (float) Z_DVAL_PP(argv[2]),
					  (float) Z_DVAL_PP(argv[3]),
					  (float) Z_DVAL_PP(argv[4]),
					  Z_STRVAL_PP(argv[6]),
					  Z_STRVAL_PP(argv[5]),
					  "note", 1);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_new()
   Creates a new PS object */
PHP_FUNCTION(ps_new) {
	PSDoc *ps;

	if(NULL == (ps = PS_new2(custom_errorhandler, ps_emalloc, ps_erealloc, ps_efree, NULL)))
		RETURN_FALSE;
	PS_set_parameter(ps, "imagewarning", "true");
	PS_set_parameter(ps, "binding", "PHP");

	ZEND_REGISTER_RESOURCE(return_value, ps, le_psdoc);
}
/* }}} */

/* {{{ proto void ps_delete(int psdoc)
   Deletes the PS object */
PHP_FUNCTION(ps_delete) {
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	zend_list_delete(Z_LVAL_PP(arg1));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_open_file(int psdoc [, char filename])
   Opens a new PS document. If filename is NULL, document is created in memory. This is not yet fully supported */
PHP_FUNCTION(ps_open_file) {
	zval **arg1, **arg2;
	int ps_file;
	char *filename;
	int argc;
	PSDoc *ps;

	if((argc = ZEND_NUM_ARGS()) > 2)
		WRONG_PARAM_COUNT;

	if (argc == 1) {
		if (zend_get_parameters_ex(1, &arg1) == FAILURE)
			WRONG_PARAM_COUNT;
	} else {
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	if (argc == 2) {
		convert_to_string_ex(arg2);
		filename = Z_STRVAL_PP(arg2);
		ps_file = PS_open_file(ps, filename);
	} else {
		/* open in memory */
		ps_file = PS_open_mem(ps, ps_flushwrite);
	}

	if (ps_file < 0) {
		RETURN_FALSE;
	}

	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto int ps_get_buffer(int psdoc)
   Fetches the full buffer containig the generated PS data */
PHP_FUNCTION(ps_get_buffer) {
	zval **arg1;
	long size;
	PSDoc *ps;
	const char *buffer;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	buffer = PS_get_buffer(ps, &size);

	RETURN_STRINGL((char *)buffer, size, 1);
}

/* }}} */
#endif

/* {{{ proto void ps_setpolydash(int psdoc, double darray)
   Sets more complicated dash pattern */ 
PHP_FUNCTION(ps_setpolydash) {
	zval **arg1, **arg2;
	HashTable *array;
	int len, i;
	float *darray;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_array_ex(arg2);
	array = Z_ARRVAL_PP(arg2);
	len = zend_hash_num_elements(array);

	if (NULL == (darray = emalloc(len * sizeof(double)))) {
		RETURN_FALSE;
	}
	zend_hash_internal_pointer_reset(array);
	for (i=0; i<len; i++) {
		zval *keydata, **keydataptr;

		zend_hash_get_current_data(array, (void **) &keydataptr);
		keydata = *keydataptr;
		if (keydata->type == IS_DOUBLE) {
			darray[i] = (float) keydata->value.dval;
		} else if (keydata->type == IS_LONG) {
			darray[i] = (float) keydata->value.lval;
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"PSlib set_polydash: illegal darray value");
		}
		zend_hash_move_forward(array);
	}

	PS_setpolydash(ps, darray, len);

	efree(darray);
	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_concat(int ps, double a, double b, double c, double d, double e, double f)
   Concatenates a matrix to the current transformation matrix for text and graphics */
PHP_FUNCTION(ps_concat) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PS_concat(ps,
	    (float) Z_DVAL_PP(arg2),
	    (float) Z_DVAL_PP(arg3),
	    (float) Z_DVAL_PP(arg4),
	    (float) Z_DVAL_PP(arg5),
	    (float) Z_DVAL_PP(arg6),
	    (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_open_ccitt(int ps, string filename, int width, int height, int bitreverse, int k, int blackls1)
   Opens an image file with raw CCITT G3 or G4 compresed bitmap data */
PHP_FUNCTION(ps_open_ccitt) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;
	char *image;
	int imageid;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &image);
#else
	image = Z_STRVAL_PP(arg2);
#endif

	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);

	imageid = PS_open_CCITT(ps,
	    image,
	    Z_LVAL_PP(arg3),
	    Z_LVAL_PP(arg4),
	    Z_LVAL_PP(arg5),
	    Z_LVAL_PP(arg6),
	    Z_LVAL_PP(arg7));

	RETURN_LONG(imageid);
}
/* }}} */
#endif

/* {{{ proto int ps_open_image(int ps, string type, string source, string data, long length, int width, int height, int components, int bpc, string params)
   Opens an image of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10;
	PSDoc *ps;
	int imageid;
	char *image;

	if (ZEND_NUM_ARGS() != 10 || zend_get_parameters_ex(10, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_string_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_long_ex(arg7);
	convert_to_long_ex(arg8);
	convert_to_long_ex(arg9);
	convert_to_string_ex(arg10);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg4), &image);
#else
	image = Z_STRVAL_PP(arg4);
#endif

	imageid = PS_open_image(ps,
		Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3),
		image,
		Z_LVAL_PP(arg5),
		Z_LVAL_PP(arg6),
		Z_LVAL_PP(arg7),
		Z_LVAL_PP(arg8),
		Z_LVAL_PP(arg9),
		Z_STRVAL_PP(arg10));

	RETURN_LONG(imageid);
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_attach_file(int ps, double lly, double lly, double urx, double ury, string filename, string description, string author, string mimetype, string icon)
   Adds a file attachment annotation at the rectangle specified by his lower left and upper right corners */
PHP_FUNCTION(ps_attach_file) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 10 || zend_get_parameters_ex(10, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9, &arg10) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	convert_to_long_ex(arg3);
	convert_to_long_ex(arg4);
	convert_to_long_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_string_ex(arg9);
	convert_to_string_ex(arg10);

	PS_attach_file(ps,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_STRVAL_PP(arg6),
		Z_STRVAL_PP(arg7),
		Z_STRVAL_PP(arg8),
		Z_STRVAL_PP(arg9),
		Z_STRVAL_PP(arg10));

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_add_note(int psdoc, double llx, double lly, double urx, double ury, string contents, string title, string icon, int open)
   Sets annotation */
PHP_FUNCTION(ps_add_note) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 9 || zend_get_parameters_ex(9, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7, &arg8, &arg9) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);
	convert_to_string_ex(arg7);
	convert_to_string_ex(arg8);
	convert_to_long_ex(arg9);

	PS_add_note(ps,
		 (float) Z_DVAL_PP(arg2),
		 (float) Z_DVAL_PP(arg3),
		 (float) Z_DVAL_PP(arg4),
		 (float) Z_DVAL_PP(arg5),
		 Z_STRVAL_PP(arg6),
		 Z_STRVAL_PP(arg7),
		 Z_STRVAL_PP(arg8),
		 Z_LVAL_PP(arg9));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_locallink(int psdoc, double llx, double lly, double urx, double ury, int page, string dest)
   Adds link to web resource */
PHP_FUNCTION(ps_add_locallink) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_long_ex(arg6);
	convert_to_string_ex(arg7);

	PS_add_locallink(ps,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_LVAL_PP(arg6),
		Z_STRVAL_PP(arg7));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_launchlink(int psdoc, double llx, double lly, double urx, double ury, string filename)
   Adds link to web resource */
PHP_FUNCTION(ps_add_launchlink) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_string_ex(arg6);

	PS_add_launchlink(ps,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		Z_STRVAL_PP(arg6));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setcolor(int ps, string type, string colorspace, double c1, double c2, double c3, double c4);
 * Set the current color space and color. */
PHP_FUNCTION(ps_setcolor) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;
	double c1;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	if (0 == (strcmp(Z_STRVAL_PP(arg3), "spot"))) {
		c1 = Z_DVAL_PP(arg4);
	} else if(0 == (strcmp(Z_STRVAL_PP(arg3), "pattern"))) {
		c1 = Z_DVAL_PP(arg4);
	} else {
		c1 = Z_DVAL_PP(arg4);
	}

	PS_setcolor(ps,
		Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3),
		(float) c1,
		(float) Z_DVAL_PP(arg5),
		(float) Z_DVAL_PP(arg6),
		(float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
} /* }}} */

#ifdef notimplementedyet
/* {{{ proto int ps_makespotcolor(int ps, string spotname);
 * Make a named spot color from the current color. */
PHP_FUNCTION(ps_makespotcolor) {
	zval **arg1, **arg2;
	PSDoc *ps;
	int spotcolor;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);

	spotcolor = PS_makespotcolor(ps,
		Z_STRVAL_PP(arg2),
		Z_STRLEN_PP(arg2));

	RETURN_LONG(spotcolor+PSLIB_SPOT_OFFSET);
} /* }}} */
#endif

/* {{{ proto void ps_arcn(int ps, double x, double y, double r, double alpha, double beta);
 * Draw a clockwise circular arc from alpha to beta degrees. */
PHP_FUNCTION(ps_arcn) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);

	PS_arcn(ps,
		(float) Z_DVAL_PP(arg2),
		(float) Z_DVAL_PP(arg3),
		(float) Z_DVAL_PP(arg4),
		(float) Z_DVAL_PP(arg5),
		(float) Z_DVAL_PP(arg6));

	RETURN_TRUE;
} /* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_initgraphics(int ps);
 * Reset all implicit color and graphics state parameters to their defaults. */
PHP_FUNCTION(ps_initgraphics) {
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_initgraphics(ps);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto void ps_add_thumbnail(int ps, int image);
 * Add an existing image as thumbnail for the current page. */
PHP_FUNCTION(ps_add_thumbnail) {
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);

	PS_add_thumbnail(ps, Z_LVAL_PP(arg2));

	RETURN_TRUE;
} /* }}} */

/* {{{ proto void ps_setmatrix(int ps, double a, double b, double c, double d, double e, double f)
   Explicitly set the current transformation matrix. */
PHP_FUNCTION(ps_setmatrix) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 7 || zend_get_parameters_ex(7, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6, &arg7) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_double_ex(arg6);
	convert_to_double_ex(arg7);

	PS_setmatrix(ps,
	    (float) Z_DVAL_PP(arg2),
	    (float) Z_DVAL_PP(arg3),
	    (float) Z_DVAL_PP(arg4),
	    (float) Z_DVAL_PP(arg5),
	    (float) Z_DVAL_PP(arg6),
	    (float) Z_DVAL_PP(arg7));

	RETURN_TRUE;
} /* }}} */
#endif

/* {{{ proto void ps_begin_pattern(int psdoc, double width, double height, double xstep, double ystep, int painttype)
   Starts pattern */
PHP_FUNCTION(ps_begin_pattern) 
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 6 || zend_get_parameters_ex(6, &arg1, &arg2, &arg3, &arg4, &arg5, &arg6) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	convert_to_long_ex(arg6);
	PS_begin_pattern(ps, (float) Z_DVAL_PP(arg2),
	                     (float) Z_DVAL_PP(arg3),
	                     (float) Z_DVAL_PP(arg4),
	                     (float) Z_DVAL_PP(arg5),
	                     (int) Z_LVAL_PP(arg6));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_pattern(int psdoc)
   Ends pattern */
PHP_FUNCTION(ps_end_pattern) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_end_pattern(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_begin_template(int psdoc, double width, double height)
   Starts template */
PHP_FUNCTION(ps_begin_template) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_begin_template(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_template(int psdoc)
   Ends template */
PHP_FUNCTION(ps_end_template) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_end_template(ps);
	RETURN_TRUE;
}
/* }}} */

/* Function without an equivalent in pdflib */

/* {{{ proto void ps_hyphenate(int ps, string word)
   Hyphenate a given word */
PHP_FUNCTION(ps_hyphenate) {
	zval **arg1, **arg2;
	PSDoc *ps;
	char *buffer;
	int i, j;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);

	if(NULL == (buffer = emalloc(strlen(Z_STRVAL_PP(arg2))+3))) {
		RETURN_FALSE;
	}
	if(0 > PS_hyphenate(ps, Z_STRVAL_PP(arg2), &buffer)) {
		efree(buffer);
		RETURN_FALSE;
	}

	array_init(return_value);
	j = 0;
	for(i=0; i<strlen(buffer); i++) {
		if(buffer[i] & 1) {
			add_index_long(return_value, j, i);
			j++;
		}
	}
	efree(buffer);
} /* }}} */

/* {{{ proto void ps_symbol(int ps, int ord)
   Output single char by its value in the font encoding */
PHP_FUNCTION(ps_symbol) {
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);

	PS_symbol(ps, (unsigned char) Z_LVAL_PP(arg2));

	RETURN_TRUE;

} /* }}} */

/* {{{ proto double ps_symbol_width(int psdoc, int ord [, int font, double size])
   Returns width of a glyph in current font */
PHP_FUNCTION(ps_symbol_width)
{
	zval **arg1, **arg2, **arg3, **arg4;
	double width, size;
	PSDoc *ps;
	int font;

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	if (ZEND_NUM_ARGS() == 2) {
		font = 0;
		size = 0;
	} else {
		convert_to_long_ex(arg3);
		font = Z_LVAL_PP(arg3);
		convert_to_double_ex(arg4);
		size = Z_DVAL_PP(arg4);
	}
	width = (double) PS_symbol_width(ps,
		(unsigned char) Z_LVAL_PP(arg2),
		font,
		(float)size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto string ps_symbol_name(int psdoc, int ord [, int font])
   Returns name of a glyph in current font */
PHP_FUNCTION(ps_symbol_name)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;
	int font;
	char glyphname[50];

	switch (ZEND_NUM_ARGS()) {
	case 2:
		if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
			WRONG_PARAM_COUNT;
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);
	if (ZEND_NUM_ARGS() == 2) {
			font = 0;
	} else {
		convert_to_long_ex(arg3);
		font = Z_LVAL_PP(arg3);
	}
	PS_symbol_name(ps,
		(unsigned char) Z_LVAL_PP(arg2),
		font, glyphname, 50);

	RETURN_STRING(glyphname, 1);
}
/* }}} */

#endif /* 0 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
