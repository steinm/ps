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
   | Authors: Uwe Steinmann <Uwe.Steinmann@fernuni-hagen.de>              |
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
static int le_psfont;

/*
 * to adopt the php way of error handling to PSlib
 * The image related functions in PSlib return -1 on error
 * but they may return 0 (FALSE) in normal cases
 * so this offset will repair this
 */
#define PSLIB_IMAGE_OFFSET	1
#define PSLIB_FONT_OFFSET	1
#define PSLIB_PDI_OFFSET	1
#define PSLIB_PATTERN_OFFSET	1
#define PSLIB_SPOT_OFFSET	1

/* {{{ ps_functions[]
 */
function_entry ps_functions[] = {
	PHP_FE(ps_new, NULL)		/* new function */
	PHP_FE(ps_delete, NULL)	/* new function */
	PHP_FE(ps_open_file, NULL)	/* new function */
//	PHP_FE(ps_get_buffer, NULL)	/* new function */
	PHP_FE(ps_close, NULL)
	PHP_FE(ps_begin_page, NULL)
	PHP_FE(ps_end_page, NULL)
	PHP_FE(ps_get_value, NULL)
	PHP_FE(ps_set_value, NULL)
	PHP_FE(ps_get_parameter, NULL)
	PHP_FE(ps_set_parameter, NULL)
	PHP_FE(ps_findfont, NULL)	/* new function */
	PHP_FE(ps_setfont, NULL)	/* new function */
	PHP_FE(ps_show, NULL)
	PHP_FE(ps_show_xy, NULL)
	PHP_FE(ps_show2, NULL)
	PHP_FE(ps_show_xy2, NULL)
	PHP_FE(ps_continue_text, NULL)
	PHP_FE(ps_show_boxed, NULL)
	PHP_FE(ps_stringwidth, NULL)	/* new parameters: [int font, float size] */
//	PHP_FE(ps_set_text_pos, NULL)
	PHP_FE(ps_setdash, NULL)
	PHP_FE(ps_setpolydash, NULL)	/* new function: not yet finished */
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
//	PHP_FE(ps_clip, NULL)
//	PHP_FE(ps_endpath, NULL)
//	PHP_FE(ps_open_image_file, NULL)  /* new parameters: [char *stringpram, int intparam] */
//	PHP_FE(ps_open_ccitt, NULL)	/* new function */
//	PHP_FE(ps_open_image, NULL)	/* new function */
//	PHP_FE(ps_close_image, NULL)
//	PHP_FE(ps_place_image, NULL)
//	PHP_FE(ps_add_bookmark, NULL)
	PHP_FE(ps_set_info, NULL)
//	PHP_FE(ps_attach_file, NULL)	/* new function */
//	PHP_FE(ps_add_note, NULL)	/* new function */
//	PHP_FE(ps_add_pslink, NULL)
//	PHP_FE(ps_add_locallink, NULL)	/* new function */
//	PHP_FE(ps_add_launchlink, NULL)/* new function */
//	PHP_FE(ps_add_weblink, NULL)
//	PHP_FE(ps_set_border_style, NULL)
//	PHP_FE(ps_set_border_color, NULL)
//	PHP_FE(ps_set_border_dash, NULL)
	PHP_FE(ps_setcolor, NULL)

	/* End of the official PSLIB V3.x API */

	/* aliases for compatibility reasons */
//	PHP_FALIAS(ps_add_outline, ps_add_bookmark, NULL)

	/* old font handling */
//	PHP_FE(ps_set_font, NULL)		/* deprecated */
	PHP_FE(ps_get_font, NULL)		/* deprecated */
	PHP_FE(ps_get_fontname, NULL)		/* deprecated */
	PHP_FE(ps_get_fontsize, NULL)		/* deprecated */

	/* old way of starting a PS document */
	PHP_FE(ps_open, NULL)			/* deprecated */

	/* old stuff for setting infos */
	PHP_FE(ps_set_info_creator, NULL)	/* deprecated */
	PHP_FE(ps_set_info_title, NULL)	/* deprecated */
	PHP_FE(ps_set_info_subject, NULL)	/* deprecated */
	PHP_FE(ps_set_info_author, NULL)	/* deprecated */
	PHP_FE(ps_set_info_keywords, NULL)	/* deprecated */
//	PHP_FE(ps_set_leading, NULL)   	/* deprecated */
//	PHP_FE(ps_set_text_rendering, NULL)	/* deprecated */
//	PHP_FE(ps_set_horiz_scaling, NULL)	/* deprecated */
//	PHP_FE(ps_set_text_rise, NULL)		/* deprecated */
//	PHP_FE(ps_set_char_spacing, NULL)	/* deprecated */
//	PHP_FE(ps_set_word_spacing, NULL)	/* deprecated */
//	PHP_FE(ps_set_transition, NULL)	/* deprecated */
//	PHP_FE(ps_set_duration, NULL)		/* deprecated */
//	PHP_FE(ps_get_image_height, NULL)	/* deprecated */
//	PHP_FE(ps_get_image_width, NULL)	/* deprecated */

	/* old stuff for opening images */
//	PHP_FE(ps_open_jpeg, NULL)		/* deprecated */
//	PHP_FE(ps_open_tiff, NULL)		/* deprecated */
//	PHP_FE(ps_open_png, NULL)		/* deprecated */
//	PHP_FE(ps_open_gif, NULL)		/* deprecated */

	/* some more stuff for compatibility */
//	PHP_FE(ps_add_annotation, NULL)
#ifdef _HAVE_LIBGD13
	PHP_FE(ps_open_memory_image, NULL)
#endif
	/* depreciatet after V4.0 of PSlib */
	PHP_FE(ps_setgray_fill, NULL)
	PHP_FE(ps_setgray_stroke, NULL)
	PHP_FE(ps_setgray, NULL)
//	PHP_FE(ps_setrgbcolor_fill, NULL)
//	PHP_FE(ps_setrgbcolor_stroke, NULL)
	PHP_FE(ps_setrgbcolor, NULL)

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

/* {{{ _free_ps_font
 */
static void _free_ps_font(zend_rsrc_list_entry *rsrc)
{
	PSFont *psfont = (PSFont *)rsrc->ptr;
}
/* }}} */

/* {{{ custom_errorhandler
 */
static void custom_errorhandler(PSDoc *p, int type, const char *shortmsg)
{
	switch (type){
		case PS_Warning:
				php_error(E_WARNING,"PSlib warning: %s", shortmsg);
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
			php_error(E_WARNING,"Internal PSlib warning: %s", shortmsg);
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
			php_error(E_ERROR,"PSlib error: %s", shortmsg);
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
	le_psfont = zend_register_list_destructors_ex(_free_ps_font, NULL, "ps font", module_number);

	REGISTER_LONG_CONSTANT("PS_LINECAP_BUTT", PS_LINECAP_BUTT, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINECAP_ROUND", PS_LINECAP_ROUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINECAP_SQUARED", PS_LINECAP_SQUARED, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_MITER", PS_LINEJOIN_MITER, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_ROUND", PS_LINEJOIN_ROUND, CONST_CS | CONST_PERSISTENT);
	REGISTER_LONG_CONSTANT("PS_LINEJOIN_BEVEL", PS_LINEJOIN_BEVEL, CONST_CS | CONST_PERSISTENT);

	/* this does something like setlocale("C", ...) in PDFlib 3.x */
	/* but it does segm fault the web server, so keep in commented
	PS_boot();
	*/
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

/* {{{ _php_ps_set_info
 */
static void _php_ps_set_info(INTERNAL_FUNCTION_PARAMETERS, char *field) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	PS_set_info(ps, field, Z_STRVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ps_set_info(int psdoc, string fieldname, string value)
   Fills an info field of the document */
PHP_FUNCTION(ps_set_info) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);
	PS_set_info(ps, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto bool ps_set_info_creator(int psdoc, string creator)
   Fills the creator field of the document */
PHP_FUNCTION(ps_set_info_creator)
{
	_php_ps_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Creator");
}
/* }}} */

/* {{{ proto bool ps_set_info_title(int psdoc, string title)
   Fills the title field of the document */
PHP_FUNCTION(ps_set_info_title) 
{
	_php_ps_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Title");
}
/* }}} */

/* {{{ proto bool ps_set_info_subject(int psdoc, string subject)
   Fills the subject field of the document */
PHP_FUNCTION(ps_set_info_subject) 
{
	_php_ps_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Subject");
}
/* }}} */

/* {{{ proto bool ps_set_info_author(int psdoc, string author)
   Fills the author field of the document */
PHP_FUNCTION(ps_set_info_author) 
{
	_php_ps_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Author");
}
/* }}} */

/* {{{ proto bool ps_set_info_keywords(int psdoc, string keywords)
   Fills the keywords field of the document */
PHP_FUNCTION(ps_set_info_keywords) {
	_php_ps_set_info(INTERNAL_FUNCTION_PARAM_PASSTHRU, "Keywords");
}
/* }}} */

/* {{{ proto int ps_open([int filedesc])
   Opens a new ps document. If filedesc is NULL, document is created in memory. This is the old interface, only for compatibility use ps_new + ps_open_file instead */
PHP_FUNCTION(ps_open) 
{
	zval **file;
	void *what;
	int type;
	FILE *fp = NULL;
	PSDoc *ps;
	int argc = ZEND_NUM_ARGS();

	if(argc > 1) 
		WRONG_PARAM_COUNT;
	if (argc != 1 || zend_get_parameters_ex(1, &file) == FAILURE) {
		fp = NULL;
	} else {
		what = zend_fetch_resource(file TSRMLS_CC, -1, "File-Handle", &type, 1, php_file_le_stream());
		ZEND_VERIFY_RESOURCE(what);
		if (php_stream_cast((php_stream*)what, PHP_STREAM_AS_STDIO, (void*)&fp, 1) == FAILURE)  {
			RETURN_FALSE;
		}
		/* XXX should do a zend_list_addref for <fp> here! */
	}

	ps = PS_new2(custom_errorhandler, ps_emalloc, ps_erealloc, ps_efree, NULL);

	if(fp) {
		if (PS_open_fp(ps, fp) < 0) RETURN_FALSE;
	} else {
		PS_open_mem(ps, ps_flushwrite);
	}

//	PS_set_parameter(ps, "imagewarning", "true");
//	PS_set_parameter(ps, "binding", "PHP");

	ZEND_REGISTER_RESOURCE(return_value, ps, le_psdoc);
}

/* }}} */

/* {{{ proto void ps_close(int psdoc)
   Closes the ps document */
PHP_FUNCTION(ps_close) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_close(ps);

	RETURN_TRUE;
}

/* }}} */

/* {{{ proto void ps_begin_page(int psdoc, double width, double height)
   Starts page */
PHP_FUNCTION(ps_begin_page) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_begin_page(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_page(int psdoc)
   Ends page */
PHP_FUNCTION(ps_end_page) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_end_page(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_parameter(int psdoc, string key, string value)
   Sets arbitrary parameters */
PHP_FUNCTION(ps_set_parameter)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps object", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_string_ex(arg3);

	PS_set_parameter(ps, Z_STRVAL_PP(arg2), Z_STRVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto string ps_get_parameter(int psdoc, string key, mixed modifier)
   Gets arbitrary parameters */
PHP_FUNCTION(ps_get_parameter)
{
	zval **argv[3];
	int argc = ZEND_NUM_ARGS();
	PSDoc *ps;
	char *value;

	if(((argc < 2) || (argc > 3)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ps, PSDoc *, argv[0], -1, "ps object", le_psdoc);

	convert_to_string_ex(argv[1]);
	if(argc == 3) {
		convert_to_double_ex(argv[2]);
		value = (char *) PS_get_parameter(ps, Z_STRVAL_PP(argv[1]), (float) Z_DVAL_PP(argv[2]));
	} else {
		value = (char *) PS_get_parameter(ps, Z_STRVAL_PP(argv[1]), 0.0);
	}

	RETURN_STRING(value, 1);
}
/* }}} */

/* {{{ proto void ps_set_value(int psdoc, string key, float value)
   Sets arbitrary value */
PHP_FUNCTION(ps_set_value)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps object", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	PS_set_value(ps, Z_STRVAL_PP(arg2), (float)Z_DVAL_PP(arg3));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto float ps_get_value(int psdoc, string key, float modifier)
   Gets arbitrary value */
PHP_FUNCTION(ps_get_value)
{
	zval **argv[3];
	int argc = ZEND_NUM_ARGS();
	PSDoc *ps;
	double value;

	if(((argc < 2) || (argc > 3)) || zend_get_parameters_array_ex(argc, argv) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, argv[0], -1, "ps object", le_psdoc);

	convert_to_string_ex(argv[1]);
	if(argc == 3)
	    convert_to_double_ex(argv[2]);

/*
	if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "imagewidth"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "imageheight"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "resx"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "resy"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_IMAGE_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "capheight"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "ascender"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "descender"))) {
		if(argc < 3) WRONG_PARAM_COUNT;
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2])-PSLIB_FONT_OFFSET);
	} else if(0 == (strcmp(Z_STRVAL_PP(argv[1]), "font"))) {
		value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), 0.0)+PSLIB_FONT_OFFSET;
	} else { */
		if(argc < 3) {
		    value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), 0.0);
		} else {
			value = PS_get_value(ps, Z_STRVAL_PP(argv[1]), (float)Z_DVAL_PP(argv[2]));
		}
/*	} */

	RETURN_DOUBLE(value);
}
/* }}} */

/* {{{ proto void ps_show(int psdoc, string text)
   Output text at current position */
PHP_FUNCTION(ps_show) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	PS_show(ps, Z_STRVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show2(int psdoc, string text, int len)
   Output text at current position */
PHP_FUNCTION(ps_show2) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_long_ex(arg3);
	PS_show2(ps, Z_STRVAL_PP(arg2), Z_LVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy(int psdoc, string text, double x-koor, double y-koor)
   Output text at position */
PHP_FUNCTION(ps_show_xy) 
{
	zval **arg1, **arg2, **arg3, **arg4;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 4 || zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	PS_show_xy(ps, Z_STRVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy2(int psdoc, string text, int len, double x-koor, double y-koor)
   Output text at position */
PHP_FUNCTION(ps_show_xy2) 
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);
	PS_show_xy2(ps, Z_STRVAL_PP(arg2), Z_LVAL_PP(arg3), (float) Z_DVAL_PP(arg4), (float) Z_DVAL_PP(arg5));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_findfont(int psdoc, string fontname, string encoding [, int embed])
   Prepares the font fontname for later use with ps_setfont() */
PHP_FUNCTION(ps_findfont) {
	zval **arg1, **arg2, **arg3, **arg4;
	int embed;
	const char *fontname, *encoding;
	PSDoc *ps;
	PSFont *font;

	switch (ZEND_NUM_ARGS()) {
	case 3:
		if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		embed = 0;
		break;
	case 4:
		if (zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long_ex(arg4);
		embed = Z_LVAL_PP(arg4);
		break;
	default:
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);
	fontname = Z_STRVAL_PP(arg2);

	convert_to_string_ex(arg3);
	encoding = Z_STRVAL_PP(arg3);

	font = PS_findfont(ps, fontname, encoding, embed);
	if (font == NULL) {
		/* pslib will do this for you, will throw some exception
		php_error(E_WARNING,"Font %s not found", fontname);
		*/
		RETURN_FALSE;
	}

	ZEND_REGISTER_RESOURCE(return_value, font, le_psfont);
}
/* }}} */

/* {{{ proto void ps_setfont(int psdoc, int font, float fontsize)
   Sets the current font in the fiven fontsize */
PHP_FUNCTION(ps_setfont) {
	zval **arg1, **arg2, **arg3;
	float fontsize;
	PSDoc *ps;
	PSFont *font;

	if(ZEND_NUM_ARGS() != 3)
		WRONG_PARAM_COUNT;
	if (zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		WRONG_PARAM_COUNT;

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	ZEND_FETCH_RESOURCE(font, PSFont *, arg2, -1, "ps font", le_psfont);

	convert_to_double_ex(arg3);
	fontsize = (float)Z_DVAL_PP(arg3);

	PS_setfont(ps, font, fontsize);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_rotate(int psdoc, double angle)
   Sets rotation */
PHP_FUNCTION(ps_rotate)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_rotate(ps, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_rect(int psdoc, double x, double y, double width, double height)
   Draws a rectangle */
PHP_FUNCTION(ps_rect)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 5 || zend_get_parameters_ex(5, &arg1, &arg2, &arg3, &arg4, &arg5) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	convert_to_double_ex(arg4);
	convert_to_double_ex(arg5);

	PS_rect(ps, (float) Z_DVAL_PP(arg2),
				  (float) Z_DVAL_PP(arg3),
				  (float) Z_DVAL_PP(arg4),
				  (float) Z_DVAL_PP(arg5));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinewidth(int psdoc, double width)
   Sets line width */
PHP_FUNCTION(ps_setlinewidth)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_setlinewidth(ps, (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setdash(int psdoc, float black, float white)
   Sets dash pattern */
PHP_FUNCTION(ps_setdash)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps object", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_setdash(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_stroke(int psdoc)
   Draw line along path path */
PHP_FUNCTION(ps_stroke)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_stroke(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_fill(int psdoc)
   Fill current path */
PHP_FUNCTION(ps_fill) 
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_fill(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_fill_stroke(int psdoc)
   Fill and stroke current path */
PHP_FUNCTION(ps_fill_stroke)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_fill_stroke(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setgray(int psdoc, double value)
   Sets drawing and filling color to gray value */
PHP_FUNCTION(ps_setgray)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_setcolor(ps, "both", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_save(int psdoc)
   Saves current enviroment */
PHP_FUNCTION(ps_save)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_save(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_restore(int psdoc)
   Restores formerly saved enviroment */
PHP_FUNCTION(ps_restore)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_restore(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_lineto(int psdoc, double x, double y)
   Draws a line */
PHP_FUNCTION(ps_lineto)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_lineto(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_moveto(int psdoc, double x, double y)
   Sets current point */
PHP_FUNCTION(ps_moveto)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_moveto(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath(int psdoc)
   Close path */
PHP_FUNCTION(ps_closepath)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_closepath(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_closepath_stroke(int psdoc)
   Close path and draw line along path */
PHP_FUNCTION(ps_closepath_stroke)
{
	zval **arg1;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	PS_closepath_stroke(ps);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_translate(int psdoc, double x, double y)
   Sets origin of coordinate system */
PHP_FUNCTION(ps_translate) 
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_translate(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_scale(int psdoc, double x-scale, double y-scale)
   Sets scaling */
PHP_FUNCTION(ps_scale)
{
	zval **arg1, **arg2, **arg3;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 3 || zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	convert_to_double_ex(arg3);
	PS_scale(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3));
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
		php_error(E_WARNING,"Parameter of ps_setlinejoin() must be between 0 and 2");
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
		php_error(E_WARNING,"Parameter of ps_setlinecap() must be > 0 and <= 2");
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
		php_error(E_WARNING,"Parameter of ps_setmiterlimit() must be >= 1");
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

/* _php_ps_set_value() {{{
 */
static void _php_ps_set_value(INTERNAL_FUNCTION_PARAMETERS, char *field) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_set_value(ps, field, (float)Z_DVAL_PP(arg2));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_get_font(int psdoc)
   Gets the current font */
PHP_FUNCTION(ps_get_font) 
{
	zval **arg1;
//	int font;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	/* FIXME: Might be better to return the existing resource */
	ZEND_REGISTER_RESOURCE(return_value, PS_getfont(ps), le_psfont);
//	font = (int) PS_get_value(ps, "font", 0);
//	RETURN_LONG(font+PSLIB_FONT_OFFSET);
}
/* }}} */

/* {{{ proto string ps_get_fontname(int psdoc)
   Gets the current font name */
PHP_FUNCTION(ps_get_fontname) 
{
	zval **arg1;
	char *fontname;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	fontname = (char *) PS_get_parameter(ps, "fontname", 0);
	RETURN_STRING(fontname, 1);
}
/* }}} */

/* {{{ proto double ps_get_fontsize(int psdoc)
   Gets the current font size */
PHP_FUNCTION(ps_get_fontsize) 
{
	zval **arg1;
	float fontsize;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 1 || zend_get_parameters_ex(1, &arg1) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	fontsize = PS_get_value(ps, "fontsize", 0);
	RETURN_DOUBLE(fontsize);
}
/* }}} */

/* {{{ proto void ps_set_leading(int psdoc, double distance)
   Sets distance between text lines */
PHP_FUNCTION(ps_set_leading) 
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "leading");
}
/* }}} */

/* {{{ proto void ps_set_text_rendering(int psdoc, int mode)
   Determines how text is rendered */
PHP_FUNCTION(ps_set_text_rendering) 
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "textrendering");
}
/* }}} */

/* {{{ proto void ps_set_horiz_scaling(int psdoc, double scale)
   Sets horizontal scaling of text */
PHP_FUNCTION(ps_set_horiz_scaling) 
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "horizscaling");
}
/* }}} */

/* {{{ proto void ps_set_text_rise(int psdoc, double value)
   Sets the text rise */
PHP_FUNCTION(ps_set_text_rise) 
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "textrise");
}
/* }}} */

/* {{{ proto void ps_set_char_spacing(int psdoc, double space)
   Sets character spacing */
PHP_FUNCTION(ps_set_char_spacing)
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "charspacing");
}
/* }}} */

/* {{{ proto void ps_set_word_spacing(int psdoc, double space)
   Sets spacing between words */
PHP_FUNCTION(ps_set_word_spacing)
{
	_php_ps_set_value(INTERNAL_FUNCTION_PARAM_PASSTHRU, "wordspacing");
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
	PSFont *font;
	double width, size;
	PSDoc *ps;

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
			font = NULL;
			size = 0;
	} else {
			ZEND_FETCH_RESOURCE(font, PSFont *, arg3, -1, "ps font", le_psfont);
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
		php_error(E_WARNING,"Parameter of ps_setflat() has to between 0 and 100");
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

/* {{{ proto void ps_setgray_fill(int psdoc, double value)
   Sets filling color to gray value */
PHP_FUNCTION(ps_setgray_fill)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_setcolor(ps, "fill", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setgray_stroke(int psdoc, double value)
   Sets drawing color to gray value */
PHP_FUNCTION(ps_setgray_stroke) 
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_setcolor(ps, "stroke", "gray", (float) Z_DVAL_PP(arg2), 0, 0, 0);
	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
/* {{{ proto void ps_setrgbcolor_fill(int psdoc, double red, double green, double blue)
   Sets filling color to RGB color value */
PHP_FUNCTION(ps_setrgbcolor_fill)
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
#if (PSLIB_MAJORVERSION >= 4)
	PS_setcolor(ps, "fill", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
#else
	PS_setrgbcolor_fill(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
#endif
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setrgbcolor_stroke(int psdoc, double red, double green, double blue)
   Sets drawing color to RGB color value */
PHP_FUNCTION(ps_setrgbcolor_stroke)
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
#if (PSLIB_MAJORVERSION >= 4)
	PS_setcolor(ps, "stroke", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
#else
	PS_setrgbcolor_stroke(ps, (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4));
#endif
	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_setrgbcolor(int psdoc, double red, double green, double blue)
   Sets drawing and filling color to RGB color value */
PHP_FUNCTION(ps_setrgbcolor)
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
	PS_setcolor(ps, "both", "rgb", (float) Z_DVAL_PP(arg2), (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), 0);
	RETURN_TRUE;
}
/* }}} */

#ifdef notimplementedyet
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

/* {{{ proto void ps_set_transition(int psdoc, int transition)
   Sets transition between pages */
PHP_FUNCTION(ps_set_transition)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_long_ex(arg2);

	switch(Z_LVAL_PP(arg2)) {
		case 0:
			PS_set_parameter(ps, "transition", "none");
			break;
		case 1:
			PS_set_parameter(ps, "transition", "split");
			break;
		case 2:
			PS_set_parameter(ps, "transition", "blinds");
			break;
		case 3:
			PS_set_parameter(ps, "transition", "box");
			break;
		case 4:
			PS_set_parameter(ps, "transition", "wipe");
			break;
		case 5:
			PS_set_parameter(ps, "transition", "dissolve");
			break;
		case 6:
			PS_set_parameter(ps, "transition", "glitter");
			break;
		case 7:
			PS_set_parameter(ps, "transition", "replace");
			break;
		default:
			PS_set_parameter(ps, "transition", "none");
	}

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_duration(int psdoc, double duration)
   Sets duration between pages */
PHP_FUNCTION(ps_set_duration)
{
	zval **arg1, **arg2;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_double_ex(arg2);
	PS_set_value(ps, "duration", (float) Z_DVAL_PP(arg2));
	RETURN_TRUE;
}
/* }}} */

/* _php_ps_open_image() {{{
 */
static void _php_ps_open_image(INTERNAL_FUNCTION_PARAMETERS, char *type) 
{
	zval **arg1, **arg2;
	PSDoc *ps;
	int ps_image;
	char *image;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);

	convert_to_string_ex(arg2);

#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &image);
#else
	image = Z_STRVAL_PP(arg2);
#endif  
        
	ps_image = PS_open_image_file(ps, type, image, "", 0);

	RETURN_LONG(ps_image+PSLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto int ps_open_gif(int ps, string giffile)
   Opens a GIF file and returns an image for placement in a ps document */
PHP_FUNCTION(ps_open_gif)
{
	_php_ps_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"gif");
}
/* }}} */

/* {{{ proto int ps_open_jpeg(int ps, string jpegfile)
   Opens a JPEG file and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_jpeg)
{
	_php_ps_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"jpeg");
}
/* }}} */

/* {{{ proto int ps_open_png(int ps, string pngfile)
   Opens a PNG file and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_png)
{
	_php_ps_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"png");
}
/* }}} */

/* {{{ proto int ps_open_tiff(int ps, string tifffile)
   Opens a TIFF file and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_tiff)
{
	_php_ps_open_image(INTERNAL_FUNCTION_PARAM_PASSTHRU,"tiff");
}
/* }}} */

/* {{{ proto int ps_open_image_file(int ps, string type, string file, string stringparam, int intparam)
   Opens an image file of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image_file)
{
	zval **arg1, **arg2, **arg3, **arg4, **arg5;
	PSDoc *ps;
	int ps_image, argc;
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
		ps_image = PS_open_image_file(ps, Z_STRVAL_PP(arg2), image, "", 0);
	} else {
	    convert_to_string_ex(arg4);
	    convert_to_long_ex(arg5);
		ps_image = PS_open_image_file(ps, Z_STRVAL_PP(arg2), image, Z_STRVAL_PP(arg4), Z_LVAL_PP(arg5));
	}

	if (ps_image == -1) {
	    /* pslib will do this for you, will throw some exception
	    php_error(E_WARNING, "Could not open image: %s", image);
	    */
	    RETURN_FALSE;
	}
	RETURN_LONG(ps_image+PSLIB_IMAGE_OFFSET);

}
/* }}} */
#endif /* notimplementedyet */

#ifdef _HAVE_LIBGD13
/* {{{ proto int ps_open_memory_image(int ps, int image)
   Takes an GD image and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_memory_image)
{
	zval **arg1, **arg2;
	int i, j, color, count;
	int ps_image;
	gdImagePtr im;
	unsigned char *buffer, *ptr;
	PSDoc *ps;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	
	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	ZEND_GET_RESOURCE_TYPE_ID(le_gd, "gd");
	if(!le_gd) {
		php_error(E_ERROR, "Unable to find handle for GD image stream. Please check the GD extension is loaded.");
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

	ps_image = PS_open_image(ps, "raw", "memory", buffer, im->sx*im->sy*3, im->sx, im->sy, 3, 8, NULL);
	efree(buffer);

	if(ps_image == -1) {
		/* pslib will do this for you, will throw some exception
		php_error(E_WARNING, "Could not open image");
		*/
		RETURN_FALSE;
	}

	RETURN_LONG(ps_image+PSLIB_IMAGE_OFFSET);
}
/* }}} */
#endif /* HAVE_LIBGD13 */

#ifdef notimplementedyet
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

	PS_close_image(ps, Z_LVAL_PP(arg2)-PSLIB_IMAGE_OFFSET);
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

	PS_place_image(ps, Z_LVAL_PP(arg2)-PSLIB_IMAGE_OFFSET, (float) Z_DVAL_PP(arg3), (float) Z_DVAL_PP(arg4), (float) Z_DVAL_PP(arg5));
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_get_image_width(int ps, int psimage)
   Returns the width of an image */
PHP_FUNCTION(ps_get_image_width)
{
	zval **arg1, **arg2;
	PSDoc *ps;
	int width;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	convert_to_long_ex(arg2);

	width = (int) PS_get_value(ps, "imagewidth", (float)Z_LVAL_PP(arg2)-PSLIB_IMAGE_OFFSET);
	RETURN_LONG(width);
}
/* }}} */

/* {{{ proto int ps_get_image_height(int ps, int psimage)
   Returns the height of an image */
PHP_FUNCTION(ps_get_image_height)
{
	zval **arg1, **arg2;
	PSDoc *ps;
	int height;

	if (ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	convert_to_long_ex(arg2);

	height = (int) PS_get_value(ps, "imageheight", (float)Z_LVAL_PP(arg2)-PSLIB_IMAGE_OFFSET);
	RETURN_LONG(height);
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

/* {{{ proto void ps_add_pslink(int psdoc, double llx, double lly, double urx, double ury, string filename, int page, string dest)
   Adds link to PS document */
PHP_FUNCTION(ps_add_pslink)
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
	PS_add_pslink(ps, (float) Z_DVAL_PP(arg2), 
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
#endif

/* {{{ proto int ps_new()
   Creates a new PS object */
PHP_FUNCTION(ps_new) {
	PSDoc *ps;

	ps = PS_new2(custom_errorhandler, ps_emalloc, ps_erealloc, ps_efree, NULL);
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
		ps_file = PS_open_file(ps, "");
	}

	if (ps_file == -1)
	    RETURN_FALSE;

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
		php_error(E_WARNING,"PSlib set_polydash: illegal darray value");
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
	int ps_image;
	char *image;

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

	ps_image = PS_open_CCITT(ps,
	    image,
	    Z_LVAL_PP(arg3),
	    Z_LVAL_PP(arg4),
	    Z_LVAL_PP(arg5),
	    Z_LVAL_PP(arg6),
	    Z_LVAL_PP(arg7));

	RETURN_LONG(ps_image+PSLIB_IMAGE_OFFSET);
}
/* }}} */

/* {{{ proto int ps_open_image(int ps, string type, string source, string data, long length, int width, int height, int components, int bpc, string params)
   Opens an image of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image) {
	zval **arg1, **arg2, **arg3, **arg4, **arg5, **arg6, **arg7, **arg8, **arg9, **arg10;
	PSDoc *ps;
	int ps_image;
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

	ps_image = PS_open_image(ps,
		Z_STRVAL_PP(arg2),
		Z_STRVAL_PP(arg3),
		image,
		Z_LVAL_PP(arg5),
		Z_LVAL_PP(arg6),
		Z_LVAL_PP(arg7),
		Z_LVAL_PP(arg8),
		Z_LVAL_PP(arg9),
		Z_STRVAL_PP(arg10));

	RETURN_LONG(ps_image+PSLIB_IMAGE_OFFSET);
}
/* }}} */

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
#endif

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
	    c1 = Z_DVAL_PP(arg4)-PSLIB_SPOT_OFFSET;
	} else if(0 == (strcmp(Z_STRVAL_PP(arg3), "pattern"))) {
	    c1 = Z_DVAL_PP(arg4)-PSLIB_PATTERN_OFFSET;
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

	PS_add_thumbnail(ps,
		Z_LVAL_PP(arg2)-PSLIB_IMAGE_OFFSET);

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

#endif /* 0 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
