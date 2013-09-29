/* Copyright (c) 2013, Eyal Birger
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * The name of the author may not be used to endorse or promote products
 *       derived from this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "util/tmalloc.h"
#include "util/event.h"
#include "util/debug.h"
#include "main/console.h"
#include "js/js_obj.h"
#include "drivers/graphics/text.h"
#include "drivers/graphics/circle.h"
#include "drivers/graphics/js_painter.h"

int do_graphics_circle_draw(obj_t **ret, obj_t *this, int argc, obj_t *argv[])
{
    int x, y, radius;

    tp_assert(argc == 4);
    x = obj_get_int(argv[1]);
    y = obj_get_int(argv[2]);
    radius = obj_get_int(argv[3]);

    circle_draw(x, y, radius, js_painter_pixel_draw, js_painter_ctx(this));
    return 0;
}

int do_graphics_string_draw(obj_t **ret, obj_t *this, int argc, obj_t *argv[])
{
    int x, y;
    string_t *s;

    tp_assert(argc == 4);
    x = obj_get_int(argv[1]);
    y = obj_get_int(argv[2]);
    s = to_string(argv[3]);

    string_draw(x, y, &s->value, js_painter_pixel_draw, js_painter_ctx(this));
    return 0;
}

int do_graphics_constructor(obj_t **ret, obj_t *this, int argc, obj_t *argv[])
{
    tp_assert(argc == 2);

    *ret = object_new();
    obj_inherit(*ret, argv[0]);
    js_painter_init(*ret, argv[1]);
    return 0;
}
