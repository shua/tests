#include <stdio.h>

#define forlims(m,M) for(int it=m; it<M; ++it)
#define forlim(M) forlims(0,M)
#define forarr(arr) \
	for(typeof(arr[0]) *it=arr; \
	    it<(arr+(sizeof(arr)/sizeof(arr[0]))); \
	    it++)
#define foreach(x, ...) \
	for(typeof(x) _it[] = { x, ##__VA_ARGS__ }, *it=_it; \
	    it < (_it+(sizeof(_it)/sizeof(_it[0]))); \
	    it++)

int
main(int argc, char **argv[]) {
	forlims(3,6)
		printf("%d ", it);
	printf("\n");
	forlim(3)
		printf("%d ", it);
	printf("\n");

	int i1 = 1;
	typeof(i1) i2 = 2;
	printf("%d %d\n", i1, i2);
	char c1 = '1';
	typeof(c1) c2 = '2';
	printf("%c %c\n", c1, c2);
	struct {
		char c;
		int i;
	} s1 = { 's', 1 };
	typeof(s1) s2 = {'s', 2};
	printf("{ %c, %d } { %c, %d }\n", s1.c, s1.i, s2.c, s2.i);

	foreach(1, 2, 3)
		printf("%d ", *it);
	printf("\n");
	foreach(s1, s2, { 's', 3 })
		printf("{%c,%d} ", it->c, it->i);
	printf("\n");
	char a[] = "arr";
	forarr(a)
		printf("%c ", *it);
	printf("\n");
}
