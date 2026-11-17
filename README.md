# wmxwl

A floating Wayland compositor in the style of [wmx](https://github.com/xaverh/wmx).
It is a small C program on top of [neuswc](https://git.sr.ht/~shrub900/neuswc),
starting from the [tohu](https://git.sr.ht/~shrub900/tohu) reference compositor.

wmx's X11 shaped frames do not exist here. The side tab is neuswc's decor
text on the left edge: the title is drawn as stacked glyphs, one per row,
beside a thin frame. The focused window's frame is a darker grey and its
label is black. neuswc paints that solid frame over the one-pixel border,
so the colour change is the focus cue.

## What it does

- Floating windows. Focus follows the pointer and does not raise.
- Side tab with the window title.
- Channels 1–9, wrapping at either end. Hidden windows stay on a list and come back most-recently-hidden first, onto the current channel.
- Opaque move and resize (neuswc interactive move and resize).
- Sticky windows, shown on every channel. There is no sticky glyph.
- Maximise toggles the focused window to the screen's usable area, inside the frame.

## What it does not do

- The X11 pixmap frame (`background.xpm`). Decor is a solid colour.
- The `~/.wmx` command menu. Super+d starts a launcher instead.
- The big channel number. neuswc has no text overlay.
- Xwayland, unless you build neuswc with it. This Makefile does not link xcb.

## Build

Install neuswc and neuwld first, so `pkg-config` finds `swc` and `wld`.
Then:

```
make
sudo make install
```

From a free VT:

```
swc-launch wmxwl
```

`swc-launch` comes with neuswc and must be allowed to open the DRM device
(setuid root, or your seat-management setup).

## Keys

The modifier is Super.

| Key | Action |
|-----|--------|
| Super+Up / Down | Raise / lower |
| Super+Tab / Super+Shift+Tab | Next / previous window |
| Super+Return | Hide |
| Super+Shift+Return | Unhide the last hidden window onto this channel |
| Super+BackSpace | Close |
| Super+Left / Right | Previous / next channel |
| Super+1 … 9 | Jump to a channel |
| Super+Shift+1 … 9 | Move the focused window to a channel |
| Super+Home | Toggle maximise |
| Super+Pause | Toggle sticky |
| Super+n | New terminal |
| Super+d | Launcher (`fuzzel`, `wmenu-run`, or `bemenu-run`) |
| Super+Shift+q | Quit |
| Super+Button1 | Move |
| Super+Button3 | Resize from the bottom-right corner |

Change colours, the tab width, and the commands in [source/config.h](source/config.h).

## Licence

ISC. See [COPYING](COPYING). tohu's copyright is uint's; the wmx-style
behaviour is Xaver Hellauer's.
