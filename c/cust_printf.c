#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#define LEN(a) (sizeof (a) / sizeof (a[0]))

typedef int (*vfprint_fun)(FILE*, char**, va_list);
int vfprint_vec3(FILE*, char**, va_list);
int fmprintf(FILE*, const char*, ...);
int vfmprintf(FILE*, const char*, va_list);

const struct {
	char type;
	vfprint_fun fun;
} def[] = {
	{'v', vfprint_vec3}
};

typedef union {
	int a[3];
	struct {
		int x;
		int y;
		int z;
	};
} vec3;

int
vfprint_vec3(FILE *fp, char **fmt, va_list ap) {
	int i=0;

	vec3 v = va_arg(ap, vec3); // can we do this?
	// fmt might have some print options, but we're ignoring them.
	*fmt += 2;
	return fprintf(fp, "(%d, %d, %d)", v.x, v.y, v.z);
}

int
fmprintf(FILE *fp, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	vfmprintf(fp, fmt, ap);
	va_end(ap);
}

int
vfmprintf(FILE* fp, const char *format, va_list ap) {
	int i=0,r=0;
	char *fmt;
	char *b,*e;

	// could do const* -> nonconst*, but let's do it the right way
	fmt = calloc(1, strlen(format)+1);
	strcpy(fmt, format);

	// go through fmt until %
	for(b=e=fmt; e[0]; e++) if(e[0]=='%') {
		// if % is custom, call fprint_fun
		for(i=0; i<LEN(def); i++) if(def[i].type == e[1]) {
			// vfprintf previous string and args
			e[0] = 0;
			r += vfprintf(fp, b, ap);
			e[0] = '%';
			r += def[i].fun(fp, &e, ap);
			b=e;
			break;
		}
	}

	if(b!=e) r += vfprintf(fp, b, ap);
	free(fmt);
	return r;
}

int
main(int argc, char **argv) {
	vec3 v = { 2, 0, -1 }, w = { -1, -2, 1e3 };
	fmprintf(stdout, "%s : %v, %v %d3 is not equal to %f\n","vec3", v, w, 10, 4.5);
}

// TODO: fmt flags for custom types
// v3 -> vec3 printing, v2 -> vec2 printing
