#ifndef CONFIG_H
#define CONFIG_H

#include <xkbcommon/xkbcommon-keysyms.h>

#include "types.h"
#include "wmxwl.h"

/* Side tab: the left edge is the tab. neuswc draws the title there as
 * stacked glyphs. The other edges are the thin frame. */
static const struct config cfg = {
	.motion_throttle_hz = 85,
	.border_active = 0xff000000,
	.border_normal = 0xff808080,
	.border_width = 1,
	.frame_active = 0xffcccccc, /* gray80, wmx tab background */
	.frame_normal = 0xffe6e6e6,
	.title_active = 0xff000000,
	.title_normal = 0xff404040,
	.decor = {
		.color = 0xffcccccc,
		.top = 8,
		.right = 8,
		.bottom = 8,
		.left = 22,
		.parts = NULL,
		.title = {
			.enabled = true,
			.edge = SWC_DECOR_EDGE_LEFT,
			.align = SWC_DECOR_ALIGN_START,
			.color = 0xff000000,
			.padding = 4,
			.font = "Bitstream Vera Sans:size=11",
		},
	},
};

/* First program that exists wins. */
static const char *termcmd[] = {
	"sh", "-c",
	"command -v foot >/dev/null && exec foot; "
	"command -v x-terminal-emulator >/dev/null && exec x-terminal-emulator; "
	"exec xterm",
	NULL
};

static const char *menucmd[] = {
	"sh", "-c",
	"command -v fuzzel >/dev/null && exec fuzzel; "
	"command -v wmenu-run >/dev/null && exec wmenu-run; "
	"command -v bemenu-run >/dev/null && exec bemenu-run; "
	"exit 1",
	NULL
};

static struct bind binds[] = {
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_n,       { .v = termcmd }, spawn },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_d,       { .v = menucmd }, spawn },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_q,       { .v = NULL },    quit },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Tab,     { .v = NULL },    focus_next },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_Tab,     { .v = NULL },    focus_prev },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Up,      { .v = NULL },    raise_sel },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Down,    { .v = NULL },    lower_sel },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Return,  { .v = NULL },    hide_sel },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_Return,  { .v = NULL },    unhide_last },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_BackSpace, { .v = NULL },  kill_sel },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Home,    { .v = NULL },    toggle_maximise },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Pause,   { .v = NULL },    toggle_sticky },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Left,    { .v = NULL },    channel_prev },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Right,   { .v = NULL },    channel_next },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_1,       { .ui = 1 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_2,       { .ui = 2 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_3,       { .ui = 3 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_4,       { .ui = 4 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_5,       { .ui = 5 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_6,       { .ui = 6 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_7,       { .ui = 7 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_8,       { .ui = 8 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_9,       { .ui = 9 },      channel_goto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_1,       { .ui = 1 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_2,       { .ui = 2 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_3,       { .ui = 3 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_4,       { .ui = 4 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_5,       { .ui = 5 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_6,       { .ui = 6 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_7,       { .ui = 7 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_8,       { .ui = 8 },      channel_moveto },
	{ SWC_BINDING_KEY,    MOD4|SHFT,   XKB_KEY_9,       { .ui = 9 },      channel_moveto },
	{ SWC_BINDING_BUTTON, MOD4,        BTN_LEFT,        { .v = NULL },    mouse_move },
	{ SWC_BINDING_BUTTON, MOD4,        BTN_RIGHT,       { .v = NULL },    mouse_resize },
};

#endif
