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

#if HAVE_PS

#include <libps/pslib.h>

extern zend_module_entry ps_module_entry;
#define ps_module_ptr &ps_module_entry

extern PHP_MINFO_FUNCTION(ps);
extern PHP_MINIT_FUNCTION(ps);
extern PHP_MSHUTDOWN_FUNCTION(ps);
PHP_FUNCTION(ps_new);		/* new function */
PHP_FUNCTION(ps_delete);	/* new function */
PHP_FUNCTION(ps_open_file);
PHP_FUNCTION(ps_get_buffer);	/* new function */
PHP_FUNCTION(ps_close);
PHP_FUNCTION(ps_begin_page);
PHP_FUNCTION(ps_end_page);
PHP_FUNCTION(ps_get_value);
PHP_FUNCTION(ps_set_value);
PHP_FUNCTION(ps_get_parameter);
PHP_FUNCTION(ps_set_parameter);
PHP_FUNCTION(ps_findfont);	/* new function */
PHP_FUNCTION(ps_stringwidth);	/* new function */
PHP_FUNCTION(ps_setfont);	/* new function */
PHP_FUNCTION(ps_show);
PHP_FUNCTION(ps_show_xy);
PHP_FUNCTION(ps_show2);
PHP_FUNCTION(ps_show_xy2);
PHP_FUNCTION(ps_continue_text);
PHP_FUNCTION(ps_show_boxed);
PHP_FUNCTION(ps_stringwidth);	/* new parameters: [int font, float size] */
PHP_FUNCTION(ps_set_text_pos);
PHP_FUNCTION(ps_setdash);
PHP_FUNCTION(ps_setpolydash);	/* new function: not yet finished */
PHP_FUNCTION(ps_setflat);
PHP_FUNCTION(ps_setlinejoin);
PHP_FUNCTION(ps_setlinecap);
PHP_FUNCTION(ps_setmiterlimit);
PHP_FUNCTION(ps_setlinewidth);
PHP_FUNCTION(ps_save);
PHP_FUNCTION(ps_restore);
PHP_FUNCTION(ps_translate);
PHP_FUNCTION(ps_scale);
PHP_FUNCTION(ps_rotate);
PHP_FUNCTION(ps_skew);
PHP_FUNCTION(ps_concat);	/* new function */
PHP_FUNCTION(ps_moveto);
PHP_FUNCTION(ps_lineto);
PHP_FUNCTION(ps_curveto);
PHP_FUNCTION(ps_circle);
PHP_FUNCTION(ps_arc);
PHP_FUNCTION(ps_rect);
PHP_FUNCTION(ps_closepath);
PHP_FUNCTION(ps_stroke);
PHP_FUNCTION(ps_closepath_stroke);
PHP_FUNCTION(ps_fill);
PHP_FUNCTION(ps_fill_stroke);
PHP_FUNCTION(ps_closepath_fill_stroke);
PHP_FUNCTION(ps_clip);
PHP_FUNCTION(ps_endpath);
PHP_FUNCTION(ps_setgray_fill);
PHP_FUNCTION(ps_setgray_stroke);
PHP_FUNCTION(ps_setgray);
PHP_FUNCTION(ps_setrgbcolor_fill);
PHP_FUNCTION(ps_setrgbcolor_stroke);
PHP_FUNCTION(ps_setrgbcolor);
PHP_FUNCTION(ps_open_image_file);  /* new parameters: [char *stringpram, int intparam] */
PHP_FUNCTION(ps_open_ccitt);	/* new function */
PHP_FUNCTION(ps_open_image);	/* new function: checkit not yet completeted :( */
PHP_FUNCTION(ps_close_image);
PHP_FUNCTION(ps_place_image);
PHP_FUNCTION(ps_add_bookmark);
PHP_FUNCTION(ps_set_info);
PHP_FUNCTION(ps_attach_file);	/* new function */
PHP_FUNCTION(ps_add_note);	/* new function */
PHP_FUNCTION(ps_add_pdflink);
PHP_FUNCTION(ps_add_locallink);	/* new function */
PHP_FUNCTION(ps_add_launchlink);	/* new function */
PHP_FUNCTION(ps_add_weblink);
PHP_FUNCTION(ps_set_border_style);
PHP_FUNCTION(ps_set_border_color);
PHP_FUNCTION(ps_set_border_dash);
PHP_FUNCTION(ps_setcolor);

/* RJS:
   End of the official PSLIB V3.x API */

/* old font handling */
PHP_FUNCTION(ps_set_font);		/* deprecated */
PHP_FUNCTION(ps_get_font);		/* deprecated */
PHP_FUNCTION(ps_get_fontname);		/* deprecated */
PHP_FUNCTION(ps_get_fontsize);		/* deprecated */

/* old way of starting a PS document */
PHP_FUNCTION(ps_open);			/* deprecated */

/* old stuff for setting infos */
PHP_FUNCTION(ps_set_info_creator);	/* deprecated */
PHP_FUNCTION(ps_set_info_title);	/* deprecated */
PHP_FUNCTION(ps_set_info_subject);	/* deprecated */
PHP_FUNCTION(ps_set_info_author);	/* deprecated */
PHP_FUNCTION(ps_set_info_keywords);	/* deprecated */
PHP_FUNCTION(ps_set_leading);   	/* deprecated */
PHP_FUNCTION(ps_set_text_rendering);	/* deprecated */
PHP_FUNCTION(ps_set_horiz_scaling);	/* deprecated */
PHP_FUNCTION(ps_set_text_rise);		/* deprecated */
PHP_FUNCTION(ps_set_char_spacing);	/* deprecated */
PHP_FUNCTION(ps_set_word_spacing);	/* deprecated */
PHP_FUNCTION(ps_set_transition);	/* deprecated */
PHP_FUNCTION(ps_set_duration);		/* deprecated */
PHP_FUNCTION(ps_get_image_height);	/* deprecated */
PHP_FUNCTION(ps_get_image_width);	/* deprecated */

/* old stuff for opening images */
PHP_FUNCTION(ps_open_jpeg);		/* deprecated */
PHP_FUNCTION(ps_open_tiff);		/* deprecated */
PHP_FUNCTION(ps_open_png);		/* deprecated */
PHP_FUNCTION(ps_open_gif);		/* deprecated */

/* some more stuff for compatibility */
PHP_FUNCTION(ps_add_annotation);
#if HAVE_LIBGD13
PHP_FUNCTION(ps_open_memory_image);
#endif

#ifdef ZTS
#define PSG(v) (ps_globals->v)
#define PSLS_FETCH() php_ps_globals *ps_globals = ts_resource(ps_globals_id)
#else
#define PSG(v) (ps_globals.v)
#define PSLS_FETCH()
#endif


#else
#define ps_module_ptr NULL
#endif
#define phpext_ps_ptr ps_module_ptr
#endif /* PHP_PS_H */
