#include<stdio.h>
#include<stdlib.h>
#include<X11/X.h>
#include<X11/Xlib.h>
#include<GL/gl.h>
#include<GL/glx.h>
#include<GL/glu.h>

static Display              *dpy;
static Window               root;
static GLint                att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
static XVisualInfo          *vi;
static Colormap             cmap;
static XSetWindowAttributes swa;
static Window               win;
static GLXContext           glc;
static XWindowAttributes    gwa;
static XEvent               xev;

void
drawquad() {
//	glClearColor(1.f, 1.f, 1.f, 1.f);
	glClearColor(0.125, 0.125, 0.125, 1.f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-1.f, 1.f, -1.f, 1.f, 1.f, 20.f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.f, 0.f, 10.f, 0.f, 0.f, 0.f, 0.f, 1.f, 0.f);

	glBegin(GL_QUADS);
		glColor3f(1.f, 0.f, 0.f); glVertex3f(-0.75, -0.75, 0.f);
		glColor3f(0.f, 1.f, 0.f); glVertex3f( 0.75, -0.75, 0.f);
		glColor3f(0.f, 0.f, 1.f); glVertex3f( 0.75,  0.75, 0.f);
		glColor3f(1.f, 1.f, 0.f); glVertex3f(-0.75,  0.75, 0.f);
	glEnd();
}

int
main(int argc, char **argv) {
	dpy = XOpenDisplay(NULL);

	if(dpy == NULL) {
		printf("error: can't connect to X server\n");
		exit(0);
	}

	root = DefaultRootWindow(dpy);

	vi = glXChooseVisual(dpy, 0, att);
	if (vi == NULL) {
		printf("error: no appropo visual found\n");
		exit(0);
	} else {
		printf("info: visual %p selected\n", (void*)vi->visualid);
	}

	cmap = XCreateColormap(dpy, root, vi->visual, AllocNone);
	swa.colormap = cmap;
	swa.event_mask = ExposureMask | KeyPressMask;

	win = XCreateWindow(dpy, root, 0, 0, 600, 600, 0, vi->depth, InputOutput, vi->visual, CWColormap |CWEventMask, &swa);

	XMapWindow(dpy, win);
	XStoreName(dpy, win, "ogltest");
	glc = glXCreateContext(dpy, vi, NULL, GL_TRUE);
	glXMakeCurrent(dpy, win, glc);
	glEnable(GL_DEPTH_TEST);

	while(1) {
		XNextEvent(dpy, &xev); 
		if(xev.type == Expose) {
			XGetWindowAttributes(dpy, win, &gwa);
			glViewport(0,0, gwa.width, gwa.height);
			drawquad();
			glXSwapBuffers(dpy, win);
		} else if (xev.type == KeyPress) {
			glXMakeCurrent(dpy, None, NULL);
			glXDestroyContext(dpy, glc);
			XDestroyWindow(dpy, win);
			XCloseDisplay(dpy);
			exit(0);
		}
	}
}
