The assignment was to modify ray-tracing program in order to
use multithreading provided by the "pthread.h" library. We used
"pthread_create()" and "pthread_join()" to manage multiple threads
and run portions of the program in parallel.

At first I was not sure where to multithread the program. I eventually
found that the pixel color calculations was the easiest place to do so
using columns. I used pthread_create() to invoke the color calculation
fucntion which determined pixel colors to be calculated based on the given
thread ID number.

To avoid race conditions where the pixels of one thread would be printed before
another, I created a 3d float array to store the scale_color values for each
pixel. After pthread_join() in main(), the stored color values were printed
in the right order.

After modifying the ray-tracer to use multithreading, I ran make clean check
to test the speed of the output with 1, 2, 4, and 8 threads. I saved the output
of this command in the file "make-log.txt". The time that it took for the program
to run with each number of threads is shown below.

1 thread:

real    0m42.796s
user    0m42.790s
sys     0m0.001s

2 threads:

real    0m22.282s
user    0m44.158s
sys     0m0.001s

4 threads:

real    0m11.247s
user    0m44.484s
sys     0m0.003s

8 threads:

real    0m5.724s
user    0m44.359s
sys     0m0.002s

The user time is similar across threads but the real time goes down by
a factor of 2 each time.

Increasing the number of threads caused a noticeable increase in the
speed of the program. This is because more threads can run more sections of the
program at the same time.