// What is this?
// this is a preprocessor hack to get switches and cases for any type
// as long as you supply the compare function

// usage
//	athru(x, compare_function) {
//	dala(y_1) // <-- fallthrough
//	dala(y_2)
//		...
//		break; // <-- works as expected
//	dala(y_3)
//		...
//		break;
//	cheal  // <-- required
//		...
//	}

// differences from normal switch/case/default:
// - the words chosen are irish: athru/dala/cheal respectively
// - no colon ':' after the dala/cheal statements
// - 'dala(x)' requires parentheses '()' as opposed to 'case n'
// - you must have a 'cheal', even if you don't have anything after,
//    (alternatively, you could do a }}, but that is weirder to remember)
// - 'cheal' can only be used after all the other 'dala's
//  
// similarities to normal switch/case/default:
// - dala/case without 'break' will fallthrough
// - athru/switch blocks can be nested
//

#ifndef NDEBUG
#include<stdio.h>
unsigned int layer = 0;
#define newlayer layer++
#define indent(n) for(int i=0; i<n; i++) printf("\t")
#define debug indent(layer); printf
#else
#define layer
#define newlayer
#define indent(n)
#define debug(x)
#endif


#include<string.h>
// notice that each nested athru, defines its own things for its own scope
// and the for loop should only run once, but allow us to use break statements inside
#define athru(x, cmp) for(char _athruf=1; _athruf; _athruf=0) { \
	newlayer; \
	char _athrup = 1; \
	typeof(x) _athru = (x); \
	int (*_athrucmp)(const typeof(x), const typeof(x)) = cmp; \
	debug("athru("#x","#cmp")\n");
#define dala(x) } if(_athrup == 0 || (_athrup = _athrucmp(x, _athru)) == 0) { \
	debug("dala("#x")\n");
#define cheal } debug("cheal\n");

int
main(int argc, char **argv) {
	int r = 0;
	if(argc > 1)
		athru(argv[1], strcmp) {
		dala("dogs")
		dala("cats")
			r = 1;
			break;
		dala("footballs")
		dala("cars")
			r = 2;
			break;
		cheal
			if(argc > 2)
				athru(argv[2], strcmp) {
				dala("hats")
					r = 3;
					break;
				cheal
				}

			r = -1;
		}
	return r;
}
