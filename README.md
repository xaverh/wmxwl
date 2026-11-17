# wmxwl

A floating Wayland compositor in the style of [wmx](https://github.com/xaverh/wmx).
It is a small C program on top of [neuswc](https://git.sr.ht/~shrub900/neuswc),
starting from the [tohu](https://git.sr.ht/~shrub900/tohu) reference compositor.

The frame is wmx's `background.xpm` texture with a one-pixel black outline.
An unfocused window shows only the left tab. Focusing it adds the 8px frame
on the other three sides, which is the wmx active/inactive split.

neuswc cannot shape a window, so the tab is a full-height strip rather than
wmx's short tab with a diagonal cut, and the title is stacked glyphs rather
than letters rotated by Xft. The typeface is the wmx default: Bitstream Vera
Sans, 12px, demibold, black.

## What it does

- Floating windows. Focus follows the pointer and does not raise.
- Drag the tab to move. On a focused window, drag the bottom-right corner to resize.
- Right-click the frame or the empty desktop to circulate. Middle-click the frame for the next channel. Middle-click the desktop for a launcher. The wheel on the frame changes channel.
- Channels 1–9, wrapping at either end. Hidden windows come back most-recently-hidden first, onto the current channel.
- Sticky windows, shown on every channel. There is no sticky glyph.
- Maximise, full height, and full width, inset by the frame.

## What it does not do

- The shaped outline, the diagonal tab end, or rotated title text.
- The `~/.wmx` command menu and the client menu on the tab button. Middle-click on the desktop starts a launcher instead.
- The big green channel number. neuswc has no text overlay.
- Xwayland, unless you build neuswc with it. This Makefile does not link xcb.

Clicking a window's interior still raises it. That raise is inside neuswc, on the button path, and wmxwl does not do it for focus or for the tab.

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

The modifier is Super, as in current wmx.

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
| Super+End | Restore size |
| Super+Page Up / Page Down | Full height / restore height |
| Super+Keypad + / − | Full width / restore width |
| Super+Pause | Toggle sticky |
| Super+n | New terminal |
| Super+d | Launcher (`fuzzel`, `wmenu-run`, or `bemenu-run`) |
| Super+Shift+q | Quit |
| Super+Button1 | Move from anywhere |
| Super+Button3 | Resize from the bottom-right, from anywhere |

Change the tab width, frame width, and commands in [source/config.h](source/config.h).

## Licence

ISC. See [COPYING](COPYING). tohu's copyright is uint's. The frame texture
is from wmx's `background.xpm`, MIT, Chris Cannam and others.
