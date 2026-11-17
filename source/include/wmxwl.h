#ifndef WMXWL_H
#define WMXWL_H

#include <stdint.h>

#include "types.h"

void focus_next(void *data, uint32_t time, uint32_t value, uint32_t state);
void focus_prev(void *data, uint32_t time, uint32_t value, uint32_t state);
void raise_sel(void *data, uint32_t time, uint32_t value, uint32_t state);
void lower_sel(void *data, uint32_t time, uint32_t value, uint32_t state);
void hide_sel(void *data, uint32_t time, uint32_t value, uint32_t state);
void unhide_last(void *data, uint32_t time, uint32_t value, uint32_t state);
void kill_sel(void *data, uint32_t time, uint32_t value, uint32_t state);
void toggle_maximise(void *data, uint32_t time, uint32_t value, uint32_t state);
void toggle_sticky(void *data, uint32_t time, uint32_t value, uint32_t state);
void mouse_move(void *data, uint32_t time, uint32_t value, uint32_t state);
void mouse_resize(void *data, uint32_t time, uint32_t value, uint32_t state);
void new_screen(struct swc_screen *scr);
void new_window(struct swc_window *win);
void new_device(struct libinput_device *dev);
void quit(void *data, uint32_t time, uint32_t value, uint32_t state);
void spawn(void *data, uint32_t time, uint32_t value, uint32_t state);
void channel_goto(void *data, uint32_t time, uint32_t value, uint32_t state);
void channel_moveto(void *data, uint32_t time, uint32_t value, uint32_t state);
void channel_prev(void *data, uint32_t time, uint32_t value, uint32_t state);
void channel_next(void *data, uint32_t time, uint32_t value, uint32_t state);
extern struct wm wm;

#endif
