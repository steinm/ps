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

/* $Id$ */

#ifndef PHP_PS_H
#define PHP_PS_H

#ifdef PHP_WIN32
#define PHP_PS_API __declspec(dllexport)
#else
#define PHP_PS_API
#endif

#if HAVE_PS

#include <libps/pslib.h>

extern zend_module_entry ps_module_entry;
#define ps_module_ptr &ps_module_entry

extern PHP_MINFO_FUNCTION(ps);
extern PHP_MINIT_FUNCTION(ps);
extern PHP_MSHUTDOWN_FUNCTION(ps);
PHP_FUNCTION(ps_new);	
PHP_FUNCTION(ps_delete);
PHP_FUNCTION(ps_open_file);
PHP_FUNCTION(ps_get_buffer);
PHP_FUNCTION(ps_close);
PHP_FUNCTION(ps_begin_page);
PHP_FUNCTION(ps_end_page);
PHP_FUNCTION(ps_get_value);
PHP_FUNCTION(ps_set_value);
PHP_FUNCTION(ps_get_parameter);
PHP_FUNCTION(ps_set_parameter);
PHP_FUNCTION(ps_findfont);
PHP_FUNCTION(ps_stringwidth);
PHP_FUNCTION(ps_string_geometry);
PHP_FUNCTION(ps_setfont);
PHP_FUNCTION(ps_show);
PHP_FUNCTION(ps_show_xy);
PHP_FUNCTION(ps_show2);
PHP_FUNCTION(ps_show_xy2);
PHP_FUNCTION(ps_continue_text);
PHP_FUNCTION(ps_show_boxed);
PHP_FUNCTION(ps_stringwidth);
PHP_FUNCTION(ps_set_text_pos);
PHP_FUNCTION(ps_setdash);
PHP_FUNCTION(ps_setpolydash);
PHP_FUNCTION(ps_setflat);
PHP_FUNCTION(ps_setlinejoin);
PHP_FUNCTION(ps_setlinecap);
PHP_FUNCTION(ps_setmiterlimit);
PHP_FUNCTION(ps_setlinewidth);
PHP_FUNCTION(ps_setoverprintmode);
PHP_FUNCTION(ps_save);
PHP_FUNCTION(ps_restore);
PHP_FUNCTION(ps_translate);
PHP_FUNCTION(ps_scale);
PHP_FUNCTION(ps_rotate);
PHP_FUNCTION(ps_skew);
PHP_FUNCTION(ps_concat);
PHP_FUNCTION(ps_moveto);
PHP_FUNCTION(ps_lineto);
PHP_FUNCTION(ps_curveto);
PHP_FUNCTION(ps_circle);
PHP_FUNCTION(ps_arc);
PHP_FUNCTION(ps_arcn);
PHP_FUNCTION(ps_rect);
PHP_FUNCTION(ps_closepath);
PHP_FUNCTION(ps_stroke);
PHP_FUNCTION(ps_closepath_stroke);
PHP_FUNCTION(ps_fill);
PHP_FUNCTION(ps_fill_stroke);
PHP_FUNCTION(ps_closepath_fill_stroke);
PHP_FUNCTION(ps_clip);
PHP_FUNCTION(ps_endpath);
PHP_FUNCTION(ps_open_image_file); 
PHP_FUNCTION(ps_open_ccitt);
PHP_FUNCTION(ps_open_image);
PHP_FUNCTION(ps_close_image);
PHP_FUNCTION(ps_place_image);
PHP_FUNCTION(ps_shading);
PHP_FUNCTION(ps_shfill);
PHP_FUNCTION(ps_shading_pattern);
PHP_FUNCTION(ps_add_bookmark);
PHP_FUNCTION(ps_set_info);
PHP_FUNCTION(ps_attach_file);
PHP_FUNCTION(ps_include_file);
PHP_FUNCTION(ps_add_note);
PHP_FUNCTION(ps_add_pdflink);
PHP_FUNCTION(ps_add_locallink);
PHP_FUNCTION(ps_add_launchlink);
PHP_FUNCTION(ps_add_weblink);
PHP_FUNCTION(ps_set_border_style);
PHP_FUNCTION(ps_set_border_color);
PHP_FUNCTION(ps_set_border_dash);
PHP_FUNCTION(ps_setcolor);
PHP_FUNCTION(ps_makespotcolor);
PHP_FUNCTION(ps_begin_pattern);
PHP_FUNCTION(ps_end_pattern);
PHP_FUNCTION(ps_begin_template);
PHP_FUNCTION(ps_end_template);
#ifdef HAVE_PSBEGINFONT
PHP_FUNCTION(ps_begin_font);
PHP_FUNCTION(ps_end_font);
PHP_FUNCTION(ps_begin_glyph);
PHP_FUNCTION(ps_end_glyph);
#endif

/* Function without an equivalent in pdflib */
PHP_FUNCTION(ps_hyphenate);
PHP_FUNCTION(ps_symbol);
PHP_FUNCTION(ps_symbol_name);
PHP_FUNCTION(ps_symbol_width);
#ifdef HAVE_PSGLYPHSHOW
PHP_FUNCTION(ps_glyph_show);
PHP_FUNCTION(ps_glyph_width);
PHP_FUNCTION(ps_glyph_list);
#endif
#ifdef HAVE_PSBEGINFONT
PHP_FUNCTION(ps_add_kerning);
PHP_FUNCTION(ps_add_ligature);
#endif

/* old way of starting a PS document */
PHP_FUNCTION(ps_open);			/* deprecated */

/* some more stuff for compatibility */
#if HAVE_LIBGD13
PHP_FUNCTION(ps_open_memory_image);
#endif

#ifdef ZTS
# define PsSG(v) TSRMG(ps_globals_id, zend_ps_globals *, v)
#else
# define PsSG(v) (ps_globals.v)
#endif

#define PSDOC_FROM_ZVAL(psdoc, zv)    ZEND_FETCH_RESOURCE(psdoc, PSDoc *, zv, -1, "ps document", le_psdoc)

#else
#define ps_module_ptr NULL
#endif
#define phpext_ps_ptr ps_module_ptr
#endif /* PHP_PS_H */
