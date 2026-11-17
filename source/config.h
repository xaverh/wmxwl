#ifndef CONFIG_H
#define CONFIG_H

#include <xkbcommon/xkbcommon-keysyms.h>

#include "types.h"
#include "frame.h"
#include "wmxwl.h"

/* Unfocused windows show only the left tab. Focus adds the other edges.
 * Both use the wmx frame texture. neuswc stacks the title; it cannot
 * rotate it the way Xft does. */
static const struct config cfg = {
	.motion_throttle_hz = 85,
	.border_active = 0xff000000,
	.border_normal = 0xff000000,
	.border_width = 0,
	.frame_active = 0xffcccccc, /* gray80, wmx tab background */
	.frame_normal = 0xfff2f2f2, /* gray95, wmx frame background */
	.title_active = 0xff000000,
	.title_normal = 0xff000000,
	.decor = {
		.color = 0xffe6e6e6,
		.top = WMX_EDGE,
		.right = WMX_EDGE,
		.bottom = WMX_EDGE,
		.left = WMX_TAB,
		.parts = &wmx_parts,
		.title = {
			.enabled = true,
			.edge = SWC_DECOR_EDGE_LEFT,
			.align = SWC_DECOR_ALIGN_START,
			.color = 0xff000000,
			.padding = 2,
			.font = "Bitstream Vera Sans:size=12:weight=demibold",
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
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_End,     { .ui = MAX_W | MAX_H }, set_span },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Prior,   { .ui = MAX_H | MAX_ON }, set_span },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_Next,    { .ui = MAX_H },        set_span },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_KP_Add,  { .ui = MAX_W | MAX_ON }, set_span },
	{ SWC_BINDING_KEY,    MOD4,        XKB_KEY_KP_Subtract, { .ui = MAX_W },    set_span },
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
	{ SWC_BINDING_BUTTON, 0,           BTN_LEFT,        { .v = NULL },    frame_button },
	{ SWC_BINDING_BUTTON, 0,           BTN_MIDDLE,      { .v = NULL },    frame_button },
	{ SWC_BINDING_BUTTON, 0,           BTN_RIGHT,       { .v = NULL },    frame_button },
};

#endif
