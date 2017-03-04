/**
 * omp_timer.h
 *
 * CS 470 timer macros
 */

#ifdef _OPENMP
#   include <omp.h>
#   define START_TIMER(X) double _start_##X = omp_get_wtime();
#   define STOP_TIMER(X)  double _elapsed_##X = omp_get_wtime() - _start_##X;
#   define GET_TIMER(X)   (_elapsed_##X)
#else
#   include <time.h>
#   define START_TIMER(X) clock_t _start_##X = clock();
#   define STOP_TIMER(X)  clock_t _elapsed_##X = clock() - _start_##X;
#   define GET_TIMER(X)   ((double)(_elapsed_##X) / (double)CLOCKS_PER_SEC)
#endif

