#if 0
# self-compiling/-running c code...
# 	unsafe?

tmp=$(mktemp)
(
	tail -n +2 |
		cc -DNDEBUG -lX11 -o $tmp -x c -;
	$tmp;
	rm $tmp;
	exit;
) << //EOF
#endif

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xutil.h>

#define FG 0xFFFF9900
#define BG 0xFF222222
#define MAX_LINES 0x1000

#ifndef NDEBUG
#define debug printf
#else
#define debug(...) ;
#endif

int width = 320;
int height = 240;

int running = 1;
Display *dpy;
int screen;
Window win;
GC gc;
Atom WM_DELETE_WINDOW;

XPoint line[MAX_LINES];
int li = 0;
int lM = MAX_LINES-1;
struct {
	XPoint d;
	short m;
} vel = { {1,1}, 2};
struct timespec cur_time;
double delta;

void
handle_event(XEvent *ev) {
	switch(ev->type) {
	case KeyPress:
		debug("keycode: %d\n", ev->xkey.keycode);
		if(ev->xkey.keycode == 53)
			running = 0;
		break;
	case Expose:
		break;
	case ClientMessage:
		if(ev->xclient.data.l[0] == WM_DELETE_WINDOW)
			running = 0;
		break;
	case ConfigureNotify:
		width = ev->xconfigure.width/10;
		height = ev->xconfigure.height/10;
		debug("ResizeRequest: %d %d\n", width, height);
		width *= 10;
		height *= 10;
		break;
	}
}

void
update_delta() {
	struct timespec last_time = cur_time;
	clock_gettime(CLOCK_REALTIME, &cur_time);

	delta += (cur_time.tv_sec - last_time.tv_sec)
	       + (double)(cur_time.tv_nsec - last_time.tv_nsec) * 1e-9;
}

XPoint xpt_smul(int s, XPoint p) { return (XPoint){ s*p.x, s*p.y}; }
XPoint xpt_add(XPoint a, XPoint b) { return (XPoint){ a.x + b.x, a.y + b.y }; }

void
update_line() {
	if((line[li].x % 10) == 0 && (line[li].y % 10) == 0) {
		debug("delta: %f, vel: %d %d, line %d: %3d %3d %3d %3d\n",
		       delta, vel.d.x, vel.d.y, li, 
		       line[li-1].x, line[li-1].y, line[li].x, line[li].y);

		if(((double)rand() / RAND_MAX) > 0.9) {
			vel.d.x *= -1;
			line[li+1] = line[li];
			li++;
		}
		if(((double)rand() / RAND_MAX) > 0.9) {
			vel.d.y *= -1;
			line[li+1] = line[li];
			li++;
		}
	}
	line[li] = xpt_add(line[li], vel.d);
//	line[li].x += vel[0];
//	line[li].y += vel[1];
	XDrawLines(dpy, win, gc, line, li+1, CoordModeOrigin);
}

void
bound_line() {
	// (x_0,y_0) + (v_x, v_y) = (x_1,y_1)
	// if (0 > x_1) then (x_2,y_2) = (x_1,y_1) - (0-x_1) * (v_x,v_y);
	// if (x_1 > w) then (x_2,y_2) = (x_1,y_1) - (x_1-w) * (v_x,v_y);

	int eps, dir = 0;
	if((eps = line[li].x) < 0) {
		dir = 0;
		goto bump;
	} else if((eps = (width - line[li].x)) < 0) {
		dir = 1;
		goto bump;
	}
check_y:
	if((eps = line[li].y) < 0) {
		dir = 2;
		goto bump;
	} else if ((eps = (height - line[li].y)) < 0) {
		dir = 3;
		goto bump;
	}
	goto check_mem;

bump:
	debug("bump: %d %d\n", dir, eps);
	line[li] = xpt_add(line[li], xpt_smul(eps, vel.d));
	line[li+1] = line[li];
	li++;
	if(dir < 2) {
		vel.d.x = (dir == 0) ? 1 : -1;
		goto check_y;
	} else {
		vel.d.y = (dir == 2) ? 1 : -1;
	}

check_mem:
	if(li >= lM)
		li = 1;
}

void
init() {
	XInitThreads();
	dpy = XOpenDisplay(NULL);

	screen = DefaultScreen(dpy);
	XSetWindowAttributes wa = {
		.background_pixel = BG,
		.event_mask = KeyPressMask|ExposureMask|ClientMessage|StructureNotifyMask,
	};
	win = XCreateWindow(dpy, RootWindow(dpy, screen), 0, 0,
	                    width, height, 0, DefaultDepth(dpy, screen), 
	                    InputOutput, DefaultVisual(dpy, screen), 
	                    CWBackPixel|CWEventMask, &wa);
	XMapWindow(dpy, win);
	XFlush(dpy);

	WM_DELETE_WINDOW = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
	XSetWMProtocols(dpy, win, &WM_DELETE_WINDOW, 1);

	XGCValues gcv = {
		.foreground = FG,
		.background = BG,
		.line_width = 5,
		.line_style = LineSolid,
		.cap_style = CapRound,
		.join_style = JoinRound,
		.fill_style = FillSolid,
	};
	int gcmask = GCForeground|GCBackground|GCLineWidth|GCLineStyle|GCCapStyle|GCJoinStyle|GCFillStyle;
	gc = XCreateGC(dpy, win, gcmask, &gcv);
//	gc = DefaultGC(dpy, screen);

	clock_gettime(CLOCK_REALTIME, &cur_time);
	srand(cur_time.tv_sec);
}

void
run() {
	XEvent ev;
	XSync(dpy, False);

	while(running) {
		while(XPending(dpy)) {
			XNextEvent(dpy, &ev);
			handle_event(&ev);
		}

		update_delta();
		while(delta > 0.f) {
			update_line();
			delta -= 1.f/(vel.m*60.f);
		}

		bound_line();
		fflush(stdout);
	}
}

void
destroy() {
	XDestroyWindow(dpy, win);
	XCloseDisplay(dpy);
}

int
main(int argc, char **argv) {
	init();
	run();
	destroy();
}

//EOF

