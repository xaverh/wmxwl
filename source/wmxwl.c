/* wmxwl — floating Wayland compositor in the style of wmx.
 * Derived from tohu (ISC, uint). See COPYING. No wmx source is included.
 */

#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <wayland-util.h>

#include "config.h"
#include "util.h"

enum { NCHANNELS = 9, STACK_STEPS = 256 };

struct wm wm;

static void focus(struct client *c);
static void apply_decor(struct client *c, bool active);
static bool shown(const struct client *c, const struct screen *s);
static struct client *first_shown(const struct screen *s);
static struct client *cycle(struct client *from, int dir);
static void refocus(void);
static void sync_visibility(void);
static void go_channel(uint32_t n);
static void remember_geometry(struct client *c);
static void stack_by(struct swc_window *win, int32_t direction);
static void on_screen_destroy(void *data);
static void on_screen_entered(void *data);
static void on_win_destroy(void *data);
static void on_win_entered(void *data);
static void on_win_metadata(void *data);
static void setup(void);
static void setup_binds(void);

static const struct swc_manager manager = {
	.new_screen = new_screen,
	.new_window = new_window,
	.new_device = new_device,
};

static const struct swc_window_handler window_handler = {
	.destroy = on_win_destroy,
	.title_changed = on_win_metadata,
	.app_id_changed = on_win_metadata,
	.entered = on_win_entered,
};

static const struct swc_screen_handler screen_handler = {
	.destroy = on_screen_destroy,
	.entered = on_screen_entered,
};

/* neuswc paints a solid decor over the inner border, so the frame and
 * label colours are the cue that a window has the pointer. The border
 * is still set, for a neuswc that draws it above the decor. */
static void apply_decor(struct client *c, bool active)
{
	struct swc_decor decor;
	const char *title;

	if (!c || !c->win)
		return;

	if (c->fullscreen) {
		swc_window_set_decor(c->win, NULL);
		swc_window_set_border(c->win, 0, 0, 0, 0);
		return;
	}

	decor = cfg.decor;
	decor.color = active ? cfg.frame_active : cfg.frame_normal;
	decor.title.color = active ? cfg.title_active : cfg.title_normal;
	title = c->win->title;
	if (!title || !title[0])
		title = c->win->app_id;
	if (title && title[0])
		decor.title.string = title;

	swc_window_set_decor(c->win, &decor);
	swc_window_set_border(c->win,
	                      active ? cfg.border_active : cfg.border_normal,
	                      cfg.border_width, 0, 0);
}

static void focus(struct client *c)
{
	if (c == wm.sel_client)
		return;

	if (wm.sel_client)
		apply_decor(wm.sel_client, false);
	if (c)
		apply_decor(c, true);

	swc_window_focus(c ? c->win : NULL);
	wm.sel_client = c;
}

static bool shown(const struct client *c, const struct screen *s)
{
	if (!c || c->hidden)
		return false;
	if (!c->sticky && c->ws != wm.ws)
		return false;
	if (s && c->scr && c->scr != s)
		return false;
	return true;
}

static struct client *first_shown(const struct screen *s)
{
	struct client *c;

	wl_list_for_each(c, &wm.clients, link) {
		if (shown(c, s))
			return c;
	}
	return NULL;
}

static struct client *cycle(struct client *from, int dir)
{
	struct wl_list *start;
	struct wl_list *it;
	struct client *c;

	if (wl_list_empty(&wm.clients))
		return NULL;

	if (!from || !shown(from, wm.sel_screen))
		return first_shown(wm.sel_screen);

	start = dir < 0 ? from->link.prev : from->link.next;
	it = start;
	do {
		if (it == &wm.clients)
			it = dir < 0 ? wm.clients.prev : wm.clients.next;
		if (it == &wm.clients)
			break;
		c = wl_container_of(it, from, link);
		if (shown(c, wm.sel_screen))
			return c;
		it = dir < 0 ? it->prev : it->next;
	} while (it != start);

	return from;
}

static void refocus(void)
{
	struct client *c;

	if (shown(wm.sel_client, NULL))
		return;

	c = first_shown(wm.sel_screen);
	if (!c)
		c = first_shown(NULL);
	focus(c);
}

static void sync_visibility(void)
{
	struct client *c;

	wl_list_for_each(c, &wm.clients, link) {
		if (!c->hidden && (c->sticky || c->ws == wm.ws))
			swc_window_show(c->win);
		else
			swc_window_hide(c->win);
	}
}

static void go_channel(uint32_t n)
{
	if (n < 1 || n > NCHANNELS || n == wm.ws)
		return;

	wm.ws = (uint8_t)n;
	sync_visibility();
	refocus();
}

static void remember_geometry(struct client *c)
{
	struct swc_rectangle geom;

	if (!c || c->maximised || !c->win)
		return;
	if (!swc_window_get_geometry(c->win, &geom))
		return;

	c->x = geom.x;
	c->y = geom.y;
	c->w = geom.width;
	c->h = geom.height;
}

/* swc_window_stack moves one visible window at a time. */
static void stack_by(struct swc_window *win, int32_t direction)
{
	int i;

	if (!win || direction == 0)
		return;
	for (i = 0; i < STACK_STEPS; i++)
		swc_window_stack(win, direction);
}

static uint32_t outward(uint32_t decor, uint32_t border)
{
	return decor > border ? decor : border;
}

static void on_screen_destroy(void *data)
{
	struct screen *s = data;
	struct client *c;

	if (!s)
		return;

	wl_list_remove(&s->link);
	if (wm.sel_screen == s) {
		if (wl_list_empty(&wm.screens))
			wm.sel_screen = NULL;
		else
			wm.sel_screen = wl_container_of(wm.screens.next, s, link);
	}

	wl_list_for_each(c, &wm.clients, link) {
		if (c->scr == s)
			c->scr = wm.sel_screen;
	}
	free(s);
}

static void on_screen_entered(void *data)
{
	struct screen *s = data;

	if (s)
		wm.sel_screen = s;
}

static void on_win_destroy(void *data)
{
	struct client *c = data;

	if (!c)
		return;

	if (wm.grab.active && wm.grab.c == c) {
		wm.grab.active = false;
		wm.grab.resize = false;
		wm.grab.c = NULL;
	}

	if (wm.sel_client == c)
		wm.sel_client = NULL;

	wl_list_remove(&c->link);
	free(c);
	refocus();
}

static void on_win_entered(void *data)
{
	struct client *c = data;

	if (wm.grab.active)
		return;
	if (!shown(c, NULL))
		return;
	focus(c);
}

static void on_win_metadata(void *data)
{
	struct client *c = data;

	if (!c)
		return;
	apply_decor(c, c == wm.sel_client);
}

static void setup_binds(void)
{
	size_t i;

	for (i = 0; i < LENGTH(binds); i++) {
		const struct bind *b = &binds[i];

		if (swc_add_binding(b->type, b->mods, b->ksym, b->fn,
		                    (void *)&b->arg) < 0)
			_log(stderr, "failed to add a binding");
	}
}

static void setup(void)
{
	const char *sock;

	wm.dpy = wl_display_create();
	if (!wm.dpy)
		die(EXIT_FAILURE, "wl_display_create failed");

	wl_list_init(&wm.screens);
	wl_list_init(&wm.clients);
	wm.sel_client = NULL;
	wm.sel_screen = NULL;
	wm.grab.active = false;
	wm.grab.resize = false;
	wm.grab.c = NULL;
	wm.ws = 1;

	wm.ev_loop = wl_display_get_event_loop(wm.dpy);
	if (!swc_initialize(wm.dpy, wm.ev_loop, &manager))
		die(EXIT_FAILURE, "swc_initialize failed");

	setup_binds();

	sock = wl_display_add_socket_auto(wm.dpy);
	if (!sock)
		die(EXIT_FAILURE, "wl_display_add_socket_auto failed");
	setenv("WAYLAND_DISPLAY", sock, 1);
	_log(stderr, "WAYLAND_DISPLAY=%s", sock);

	signal(SIGINT, sig_handler);
	signal(SIGTERM, sig_handler);
	signal(SIGQUIT, sig_handler);
	signal(SIGCHLD, SIG_IGN);
}

void focus_next(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	struct client *c;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;

	c = cycle(wm.sel_client, +1);
	if (!c)
		c = first_shown(NULL);
	focus(c);
}

void focus_prev(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	struct client *c;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;

	c = cycle(wm.sel_client, -1);
	if (!c)
		c = first_shown(NULL);
	focus(c);
}

void raise_sel(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;
	stack_by(wm.sel_client->win, -1);
}

void lower_sel(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;
	stack_by(wm.sel_client->win, +1);
}

void hide_sel(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	struct client *c;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;

	c = wm.sel_client;
	c->hidden = true;
	swc_window_hide(c->win);
	/* Most recently hidden sits at the head, so unhide is LIFO. */
	wl_list_remove(&c->link);
	wl_list_insert(&wm.clients, &c->link);
	refocus();
}

void unhide_last(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	struct client *c;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;

	wl_list_for_each(c, &wm.clients, link) {
		if (!c->hidden)
			continue;
		c->hidden = false;
		c->ws = wm.ws;
		if (wm.sel_screen)
			c->scr = wm.sel_screen;
		swc_window_show(c->win);
		focus(c);
		return;
	}
}

void kill_sel(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;
	swc_window_close(wm.sel_client->win);
}

void toggle_maximise(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	struct client *c;
	struct swc_screen *scr;
	struct swc_rectangle geom;
	uint32_t left, right, top, bottom;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;

	c = wm.sel_client;
	if (c->fullscreen || !c->win)
		return;

	if (c->maximised) {
		c->maximised = false;
		if (c->w > 0 && c->h > 0) {
			geom.x = c->x;
			geom.y = c->y;
			geom.width = c->w;
			geom.height = c->h;
			swc_window_set_geometry(c->win, &geom);
		}
		return;
	}

	scr = NULL;
	if (c->scr)
		scr = c->scr->scr;
	else if (wm.sel_screen)
		scr = wm.sel_screen->scr;
	if (!scr)
		return;

	remember_geometry(c);

	left = outward(cfg.decor.left, cfg.border_width);
	right = outward(cfg.decor.right, cfg.border_width);
	top = outward(cfg.decor.top, cfg.border_width);
	bottom = outward(cfg.decor.bottom, cfg.border_width);
	geom = scr->usable_geometry;
	if (geom.width <= left + right || geom.height <= top + bottom)
		return;

	geom.x += (int32_t)left;
	geom.y += (int32_t)top;
	geom.width -= left + right;
	geom.height -= top + bottom;
	c->maximised = true;
	swc_window_set_geometry(c->win, &geom);
}

void toggle_sticky(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client)
		return;
	wm.sel_client->sticky = !wm.sel_client->sticky;
}

void mouse_move(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;

	if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
		if (!wm.sel_client)
			return;
		wm.grab.active = true;
		wm.grab.resize = false;
		wm.grab.c = wm.sel_client;
		swc_window_begin_move(wm.grab.c->win);
		return;
	}

	if (!wm.grab.active || wm.grab.resize || !wm.grab.c)
		return;
	swc_window_end_move(wm.grab.c->win);
	remember_geometry(wm.grab.c);
	wm.grab.active = false;
	wm.grab.c = NULL;
}

void mouse_resize(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;

	if (state == WL_POINTER_BUTTON_STATE_PRESSED) {
		if (!wm.sel_client)
			return;
		wm.grab.active = true;
		wm.grab.resize = true;
		wm.grab.c = wm.sel_client;
		swc_window_begin_resize(wm.grab.c->win,
		                        SWC_WINDOW_EDGE_RIGHT | SWC_WINDOW_EDGE_BOTTOM);
		return;
	}

	if (!wm.grab.active || !wm.grab.resize || !wm.grab.c)
		return;
	swc_window_end_resize(wm.grab.c->win);
	remember_geometry(wm.grab.c);
	wm.grab.active = false;
	wm.grab.resize = false;
	wm.grab.c = NULL;
}

static struct screen *screen_at(int32_t x, int32_t y)
{
	struct screen *s;

	wl_list_for_each(s, &wm.screens, link) {
		const struct swc_rectangle *g;

		if (!s->scr)
			continue;
		g = &s->scr->geometry;
		if (x >= g->x && y >= g->y &&
		    x < g->x + (int32_t)g->width &&
		    y < g->y + (int32_t)g->height)
			return s;
	}
	return wm.sel_screen;
}

void new_screen(struct swc_screen *scr)
{
	struct screen *s;

	s = malloc(sizeof(*s));
	if (!s)
		die(EXIT_FAILURE, "out of memory");

	s->scr = scr;
	wl_list_insert(&wm.screens, &s->link);
	if (!wm.sel_screen)
		wm.sel_screen = s;
	swc_screen_set_handler(scr, &screen_handler, s);
}

void new_window(struct swc_window *win)
{
	struct client *c;
	int32_t cx = 0;
	int32_t cy = 0;
	struct screen *s;

	c = malloc(sizeof(*c));
	if (!c)
		die(EXIT_FAILURE, "out of memory");

	win->motion_throttle_ms = 1000 / cfg.motion_throttle_hz;
	win->min_width = 1;
	win->min_height = 1;
	win->max_width = 0;
	win->max_height = 0;

	c->win = win;
	c->hidden = false;
	c->sticky = false;
	c->fullscreen = false;
	c->maximised = false;
	c->x = 0;
	c->y = 0;
	c->w = 0;
	c->h = 0;
	c->ws = wm.ws;

	if (swc_cursor_position(&cx, &cy)) {
		cx /= 256;
		cy /= 256;
	}
	s = screen_at(cx, cy);
	c->scr = s ? s : wm.sel_screen;
	if (s)
		wm.sel_screen = s;

	wl_list_insert(&wm.clients, &c->link);
	swc_window_set_handler(win, &window_handler, c);
	swc_window_set_stacked(win);
	swc_window_set_position(win, cx, cy);
	swc_window_show(win);
	focus(c);
}

void new_device(struct libinput_device *dev)
{
	(void)dev;
}

void quit(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;
	wl_display_terminate(wm.dpy);
}

void spawn(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	union arg *a = data;
	char *const *cmd;

	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;
	if (!a || !a->v)
		return;

	cmd = (char *const *)a->v;
	if (fork() == 0) {
		execvp(cmd[0], cmd);
		_exit(127);
	}
}

void channel_goto(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	union arg *a = data;

	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !a)
		return;
	go_channel(a->ui);
}

void channel_moveto(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	union arg *a = data;
	struct client *c;

	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED || !wm.sel_client || !a)
		return;
	if (a->ui < 1 || a->ui > NCHANNELS)
		return;

	c = wm.sel_client;
	if (c->ws == a->ui)
		return;

	c->ws = a->ui;
	if (c->hidden)
		return;
	if (c->sticky || c->ws == wm.ws)
		swc_window_show(c->win);
	else {
		swc_window_hide(c->win);
		refocus();
	}
}

void channel_prev(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	uint32_t n;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;

	n = wm.ws <= 1 ? NCHANNELS : (uint32_t)wm.ws - 1;
	go_channel(n);
}

void channel_next(void *data, uint32_t time, uint32_t value, uint32_t state)
{
	uint32_t n;

	(void)data;
	(void)time;
	(void)value;
	if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
		return;

	n = wm.ws >= NCHANNELS ? 1 : (uint32_t)wm.ws + 1;
	go_channel(n);
}

int main(void)
{
	setup();
	wl_display_run(wm.dpy);
	swc_finalize();
	wl_display_destroy(wm.dpy);
	return EXIT_SUCCESS;
}
