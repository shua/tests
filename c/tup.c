#include<stdio.h>
#define CAT(c, ...) c ## __VA_ARGS__
#define nargs(...) nargs_(__VA_ARGS__, nargs_rseq)
#define nargs_rseq 4, 3, 2, 1
#define nargs_(...) nargs__(__VA_ARGS__)
#define nargs__(_1, _2, _3, _4, N, ...) N

#define tup(V, ...) tup_t(__VA_ARGS__) V = { __VA_ARGS__ }
#define tup_t(...) tup_(nargs(__VA_ARGS__),__VA_ARGS__)
#define tup_(N, ...) struct { CAT(tup,N)(__VA_ARGS__); }
#define tup1(X, ...) typeof(X) x
#define tup2(X, Y, ...) tup1(X); typeof(Y) y
#define tup3(X, Y, Z, ...) tup2(X, Y); typeof(Z) z
#define tup4(X, Y, Z, W, ...) tup3(X, Y, Z); typeof(W) w


struct {
	int i;
	float f;
	char c;
} test = { 2, 2.5, 'c'};

int
main(int argc, char **argv) {
	tup_t(int, float, 0x3) x;
	tup(y, 0.4, 'a', 2.f);
	printf("%f %c %f\n", y.x, y.y, y.z);
}
