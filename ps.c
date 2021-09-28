/*
   +----------------------------------------------------------------------+
   | PHP version 5                                                        |
   +----------------------------------------------------------------------+
   | Copyright (c) 2003-2021 Uwe Steinmann                                |
   +----------------------------------------------------------------------+
   | This source file is subject to version 3.01 of the PHP license,      |
   | that is bundled with this package in the file LICENSE, and is        |
   | available through the world-wide-web at the following url:           |
   | http://www.php.net/license/3_01.txt                                  |
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

#ifdef HAVE_LIBGD
#include "ext/gd/php_gd.h"
#if HAVE_GD_BUNDLED
#include "ext/gd/libgd/gd.h"
#else
#include "gd.h"
#endif
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
ZEND_BEGIN_ARG_INFO_EX(arginfo_void, 0, 0, 0)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_psdoc, 0, 0, 0)
  ZEND_ARG_INFO(0, psdoc)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_open_file, 0, 0, 1)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_begin_page, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_get_value_parameter, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, modifier)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_value_parameter, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, key)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_findfont, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, fontname)
  ZEND_ARG_INFO(0, encoding)
  ZEND_ARG_INFO(0, embed)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setfont, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, font)
  ZEND_ARG_INFO(0, fontsize)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_show, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_show_xy, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_show2, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
  ZEND_ARG_INFO(0, length)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_show_xy2, 0, 0, 5)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
  ZEND_ARG_INFO(0, length)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_continue_text, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_show_boxed, 0, 0, 7)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
  ZEND_ARG_INFO(0, mode)
  ZEND_ARG_INFO(0, feature)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_stringwidth_geometry, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, text)
  ZEND_ARG_INFO(0, font)
  ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_x_y_coord, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setdash, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, black)
  ZEND_ARG_INFO(0, white)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setpolydash, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, darray)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_flat_join_cap_miter, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setlinewidth, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, width)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setoverprintmode, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, mode)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_scale, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-scale)
  ZEND_ARG_INFO(0, y-scale)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_rotate, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, angle)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_curveto, 0, 0, 7)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x1)
  ZEND_ARG_INFO(0, y1)
  ZEND_ARG_INFO(0, x2)
  ZEND_ARG_INFO(0, y2)
  ZEND_ARG_INFO(0, x3)
  ZEND_ARG_INFO(0, y3)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_circle, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, radius)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_arc, 0, 0, 6)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, radius)
  ZEND_ARG_INFO(0, start)
  ZEND_ARG_INFO(0, end)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_arcn, 0, 0, 6)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, radius)
  ZEND_ARG_INFO(0, alpha)
  ZEND_ARG_INFO(0, beta)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_rect, 0, 0, 5)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_open_image_file, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, type)
  ZEND_ARG_INFO(0, filename)
  ZEND_ARG_INFO(0, stringparam)
  ZEND_ARG_INFO(0, intparam)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_open_image, 0, 0, 10)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, type)
  ZEND_ARG_INFO(0, source)
  ZEND_ARG_INFO(0, data)
  ZEND_ARG_INFO(0, length)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
  ZEND_ARG_INFO(0, components)
  ZEND_ARG_INFO(0, bpc)
  ZEND_ARG_INFO(0, params)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_close_image, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, image)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_place_image, 0, 0, 5)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, image)
  ZEND_ARG_INFO(0, x-coor)
  ZEND_ARG_INFO(0, y-coor)
  ZEND_ARG_INFO(0, scale)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_bookmark, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, string)
  ZEND_ARG_INFO(0, parent)
  ZEND_ARG_INFO(0, open)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_info, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, fieldname)
  ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_include_file, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_note, 0, 0, 9)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
  ZEND_ARG_INFO(0, contents)
  ZEND_ARG_INFO(0, title)
  ZEND_ARG_INFO(0, icon)
  ZEND_ARG_INFO(0, open)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_pdflink, 0, 0, 8)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
  ZEND_ARG_INFO(0, filename)
  ZEND_ARG_INFO(0, page)
  ZEND_ARG_INFO(0, dest)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_locallink, 0, 0, 7)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
  ZEND_ARG_INFO(0, page)
  ZEND_ARG_INFO(0, dest)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_launchlink, 0, 0, 6)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
  ZEND_ARG_INFO(0, filename)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_weblink, 0, 0, 6)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
  ZEND_ARG_INFO(0, url)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_border_style, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, style)
  ZEND_ARG_INFO(0, width)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_border_color, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, red)
  ZEND_ARG_INFO(0, green)
  ZEND_ARG_INFO(0, blue)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_set_border_dash, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, black)
  ZEND_ARG_INFO(0, white)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_setcolor, 0, 0, 7)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, type)
  ZEND_ARG_INFO(0, colorspace)
  ZEND_ARG_INFO(0, c1)
  ZEND_ARG_INFO(0, c2)
  ZEND_ARG_INFO(0, c3)
  ZEND_ARG_INFO(0, c4)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_makespotcolor, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, spotcolor)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_begin_pattern, 0, 0, 6)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
  ZEND_ARG_INFO(0, xstep)
  ZEND_ARG_INFO(0, ystep)
  ZEND_ARG_INFO(0, painttype)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_begin_template, 0, 0, 3)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, width)
  ZEND_ARG_INFO(0, height)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_shfill, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, psshading)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_shading, 0, 0, 11)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, type)
  ZEND_ARG_INFO(0, x0)
  ZEND_ARG_INFO(0, y0)
  ZEND_ARG_INFO(0, x1)
  ZEND_ARG_INFO(0, y1)
  ZEND_ARG_INFO(0, c1)
  ZEND_ARG_INFO(0, c2)
  ZEND_ARG_INFO(0, c3)
  ZEND_ARG_INFO(0, c4)
  ZEND_ARG_INFO(0, optlist)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_shading_pattern, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, psshading)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_begin_font, 0, 0, 8)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, fontname)
  ZEND_ARG_INFO(0, a)
  ZEND_ARG_INFO(0, b)
  ZEND_ARG_INFO(0, c)
  ZEND_ARG_INFO(0, d)
  ZEND_ARG_INFO(0, e)
  ZEND_ARG_INFO(0, f)
  ZEND_ARG_INFO(0, optlist)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_begin_glyph, 0, 0, 7)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, wx)
  ZEND_ARG_INFO(0, llx)
  ZEND_ARG_INFO(0, lly)
  ZEND_ARG_INFO(0, urx)
  ZEND_ARG_INFO(0, ury)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_open_memory_image, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, psimage)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_hyphenate, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, word)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_symbol, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, ord)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_symbol_name, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, ord)
  ZEND_ARG_INFO(0, font)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_symbol_width, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, ord)
  ZEND_ARG_INFO(0, font)
  ZEND_ARG_INFO(0, width)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_glyph_show, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_glyph_width, 0, 0, 2)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, name)
  ZEND_ARG_INFO(0, font)
  ZEND_ARG_INFO(0, size)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_glyph_list, 0, 0, 1)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, font)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_kerning, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, glyphname1)
  ZEND_ARG_INFO(0, glyphname2)
  ZEND_ARG_INFO(0, kern)
  ZEND_ARG_INFO(0, font)
ZEND_END_ARG_INFO()
ZEND_BEGIN_ARG_INFO_EX(arginfo_add_ligature, 0, 0, 4)
  ZEND_ARG_INFO(0, psdoc)
  ZEND_ARG_INFO(0, glyphname1)
  ZEND_ARG_INFO(0, glyphname2)
  ZEND_ARG_INFO(0, glyphname3)
  ZEND_ARG_INFO(0, font)
ZEND_END_ARG_INFO()

zend_function_entry ps_functions[] = {
	PHP_FE(ps_new, arginfo_void)
	PHP_FE(ps_delete, arginfo_psdoc)
	PHP_FE(ps_open_file, arginfo_open_file)
	PHP_FE(ps_get_buffer, arginfo_psdoc)
	PHP_FE(ps_close, arginfo_psdoc)
	PHP_FE(ps_begin_page, arginfo_begin_page)
	PHP_FE(ps_end_page, arginfo_psdoc)
	PHP_FE(ps_get_value, arginfo_get_value_parameter)
	PHP_FE(ps_set_value, arginfo_set_value_parameter)
	PHP_FE(ps_get_parameter, arginfo_get_value_parameter)
	PHP_FE(ps_set_parameter, arginfo_set_value_parameter)
	PHP_FE(ps_findfont, arginfo_findfont)
	PHP_FE(ps_setfont, arginfo_setfont)
	PHP_FE(ps_show, arginfo_show)
	PHP_FE(ps_show_xy, arginfo_show_xy)
	PHP_FE(ps_show2, arginfo_show2)
	PHP_FE(ps_show_xy2, arginfo_show_xy2)
	PHP_FE(ps_continue_text, arginfo_continue_text)
	PHP_FE(ps_show_boxed, arginfo_show_boxed)
	PHP_FE(ps_stringwidth, arginfo_stringwidth_geometry)
	PHP_FE(ps_string_geometry, arginfo_stringwidth_geometry) /* not available in pdf extension */
	PHP_FE(ps_set_text_pos, arginfo_x_y_coord)
	PHP_FE(ps_setdash, arginfo_setdash)
	PHP_FE(ps_setpolydash, arginfo_setpolydash)
	PHP_FE(ps_setflat, arginfo_set_flat_join_cap_miter)
	PHP_FE(ps_setlinejoin, arginfo_set_flat_join_cap_miter)
	PHP_FE(ps_setlinecap, arginfo_set_flat_join_cap_miter)
	PHP_FE(ps_setmiterlimit, arginfo_set_flat_join_cap_miter)
	PHP_FE(ps_setlinewidth, arginfo_setlinewidth)
	PHP_FE(ps_setoverprintmode, arginfo_setoverprintmode)
	PHP_FE(ps_save, arginfo_psdoc)
	PHP_FE(ps_restore, arginfo_psdoc)
	PHP_FE(ps_translate, arginfo_x_y_coord)
	PHP_FE(ps_scale, arginfo_scale)
	PHP_FE(ps_rotate, arginfo_rotate)
//	PHP_FE(ps_skew, NULL)
//	PHP_FE(ps_concat, NULL)	/* new function */
	PHP_FE(ps_moveto, arginfo_x_y_coord)
	PHP_FE(ps_lineto, arginfo_x_y_coord)
	PHP_FE(ps_curveto, arginfo_curveto)
	PHP_FE(ps_circle, arginfo_circle)
	PHP_FE(ps_arc, arginfo_arc)
	PHP_FE(ps_arcn, arginfo_arcn)
	PHP_FE(ps_rect, arginfo_rect)
	PHP_FE(ps_closepath, arginfo_psdoc)
	PHP_FE(ps_stroke, arginfo_psdoc)
	PHP_FE(ps_closepath_stroke, arginfo_psdoc)
	PHP_FE(ps_fill, arginfo_psdoc)
	PHP_FE(ps_fill_stroke, arginfo_psdoc)
//	PHP_FE(ps_closepath_fill_stroke, NULL)
	PHP_FE(ps_clip, arginfo_psdoc)
//	PHP_FE(ps_endpath, NULL)
	PHP_FE(ps_open_image_file, arginfo_open_image_file) 
//	PHP_FE(ps_open_ccitt, NULL)
	PHP_FE(ps_open_image, arginfo_open_image)
	PHP_FE(ps_close_image, arginfo_close_image)
	PHP_FE(ps_place_image, arginfo_place_image)
	PHP_FE(ps_add_bookmark, arginfo_add_bookmark)
	PHP_FE(ps_set_info, arginfo_set_info)
//	PHP_FE(ps_attach_file, NULL)
	PHP_FE(ps_include_file, arginfo_include_file)
	PHP_FE(ps_add_note, arginfo_add_note)
	PHP_FE(ps_add_pdflink, arginfo_add_pdflink)
	PHP_FE(ps_add_locallink, arginfo_add_locallink)
	PHP_FE(ps_add_launchlink, arginfo_add_launchlink)
	PHP_FE(ps_add_weblink, arginfo_add_weblink)
	PHP_FE(ps_set_border_style, arginfo_set_border_style)
	PHP_FE(ps_set_border_color, arginfo_set_border_color)
	PHP_FE(ps_set_border_dash, arginfo_set_border_dash)
	PHP_FE(ps_setcolor, arginfo_setcolor)
	PHP_FE(ps_makespotcolor, arginfo_makespotcolor)
	PHP_FE(ps_begin_pattern, arginfo_begin_pattern)
	PHP_FE(ps_end_pattern, arginfo_psdoc)
	PHP_FE(ps_begin_template, arginfo_begin_template)
	PHP_FE(ps_end_template, arginfo_psdoc)
	PHP_FE(ps_shfill, arginfo_shfill)
	PHP_FE(ps_shading, arginfo_shading)
	PHP_FE(ps_shading_pattern, arginfo_shading_pattern)
#ifdef HAVE_PSBEGINFONT
	PHP_FE(ps_begin_font, arginfo_begin_font)
	PHP_FE(ps_end_font, arginfo_psdoc)
	PHP_FE(ps_begin_glyph, arginfo_begin_glyph)
	PHP_FE(ps_end_glyph, arginfo_psdoc)
#endif

#ifdef HAVE_LIBGD
	PHP_FE(ps_open_memory_image, arginfo_open_memory_image)
#endif
	/* Function without an equivalent in pdflib */
	PHP_FE(ps_hyphenate, arginfo_hyphenate)
	PHP_FE(ps_symbol, arginfo_symbol)
	PHP_FE(ps_symbol_name, arginfo_symbol_name)
	PHP_FE(ps_symbol_width, arginfo_symbol_width)
#ifdef HAVE_PSGLYPHSHOW
	PHP_FE(ps_glyph_show, arginfo_glyph_show)
	PHP_FE(ps_glyph_width, arginfo_glyph_width)
	PHP_FE(ps_glyph_list, arginfo_glyph_list)
#endif
#ifdef HAVE_PSBEGINFONT
	PHP_FE(ps_add_kerning, arginfo_add_kerning)
	PHP_FE(ps_add_ligature, arginfo_add_ligature)
#endif

	ZEND_FE_END
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
	PHP_PS_VERSION,
	STANDARD_MODULE_PROPERTIES 
};
/* }}} */

#ifdef COMPILE_DL_PS
ZEND_GET_MODULE(ps)
#endif

/* {{{ _free_ps_doc
 */
static void _free_ps_doc(zend_resource *rsrc TSRMLS_DC)
{
	PSDoc *psdoc = (PSDoc *)rsrc->ptr;
	PS_delete(psdoc);
}
/* }}} */

/* {{{ custom_errorhandler
 */
static void custom_errorhandler(PSDoc *p, int type, const char *shortmsg, void *data)
{
	TSRMLS_FETCH();
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
	TSRMLS_FETCH();
	return(php_write(data, size TSRMLS_CC));
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
	php_info_print_table_header(2, "PS Support", "enabled" );
	php_info_print_table_row(2, "PSlib Version", tmp );
	php_info_print_table_row(2, "Extension Version", PHP_PS_VERSION);
	php_info_print_table_row(2, "Revision", "$Revision$" );
#ifdef HAVE_LIBGD
	php_info_print_table_row(2, "GD Support", "enabled" );
#else
	php_info_print_table_row(2, "GD Support", "disabled" );
#endif
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
	zend_long name_len, value_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zps, &name, &name_len, &value, &value_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t name_len, value_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss", &zps, &name, &name_len, &value, &value_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t name_len;
	PSDoc *ps;
	char *value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zps, &name, &name_len, &zmod)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	/* FIXME: Do we need convert_to_double_ex(zmod)? */
	if(zmod) {
		value = (char *) PS_get_parameter(ps, name, (float) Z_DVAL_P(zmod));
	} else {
		value = (char *) PS_get_parameter(ps, name, 0.0);
	}

	RETURN_STRING(value);
}
/* }}} */

/* {{{ proto void ps_set_value(int psdoc, string key, float value)
   Sets arbitrary value */
PHP_FUNCTION(ps_set_value)
{
	zval *zps;
	char *name;
	size_t name_len;
	double value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsd", &zps, &name, &name_len, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_set_value(ps, name, (float) value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto float ps_get_value(int psdoc, string key[, mixed modifier])
   Gets arbitrary value */
PHP_FUNCTION(ps_get_value)
{
	zval *zps, *zmod=NULL;
	char *name;
	size_t name_len;
	PSDoc *ps;
	double value;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|z", &zps, &name, &name_len, &zmod)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	if(zmod) {
		value = (double) PS_get_value(ps, name, (float) Z_LVAL_P(zmod));
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
	size_t text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t text_len;
	zend_long len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsl", &zps, &text, &text_len, &len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_show2(ps, text, (int)len);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy(int psdoc, string text, double x-coor, double y-coor)
   Output text at position */
PHP_FUNCTION(ps_show_xy) 
{
	zval *zps;
	char *text;
	size_t text_len;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdd", &zps, &text, &text_len, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_show_xy(ps, text, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_show_xy2(int psdoc, string text, int len, double x-coor, double y-coor)
   Output text at position */
PHP_FUNCTION(ps_show_xy2) 
{
	zval *zps;
	char *text;
	size_t text_len;
  zend_long len;
	double x, y;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsldd", &zps, &text, &text_len, &len, &x, &y)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_show_xy2(ps, text, (int)len, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_findfont(int psdoc, string fontname, string encoding [, bool embed])
   Prepares the font fontname for later use with ps_setfont() */
PHP_FUNCTION(ps_findfont) {
	zval *zps;
	zend_bool embed = 0;
	char *fontname, *encoding;
	size_t fontname_len, encoding_len;
	PSDoc *ps;
	int font;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|b", &zps, &fontname, &fontname_len, &encoding, &encoding_len, &embed)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	zend_long font;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rld", &zps, &font, &fontsize)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setfont(ps, (int)font, (float) fontsize);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setlinewidth(ps, (float) width);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setoverprintmode(int psdoc, long mode)
   Sets overprint mode */
PHP_FUNCTION(ps_setoverprintmode)
{
	zval *zps;
	zend_long mode;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &mode)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setoverprintmode(ps, (int)mode);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_scale(ps, (float) x, (float) y);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinejoin(int psdoc, int value)
   Sets linejoin parameter */
PHP_FUNCTION(ps_setlinejoin) 
{
	zval *zps;
	zend_long value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setlinejoin(ps, (int)value);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_setlinecap(int psdoc, int value)
   Sets linecap parameter */
PHP_FUNCTION(ps_setlinecap) 
{
	zval *zps;
	zend_long value;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &value)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setlinecap(ps, (int)value);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_curveto(ps, (float) x1, (float) y1, (float) x2, (float) y2, (float) x3, (float) y3);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_show_boxed(int psdoc, string text, double x-coor, double y-coor, double width, double height, string mode [, string feature])
   Output text formated in a boxed */
PHP_FUNCTION(ps_show_boxed) 
{
	zval *zps;
	double x, y, width, height;
	char *text, *feature = NULL, *mode;
	size_t text_len, feature_len, mode_len;
	int nr;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdddds|s", &zps, &text, &text_len, &x, &y, &width, &height, &mode, &mode_len, &feature, &feature_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t text_len;
	double width, size = 0.0;
	zend_long font = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ld", &zps, &text, &text_len, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	width = (double) PS_stringwidth2(ps, text, text_len, (int)font, (float)size);

	RETURN_DOUBLE((double) width);
}
/* }}} */

/* {{{ proto array ps_string_geometry(int psdoc, string text [, int font, double size])
   Returns geometry of text in current font */
PHP_FUNCTION(ps_string_geometry)
{
	zval *zps;
	char *text;
	size_t text_len;
	double width, size = 0.0;
	zend_long font = 0;
	float dimension[3];
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ld", &zps, &text, &text_len, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	width = (double) PS_string_geometry(ps, text, text_len, (int)font, (float) size, dimension);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t text_len;
	zend_long parentid = 0, open = 0, id;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ll", &zps, &text, &text_len, &parentid, &open)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	/* will never return 0 */
	id = PS_add_bookmark(ps, text, (int)parentid, (int)open);

	RETURN_LONG(id);
}
/* }}} */

/* {{{ proto int ps_open_image_file(int ps, string type, string filename[, string stringparam, int intparam])
   Opens an image file of the given type and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_image_file)
{
	zval *zps;
	char *type, *filename, *image, *stringparam = NULL;
	size_t type_len, filename_len, stringparam_len;
	long imageid;
	zend_long intparam = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rss|sl", &zps, &type, &type_len, &filename, &filename_len, &stringparam, &stringparam_len, &intparam)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

#ifdef VIRTUAL_DIR
	virtual_filepath(filename, &image TSRMLS_CC);
#else
	image = filename;
#endif

	imageid = PS_open_image_file(ps, type, image, stringparam, (int)intparam);

	if (imageid == 0) {
		RETURN_FALSE;
	}
	RETURN_LONG(imageid);
}
/* }}} */

#ifdef HAVE_LIBGD
/* {{{ proto int ps_open_memory_image(int ps, int image)
   Takes an GD image and returns an image for placement in a PS document */
PHP_FUNCTION(ps_open_memory_image)
{
	zval *zps;
	zval *zgd;
	zend_class_entry *gd_image_ce;
	int i, j, color, count;
	int imageid;
	gdImagePtr im;
	unsigned char *buffer, *ptr;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rO", &zps, &zgd, gd_image_ce)) {
		return;
	}
	
	PSDOC_FROM_ZVAL(ps, zps);

	im = php_gd_libgdimageptr_from_zval_p(zgd);

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

	imageid = PS_open_image(ps, "memory", "memory", (const char *) buffer, im->sx*im->sy*3, im->sx, im->sy, 3, 8, NULL);
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
#endif /* HAVE_LIBGD */

/* {{{ proto void ps_close_image(int ps, int psimage)
   Closes the PS image */
PHP_FUNCTION(ps_close_image)
{
	zval *zps;
	long imageid;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &imageid)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_close_image(ps, (int)imageid);
}
/* }}} */

/* {{{ proto void ps_place_image(int ps, int psimage, double x-coor, double y-coor, double scale)
   Places image in the PS document */
PHP_FUNCTION(ps_place_image)
{
	zval *zps;
	long imageid;
	double x, y, scale;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rlddd", &zps, &imageid, &x, &y, &scale)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_place_image(ps, (int)imageid, (float) x, (float) y, (float) scale);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_shading(int ps, string type, double x0, double y0, double x1, double y1, double c1, double c2, double c3, double c4, string optlist);
 * Define a shading from the current fill color to another color. */
PHP_FUNCTION(ps_shading)
{
	zval *zps;
	PSDoc *ps;
	char *type, *optlist;
	size_t tlen, olen;
	double x0, y0, x1, y1, c1, c2, c3, c4;
	long shadingid = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdddddddds", &zps, &type, &tlen, &x0, &y0, &x1, &y1, &c1, &c2, &c3, &c4, &optlist, &olen)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	shadingid = PS_shading(ps, type, (float) x0, (float) y0, (float) x1, (float) y1, (float) c1, (float) c2, (float) c3, (float) c4, optlist);

	RETURN_LONG(shadingid);
}
/* }}} */

/* {{{ proto void ps_shfill(int ps, int psshading)
   Fill an area with a gradient fill */
PHP_FUNCTION(ps_shfill)
{
	zval *zps;
	zend_long shadingid;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &shadingid)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_shfill(ps, (int)shadingid);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_shading_pattern(int ps, int psshading)
   Create a shading pattern */
PHP_FUNCTION(ps_shading_pattern)
{
	zval *zps;
	zend_long patternid, shadingid;
	char *optlist;
	size_t olen;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rls", &zps, &shadingid, &optlist, &olen)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	patternid = PS_shading_pattern(ps, (int)shadingid, optlist);

	RETURN_LONG(patternid);
}
/* }}} */

/* {{{ proto void ps_add_weblink(int psdoc, double llx, double lly, double urx, double ury, string url)
   Adds link to web resource */
PHP_FUNCTION(ps_add_weblink)
{
	zval *zps;
	double llx, lly, urx, ury;
	char *url;
	size_t url_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddds", &zps, &llx, &lly, &urx, &ury, &url, &url_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t filename_len, dest_len;
	zend_long page;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsls", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len, &page, &dest, &dest_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_pdflink(ps, (float) llx, (float) lly, (float) urx, (float) ury, filename, (int)page, dest);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_set_border_style(int psdoc, string style, double width)
   Sets style of box surounding all kinds of annotations and link */
PHP_FUNCTION(ps_set_border_style)
{
	zval *zps;
	char *style;
	size_t style_len;
	double width;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsd", &zps, &style, &style_len, &width)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &black, &white)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t title_len, text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddss", &zps, &llx, &lly, &urx, &ury, &title, &title_len, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

//	ZEND_REGISTER_RESOURCE(return_value, ps, le_psdoc);
	RETURN_RES(zend_register_resource(ps, le_psdoc));
}
/* }}} */

/* {{{ proto void ps_delete(int psdoc)
   Deletes the PS object */
PHP_FUNCTION(ps_delete) {
	zval *zps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}


//	zend_list_delete(Z_LVAL_P(zps));
	zend_list_delete(Z_RES_P(zps));

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_open_file(int psdoc [, char filename])
   Opens a new PS document. If filename is NULL, document is created in memory. This is not yet fully supported */
PHP_FUNCTION(ps_open_file) {
	zval *zps;
	char *filename = NULL;
	size_t filename_len;
	int ps_file;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z|s", &zps, &filename, &filename_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

	buffer = PS_get_buffer(ps, &size);

	RETURN_STRINGL((char *)buffer, size);
}

/* }}} */

/* {{{ proto void ps_setpolydash(int psdoc, double darray)
   Sets more complicated dash pattern */ 
PHP_FUNCTION(ps_setpolydash) {
	zval *zps, *arg2, *tmp;
	int len, i;
	float *darray;
	PSDoc *ps;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "zz", &zps, &arg2) == FAILURE) {
		WRONG_PARAM_COUNT;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	len = zend_hash_num_elements(Z_ARRVAL_P(arg2));

	if (NULL == (darray = emalloc(len * sizeof(double)))) {
		RETURN_FALSE;
	}

	i = 0;
	ZEND_HASH_FOREACH_VAL(Z_ARRVAL_P(arg2), tmp) {
		if (Z_TYPE_P(tmp) == IS_DOUBLE) {
			darray[i] = (float) Z_DVAL_P(tmp);
		} else if (Z_TYPE_P(tmp) == IS_LONG) {
			darray[i] = (float) Z_LVAL_P(tmp);
		} else {
			php_error_docref(NULL TSRMLS_CC, E_WARNING,"PSlib set_polydash: illegal darray value");
		}
		i++;
	}  ZEND_HASH_FOREACH_END();

	/* Old PHP5 code
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
	} */

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

	PSDOC_FROM_ZVAL(ps, zps);

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

//	ZEND_FETCH_RESOURCE(ps, PSDoc *, arg1, -1, "ps document", le_psdoc);
	if ((ps = (PSDoc *) zend_fetch_resource(Z_RES_P(arg1), "ps document", le_psdoc)) == NULL) { RETURN_FALSE; }

	convert_to_string_ex(arg2);
#ifdef VIRTUAL_DIR
	virtual_filepath(Z_STRVAL_PP(arg2), &image TSRMLS_CC);
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
	size_t type_len, params_len, source_len, data_len;
	zend_long length, width, height, components, bpc;
	long imageid;
	char *image;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsssllllls", &zps, &type, &type_len, &source, &source_len, &data, &data_len, &length, &width, &height, &components, &bpc, &params, &params_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

#ifdef VIRTUAL_DIR
	virtual_filepath(data, &image TSRMLS_CC);
#else
	image = data;
#endif

	imageid = PS_open_image(ps, type, source, image, (int)length, (int)width, (int)height, (int)components, (int)bpc, params);

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
	size_t filename_len, description_len, author_len, mimetype_len, icon_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsssss", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len, &description, &description_len, &author, &author_len, &mimetype, &mimetype_len, &icon, &icon_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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

/* {{{ proto void ps_include_file(int psdoc, string filename)
   Reads an external file with raw PostScript code */
PHP_FUNCTION(ps_include_file) {
	zval *zps;
	char *filename;
	size_t filename_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &filename, &filename_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_include_file(ps, filename);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_note(int psdoc, double llx, double lly, double urx, double ury, string contents, string title, string icon, int open)
   Sets annotation */
PHP_FUNCTION(ps_add_note) {
	zval *zps;
	double llx, lly, urx, ury;
	char *contents, *title, *icon;
	size_t contents_len, title_len, icon_len;
	zend_long open;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddsssl", &zps, &llx, &lly, &urx, &ury, &contents, &contents_len, &title, &title_len, &icon, &icon_len, &open)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_note(ps, (float) llx, (float) lly, (float) urx, (float) ury, contents, title, icon, (int)open);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_locallink(int psdoc, double llx, double lly, double urx, double ury, int page, string dest)
   Adds link to web resource */
PHP_FUNCTION(ps_add_locallink) {
	zval *zps;
	double llx, lly, urx, ury;
	char *dest;
	size_t dest_len;
	zend_long page;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddls", &zps, &llx, &lly, &urx, &ury, &page, &dest, &dest_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_locallink(ps, (float) llx, (float) lly, (float) urx, (float) ury, (int)page, dest);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_launchlink(int psdoc, double llx, double lly, double urx, double ury, string filename)
   Adds link to web resource */
PHP_FUNCTION(ps_add_launchlink) {
	zval *zps;
	double llx, lly, urx, ury;
	char *filename;
	size_t filename_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdddds", &zps, &llx, &lly, &urx, &ury, &filename, &filename_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	size_t type_len, colorspace_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssdddd", &zps, &type, &type_len, &colorspace, &colorspace_len, &c1, &c2, &c3, &c4)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setcolor(ps, type, colorspace, (float) c1, (float) c2, (float) c3, (float) c4);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto int ps_makespotcolor(int ps, string spotname);
 * Make a named spot color from the current color. */
PHP_FUNCTION(ps_makespotcolor) {
	zval *zps;
	char *spotname;
	size_t spotname_len;
	int spot;
	zend_long reserved = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|l", &zps, &spotname, &spotname_len, &reserved)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	spot = PS_makespotcolor(ps, spotname, (int)reserved);

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

	PSDOC_FROM_ZVAL(ps, zps);

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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_initgraphics(ps);

	RETURN_TRUE;
} /* }}} */

/* {{{ proto void ps_add_thumbnail(int ps, int image);
 * Add an existing image as thumbnail for the current page. */
PHP_FUNCTION(ps_add_thumbnail) {
	zval *zps;
	long imageid;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &imageid)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_thumbnail(ps, (int)imageid);

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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_setmatrix(ps, (float) a, (float) b, (float) c, (float) d, (float) e, (float) f);

	RETURN_TRUE;
} /* }}} */
#endif

/* {{{ proto int ps_begin_pattern(int psdoc, double width, double height, double xstep, double ystep, int painttype)
   Starts pattern */
PHP_FUNCTION(ps_begin_pattern) 
{
	zval *zps;
	double width, height, xstep, ystep;
	zend_long painttype;
	PSDoc *ps;
	int patid;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rddddl", &zps, &width, &height, &xstep, &ystep, &painttype)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	patid = PS_begin_pattern(ps, (float) width,
	                     (float) height,
	                     (float) xstep,
	                     (float) ystep,
	                     (int) painttype);
	RETURN_LONG(patid);
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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_end_pattern(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_begin_template(int psdoc, double width, double height)
   Starts template */
PHP_FUNCTION(ps_begin_template) 
{
	zval *zps;
	double width, height;
	PSDoc *ps;
	int tempid;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rdd", &zps, &width, &height)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	tempid = PS_begin_template(ps, (float) width, (float) height);

	RETURN_LONG(tempid);
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

	PSDOC_FROM_ZVAL(ps, zps);

	PS_end_template(ps);

	RETURN_TRUE;
}
/* }}} */

/* Function without an equivalent in pdflib */

#ifdef HAVE_PSBEGINFONT
/* {{{ proto int ps_begin_font(int psdoc, string fontname, double a, double b, double c, double d, double e, double f [, array optlist])
   Starts pattern */
PHP_FUNCTION(ps_begin_font) 
{
	zval *zps;
	char *optlist = NULL, *fontname;
	size_t olen, flen;
	double a, b, c, d, e, f;
	PSDoc *ps;
	int fontid;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsdddddd|s", &zps, &fontname, &flen, &a, &b, &c, &d, &e, &f, &optlist, &olen)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	fontid = PS_begin_font(ps,
											 fontname,
											 0,
	                     (double) a,
	                     (double) b,
	                     (double) c,
	                     (double) d,
	                     (double) e,
	                     (double) f,
	                     optlist);
	RETURN_LONG(fontid);
}
/* }}} */

/* {{{ proto void ps_end_font(int psdoc)
   Ends font */
PHP_FUNCTION(ps_end_font) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_end_font(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto int ps_begin_glyph(int psdoc, string glyphname, double wx, double llx, double lly, double urx, double ury)
   Starts pattern */
PHP_FUNCTION(ps_begin_glyph) 
{
	zval *zps;
	char *glyphname;
	size_t glen;
	double wx, llx, lly, urx, ury;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsddddd", &zps, &glyphname, &glen, &wx, &llx, &lly, &urx, &ury)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_begin_glyph(ps,
								 glyphname,
	               (double) wx,
	               (double) llx,
	               (double) lly,
	               (double) urx,
	               (double) ury);
	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_end_glyph(int psdoc)
   Ends glyph */
PHP_FUNCTION(ps_end_glyph) 
{
	zval *zps;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r", &zps)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_end_glyph(ps);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_kerning(int psdoc, string glyphname1, string glyphname2, int kern [, int font])
   Adds a new kerning pair to the font */
PHP_FUNCTION(ps_add_kerning) 
{
	zval *zps;
	PSDoc *ps;
	char *glyphname1, *glyphname2;
	size_t glen1, glen2;
	zend_long font = 0;
	zend_long kern = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rssl|l", &zps, &glyphname1, &glen1, &glyphname2, &glen2, &kern, &font)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_kerning(ps, (int)font, glyphname1, glyphname2, (int)kern);

	RETURN_TRUE;
}
/* }}} */

/* {{{ proto void ps_add_ligature(int psdoc, string glyphname1, string glyphname2, string glyphname3 [, int font])
   Adds a new ligature to the font */
PHP_FUNCTION(ps_add_ligature) 
{
	zval *zps;
	PSDoc *ps;
	char *glyphname1, *glyphname2, *glyphname3;
	size_t glen1, glen2, glen3;
	zend_long font = 0;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rsss|l", &zps, &glyphname1, &glen1, &glyphname2, &glen2, &glyphname3, &glen3, &font)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_add_ligature(ps, (int)font, glyphname1, glyphname2, glyphname3);

	RETURN_TRUE;
}
/* }}} */
#endif

/* {{{ proto void ps_hyphenate(int ps, string word)
   Hyphenate a given word */
PHP_FUNCTION(ps_hyphenate) {
	zval *zps;
	char *text;
	size_t text_len;
	char *buffer;
	unsigned int i, j;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

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
	zend_long ord;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl", &zps, &ord)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_symbol(ps, (unsigned char) ord);

	RETURN_TRUE;

} /* }}} */

/* {{{ proto double ps_symbol_width(int psdoc, int ord [, int font, double width])
   Returns width of a glyph in current font */
PHP_FUNCTION(ps_symbol_width)
{
	zval *zps;
	zend_long ord;
	double width, size = 0.0;
	zend_long font = 0;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|ld", &zps, &ord, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	width = (double) PS_symbol_width(ps, (unsigned char) ord, (int)font, (float) size);

	RETURN_DOUBLE(width);
}
/* }}} */

/* {{{ proto string ps_symbol_name(int psdoc, int ord [, int font])
   Returns name of a glyph in current font */
PHP_FUNCTION(ps_symbol_name)
{
	zval *zps;
	zend_long ord;
	zend_long font = 0;
	char glyphname[50];
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rl|l", &zps, &ord, &font)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_symbol_name(ps, (unsigned char) ord, (int)font, glyphname, 50);

	RETURN_STRING(glyphname);
}
/* }}} */

#ifdef HAVE_PSGLYPHSHOW
/* {{{ proto void ps_glyhp_show(int ps, string name)
   Output glyph by its name */
PHP_FUNCTION(ps_glyph_show) {
	zval *zps;
	char *text;
	size_t text_len;
	PSDoc *ps;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs", &zps, &text, &text_len)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	PS_glyph_show(ps, text);

} /* }}} */

/* {{{ proto void ps_glyhp_width(int ps, string name [, int font, double size])
   Return width of glyph by its name */
PHP_FUNCTION(ps_glyph_width) {
	zval *zps;
	char *text;
	size_t text_len;
	double width;
	PSDoc *ps;
	zend_long font;
	double size;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "rs|ld", &zps, &text, &text_len, &font, &size)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	width = PS_glyph_width(ps, text, (int)font, size);

	RETURN_DOUBLE(width);

} /* }}} */

/* {{{ proto array ps_glyhp_list(int ps, [, int font])
   Return array of glyph names */
PHP_FUNCTION(ps_glyph_list) {
	zval *zps;
	PSDoc *ps;
	zend_long font;
	char **glyphlist;
	int i, listlen;

	if (FAILURE == zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "r|l", &zps, &font)) {
		return;
	}

	PSDOC_FROM_ZVAL(ps, zps);

	if(PS_glyph_list(ps, (int)font, &glyphlist, &listlen)) {
		array_init(return_value);
		for(i=0; i<listlen; i++) {
			add_index_string(return_value, i, glyphlist[i]);
		}
	} else {
		RETURN_FALSE;
	}
} /* }}} */
#endif

#endif /* 0 */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
