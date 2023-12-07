#ifndef CONFIG_H
#define CONFIG_H

/********************************
 * mode trace
 ********************************/
// uncomment to use verbose mode
#define VERBOSE

#ifdef VERBOSE
    #define TRACE0(x) fprintf(stderr, (x))
    #define TRACE1(x,p1) fprintf(stderr, (x), (p1))
    #define TRACE2(x,p1,p2) fprintf(stderr, (x), (p1), (p2))
    #define TRACE3(x,p1,p2,p3) fprintf(stderr, (x), (p1), (p2), (p3))
#else
    #define TRACE0(x)
    #define TRACE1(x,p1)
    #define TRACE2(x,p1,p2)
    #define TRACE3(x,p1,p2,p3)
#endif

#endif
