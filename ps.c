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
	PHP_FE(ps_setoverprint, NULL)
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
	PHP_FE(ps_makespotcolor, NULL)
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

	/* FIXME: Do we need convert_to_double_ex(zmod)? */
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
	zend_bool embed = 0;
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

/* {{{ proto void ps_setoverprint(int psdoc, long mode)
   Sets overprint mode */
PHP_FUNCTION(ps_setoverprint)
{
	zval *zps;
	int mode;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &mode)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setoverprint(ps, mode);

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
	zval *zps;
	int value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setlinejoin(ps, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinecap(int psdoc, int value)
   Sets linecap parameter */
PHP_FUNCTION(ps_setlinecap) 
{
	zval *zps;
	int value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setlinecap(ps, value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setmiterlimit(int psdoc, double value)
   Sets miter limit */
PHP_FUNCTION(ps_setmiterlimit)
{
	zval *zps;
	double value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rd", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setmiterlimit(ps, (float) value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_curveto(int psdoc, double x1, double y1, double x2, double y2, double x3, double y3)
   Draws a curve */
PHP_FUNCTION(ps_curveto)
{
	zval *zps;
	double x1, y1, x2, y2, x3, y3;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddddd", &zps, &x1, &y1, &x2, &y2, &x3, &y3)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_curveto(ps, (float) x1, (float) y1, (float) x2, (float) y2, (float) x3, (float) y3);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_show_boxed(int psdoc, string text, double x-koor, double y-koor, double width, double height, string mode [, string feature])
   Output text formated in a boxed */
PHP_FUNCTION(ps_show_boxed) 
{
	zval *zps;
	double x, y, width, height;
	char *text, *feature, *mode;
	int text_len, feature_len, mode_len;
	int nr;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdddds|s", &zps, &text, &text_len, &x, &y, &width, &height, &mode, &mode_len, &feature, &feature_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	nr = PS_show_boxed(ps, text, (float) x, (float) y, (float) width, (float) height, mode, feature);

	RETURN_LONG(nr);
}
/* }}} */

/* {{{ proto void ps_set_text_pos(int psdoc, double x, double y)
   Sets the position of text for the next ps_show call */
PHP_FUNCTION(ps_set_text_pos) 
{
	zval *zps;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_text_pos(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_continue_text(int psdoc, string text)
   Output text in next line */
PHP_FUNCTION(ps_continue_text)
{
	zval *zps;
	char *text;
	int text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_continue_text2(ps, text, text_len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto double ps_stringwidth(int psdoc, string text [, int font, double size])
   Returns width of text in current font */
PHP_FUNCTION(ps_stringwidth)
{
	zval *zps;
	char *text;
	int text_len;
	double width, size = 0.0;
	int font = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ld", &zps, &text, &text_len, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	width = (double) PS_stringwidth2(ps, text, text_len, font, (float)size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto array ps_string_geometry(int psdoc, string text [, int font, double size])
   Returns geometry of text in current font */
PHP_FUNCTION(ps_string_geometry)
{
	zval *zps;
	char *text;
	int text_len;
	double width, size = 0.0;
	int font = 0;
	float dimension[3];
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ld", &zps, &text, &text_len, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	width = (double) PS_string_geometry(ps, text, text_len, font, (float) size, dimension);

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
	zval *zps;
	double value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rd", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setflat(ps, (float) value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_circle(int psdoc, double x, double y, double radius)
   Draws a circle */
PHP_FUNCTION(ps_circle)
{
	zval *zps;
	double x, y, radius;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddd", &zps, &x, &y, &radius)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_circle(ps, (float) x, (float) y, (float) radius);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_arc(int psdoc, double x, double y, double radius, double start, double end)
   Draws an arc */
PHP_FUNCTION(ps_arc)
{
	zval *zps;
	double x, y, radius, start, end;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddd", &zps, &x, &y, &radius, &start, &end)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_arc(ps, (float) x, (float) y, (float) radius, (float) start, (float) end);

	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_skew(int psdoc, double xangle, double yangle)
   Skew the coordinate system */
PHP_FUNCTION(ps_skew)
{
	zval *zps;
	double xangle, yangle;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &xangle, &yangle)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_skew(ps, (float) xangle, (float) yangle);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath_fill_stroke(int psdoc)
   Close, fill and stroke current path */
PHP_FUNCTION(ps_closepath_fill_stroke)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_closepath_fill_stroke(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_endpath(int psdoc)
   Ends current path */
PHP_FUNCTION(ps_endpath) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_endpath(ps);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_clip(int psdoc)
   Clips to current path */
PHP_FUNCTION(ps_clip)
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_clip(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_add_bookmark(int psdoc, string text [, int parent, int open])
   Adds bookmark for current page */
PHP_FUNCTION(ps_add_bookmark)
{
	zval *zps;
	char *text;
	int text_len;
	int parentid = 0, open = 0, id;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ll", &zps, &text, &text_len, &parentid, &open)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	/* will never return 0 */
	id = PS_add_bookmark(ps, text, parentid, open);

	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int ps_open_image_file(int ps, string type, string file[, string stringparam, int intparam])
   Opens an image file of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image_file)
{
	zval *zps;
	char *type, *filename, *image, *stringparam = NULL;
	int type_len, filename_len, stringparam_len;
	int imageid, intparam = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|sl", &zps, &type, &type_len, &filename, &filename_len, &stringparam, &stringparam_len, &intparam)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

#ifdef VIRTUAL_DIR
	virtual_filepath(filename, &image);
#else
	image = filename;
#endif

	imageid = PS_open_image_file(ps, type, image, stringparam, intparam);

	if (imageid == 0) {
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
	zval *zps;
	int imageid;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &imageid)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_close_image(ps, imageid);
}
/* }}} */

/* {{{ proto void ps_place_image(int ps, int psimage, double x, double y, double scale)
   Places image in the PS document */
PHP_FUNCTION(ps_place_image)
{
	zval *zps;
	int imageid;
	double x, y, scale;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlddd", &zps, &imageid, &x, &y, &scale)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_place_image(ps, imageid, (float) x, (float) y, (float) scale);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_weblink(int psdoc, double llx, double lly, double urx, double ury, string url)
   Adds link to web resource */
PHP_FUNCTION(ps_add_weblink)
{
	zval *zps;
	double llx, lly, urx, ury;
	char *url;
	int url_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddds", &zps, &llx, &lly, &urx, &ury, &url, &url_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_weblink(ps, (float) llx, (float) lly, (float) urx, (float) ury, url);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_pdflink(int psdoc, double llx, double lly, double urx, double ury, string filename, int page, string dest)
   Adds link to PS document */
PHP_FUNCTION(ps_add_pdflink)
{
	zval *zps;
	double llx, lly, urx, ury;
	char *filename, *dest;
	int filename_len, dest_len;
	int page;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsls", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len, &page, &dest, &dest_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_pdflink(ps, llx, (float) lly, (float) urx, (float) ury, filename, page, dest);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_style(int psdoc, string style, double width)
   Sets style of box surounding all kinds of annotations and link */
PHP_FUNCTION(ps_set_border_style)
{
	zval *zps;
	char *style;
	int style_len;
	double width;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsd", &zps, &style, &style_len, &width)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_border_style(ps, style, (float) width);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_color(int psdoc, double red, double green, double blue)
   Sets color of box surounded all kinds of annotations and links */
PHP_FUNCTION(ps_set_border_color)
{
	zval *zps;
	double red, green, blue;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddd", &zps, &red, &green, &blue)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_border_color(ps, (float) red, (float) green, (float) blue);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_dash(int psdoc, double black, double white)
   Sets the border dash style of all kinds of annotations and links */
PHP_FUNCTION(ps_set_border_dash)
{
	zval *zps;
	double black, white;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddd", &zps, &black, &white)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_set_border_dash(ps, (float) black, (float) white);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_annotation(int psdoc, double llx, double lly, double urx, double ury, string title, string text)
   Sets annotation (depreciated use ps_add_note instead) */
PHP_FUNCTION(ps_add_annotation)
{
	zval *zps;
	double llx, lly, urx, ury;
	char *title, *text;
	int title_len, text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddss", &zps, &llx, &lly, &urx, &ury, &title, &title_len, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_note(ps, (float) llx, (float) lly, (float) urx, (float) ury, title, text, "note", 1);

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

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	zend_list_delete(Z_LVAL_PP(arg1));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_open_file(int psdoc [, char filename])
   Opens a new PS document. If filename is NULL, document is created in memory. This is not yet fully supported */
PHP_FUNCTION(ps_open_file) {
	zval *zps;
	char *filename = NULL;
	int filename_len;
	int ps_file;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|s", &zps, &filename, &filename_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	if (filename) {
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
	zval *zps;
	long size;
	const char *buffer;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

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
	zval *zps;
	double a, b, c, d, e, f;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddddd", &zps, &a, &b, &c, &d, &e, &f)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_concat(ps, (float) a, (float) b, (float) c, (float) d, (float) e, (float) f);

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
	zval *zps;
	char *type, *params, *source, *data;
	int type_len, params_len, source_len, data_len;
	int length, width, height, components, bpc;
	int imageid;
	char *image;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsssllllls", &zps, &type, &type_len, &source, &source_len, &data, &data_len, &length, &width, &height, &components, &bpc, &params, &params_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

#ifdef VIRTUAL_DIR
	virtual_filepath(data, &image);
#else
	image = data;
#endif

	imageid = PS_open_image(ps, type, source, image, length, width, height, components, bpc, params);

	RETURN_LONG(imageid);
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_attach_file(int ps, double lly, double lly, double urx, double ury, string filename, string description, string author, string mimetype, string icon)
   Adds a file attachment annotation at the rectangle specified by his lower left and upper right corners */
PHP_FUNCTION(ps_attach_file) {
	zval *zps;
	double llx, lly, urx, ury;
	char *filename, *description, *author, *mimetype, *icon;
	int filename_len, description_len, author_len, mimetype_len, icon_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsssss", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len, &description, &description_len, &author, &author_len, &mimetype, &mimetype_len, &icon, &icon_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_attach_file(ps,
		(float) llx,
		(float) lly,
		(float) urx,
		(float) ury,
		filename,
		description,
		author,
		mimetype,
		icon);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_add_note(int psdoc, double llx, double lly, double urx, double ury, string contents, string title, string icon, int open)
   Sets annotation */
PHP_FUNCTION(ps_add_note) {
	zval *zps;
	double llx, lly, urx, ury;
	char *contents, *title, *icon;
	int contents_len, title_len, icon_len;
	int open;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsssl", &zps, &llx, &lly, &urx, &ury, &contents, &contents_len, &title, &title_len, &icon, &icon_len, &open)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_note(ps, (float) llx, (float) lly, (float) urx, (float) ury, contents, title, icon, open);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_locallink(int psdoc, double llx, double lly, double urx, double ury, int page, string dest)
   Adds link to web resource */
PHP_FUNCTION(ps_add_locallink) {
	zval *zps;
	double llx, lly, urx, ury;
	char *dest;
	int dest_len;
	int page;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddls", &zps, &llx, &lly, &urx, &ury, &page, &dest, &dest_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_locallink(ps, (float) llx, (float) lly, (float) urx, (float) ury, page, dest);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_launchlink(int psdoc, double llx, double lly, double urx, double ury, string filename)
   Adds link to web resource */
PHP_FUNCTION(ps_add_launchlink) {
	zval *zps;
	double llx, lly, urx, ury;
	char *filename;
	int filename_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddds", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_launchlink(ps, (float) llx, (float) lly, (float) urx, (float) ury, filename);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setcolor(int ps, string type, string colorspace, double c1, double c2, double c3, double c4);
 * Set the current color space and color. */
PHP_FUNCTION(ps_setcolor) {
	zval *zps;
	double c1, c2, c3, c4;
	char *type, *colorspace;
	int type_len, colorspace_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssdddd", &zps, &type, &type_len, &colorspace, &colorspace_len, &c1, &c2, &c3, &c4)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setcolor(ps, type, colorspace, (float) c1, (float) c2, (float) c3, (float) c4);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto int ps_makespotcolor(int ps, string spotname);
 * Make a named spot color from the current color. */
PHP_FUNCTION(ps_makespotcolor) {
	zval *zps;
	char *spotname;
	int spotname_len;
	int spot;
	double reserved = 0.0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|d", &zps, &spotname, &spotname_len, &reserved)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	spot = PS_makespotcolor(ps, spotname, reserved);

	RETURN_LONG(spot);
} /* }}} */

/* {{{ proto void ps_arcn(int ps, double x, double y, double r, double alpha, double beta);
 * Draw a clockwise circular arc from alpha to beta degrees. */
PHP_FUNCTION(ps_arcn) {
	zval *zps;
	double x, y, radius, start, end;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddd", &zps, &x, &y, &radius, &start, &end)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_arcn(ps, (float) x, (float) y, (float) radius, (float) start, (float) end);

	RETURN_TRUE;
} /* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_initgraphics(int ps);
 * Reset all implicit color and graphics state parameters to their defaults. */
PHP_FUNCTION(ps_initgraphics) {
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_initgraphics(ps);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto void ps_add_thumbnail(int ps, int image);
 * Add an existing image as thumbnail for the current page. */
PHP_FUNCTION(ps_add_thumbnail) {
	zval *zps;
	int imageid;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &imageid)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_add_thumbnail(ps, imageid);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto void ps_setmatrix(int ps, double a, double b, double c, double d, double e, double f)
   Explicitly set the current transformation matrix. */
PHP_FUNCTION(ps_setmatrix) {
	zval *zps;
	double a, b, c, d, e, f;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddddd", &zps, &a, &b, &c, &d, &e, &f)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_setmatrix(ps, (float) a, (float) b, (float) c, (float) d, (float) e, (float) f);

	RETURN_TRUE;
} /* }}} */
#endif

/* {{{ proto void ps_begin_pattern(int psdoc, double width, double height, double xstep, double ystep, int painttype)
   Starts pattern */
PHP_FUNCTION(ps_begin_pattern) 
{
	zval *zps;
	double width, height, xstep, ystep;
	int painttype;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddl", &zps, &width, &height, &xstep, &ystep, &painttype)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_begin_pattern(ps, (float) width,
	                     (float) height,
	                     (float) xstep,
	                     (float) ystep,
	                     (int) painttype);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_pattern(int psdoc)
   Ends pattern */
PHP_FUNCTION(ps_end_pattern) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_end_pattern(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_begin_template(int psdoc, double width, double height)
   Starts template */
PHP_FUNCTION(ps_begin_template) 
{
	zval *zps;
	double width, height;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &width, &height)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_begin_template(ps, (float) width, (float) height);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_template(int psdoc)
   Ends template */
PHP_FUNCTION(ps_end_template) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_end_template(ps);

	RETURN_TRUE;
}
/* }}} */

/* Function without an equivalent in pdflib */

/* {{{ proto void ps_hyphenate(int ps, string word)
   Hyphenate a given word */
PHP_FUNCTION(ps_hyphenate) {
	zval *zps;
	char *text;
	int text_len;
	char *buffer;
	int i, j;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	if(NULL == (buffer = emalloc(text_len+3))) {
		RETURN_FALSE;
	}
	if(0 > PS_hyphenate(ps, text, &buffer)) {
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
	zval *zps;
	int ord;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &ord)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_symbol(ps, (unsigned char) ord);

	RETURN_TRUE;

} /* }}} */

/* {{{ proto double ps_symbol_width(int psdoc, int ord [, int font, double size])
   Returns width of a glyph in current font */
PHP_FUNCTION(ps_symbol_width)
{
	zval *zps;
	int ord;
	double width, size = 0.0;
	int font = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|ld", &zps, &ord, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	width = (double) PS_symbol_width(ps, (unsigned char) ord, font, (float) size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto string ps_symbol_name(int psdoc, int ord [, int font])
   Returns name of a glyph in current font */
PHP_FUNCTION(ps_symbol_name)
{
	zval *zps;
	int ord;
	int font = 0;
	char glyphname[50];
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &zps, &ord, &font)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, &zps);

	PS_symbol_name(ps, (unsigned char) ord, font, glyphname, 50);

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
