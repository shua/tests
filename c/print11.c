#include <stdio.h>

#define CAT(X,...) X ## __VA_ARGS__

#define NARGS_RSEQ 16,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1
#define NARGS(...) NARGS_(__VA_ARGS__, NARGS_RSEQ)
#define NARGS_(...) NARGS__(__VA_ARGS__)
#define NARGS__(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,_16,N,...) \
	NARGS_C(IS_1(N),N,_1)
#define NARGS_C(C,N,_1) 	CAT(NARGS_,C)(N,_1)
#define NARGS_0(N,_1) 		N
#define NARGS_1(N,_1) 		NARGS_1C(IS_EMPTY(_1))
#define NARGS_1C(C) 		CAT(NARGS_1,C)
#define NARGS_11 0
#define NARGS_10 1

#define CHECK(X,...) 		CHECK_(X,1,0)
#define CHECK_(X,Y,B,...) 	B
#define PROBE(...) 			~,~
#define IS_1(N) 			CHECK(CAT(IS_1_,N))
#define IS_1_1 				PROBE()
#define IS_1_(...) 			0
#define IS_PAREN(X) 		CHECK(PROBE X)
#define IS_EMPTY(X) 		IS_EMPTY_C(IS_PAREN(X),X)
#define IS_EMPTY_C(C,X) 	CAT(IS_EMPTY_,C)(X)
#define IS_EMPTY_1(X) 		0
#define IS_EMPTY_0(X) 		CHECK(CAT(IS_EMPTY_0_,X))
#define IS_EMPTY_0_ 		PROBE()

#if 0
"check"
CHECK(x)
CHECK(PROBE())
"is_1"
IS_1(x)
IS_1(0)
IS_1(1)
"is_paren"
IS_PAREN(x)
IS_PAREN(())
IS_PAREN((char)'c')
"is_empty"
IS_EMPTY()
IS_EMPTY(x)
IS_EMPTY((char)'c')
"nargs"
NARGS_C(0,12,)
NARGS_C(1,1,)
NARGS_C(1,1,())
NARGS_C(1,1,1)
NARGS(1,2)
NARGS(1)
NARGS()
"end tests"
#endif

#define MAP(fun,...) MAPN(NARGS(__VA_ARGS__),fun,__VA_ARGS__)
#define MAPN(N,...) CAT(MAP,N)(__VA_ARGS__)
#define MAP0(...)
#define MAP1(fun,X,...) fun(X)
#define MAP2(fun,X,...) fun(X) MAP1(fun,__VA_ARGS__)
#define MAP3(fun,X,...) fun(X) MAP2(fun,__VA_ARGS__)
#define MAP4(fun,X,...) fun(X) MAP3(fun,__VA_ARGS__)
#define MAP5(fun,X,...) fun(X) MAP4(fun,__VA_ARGS__)
#define MAP6(fun,X,...) fun(X) MAP5(fun,__VA_ARGS__)
#define MAP7(fun,X,...) fun(X) MAP6(fun,__VA_ARGS__)
#define MAP8(fun,X,...) fun(X) MAP7(fun,__VA_ARGS__)
#define MAP9(fun,X,...) fun(X) MAP8(fun,__VA_ARGS__)
#define MAP10(fun,X,...) fun(X) MAP9(fun,__VA_ARGS__)
#define MAP11(fun,X,...) fun(X) MAP10(fun,__VA_ARGS__)
#define MAP12(fun,X,...) fun(X) MAP11(fun,__VA_ARGS__)
#define MAP13(fun,X,...) fun(X) MAP12(fun,__VA_ARGS__)
#define MAP14(fun,X,...) fun(X) MAP13(fun,__VA_ARGS__)
#define MAP15(fun,X,...) fun(X) MAP14(fun,__VA_ARGS__)
#define MAP16(fun,X,...) fun(X) MAP15(fun,__VA_ARGS__)

#define printfmt_(X) _Generic((X), \
	char: "%c ", \
	int: "%d ", \
	unsigned int: "%ud ", \
	float: "%f ", \
	double: "%f ", \
	char*: "%s ", \
	default: "? ")
#define printfmt printfmt_
#define print(...) MAP(print_,__VA_ARGS__) printf("\n");
#define print_(X) printf(printfmt(X), (X));

int
main(int argc, char **argv) {
	int i = 4;
	printf("printfmt('\\n') = %s\n", printfmt('\n'));
	print(1,"this is next");
	print();
}
