# pthread lockdep

`lockdep` is a deadlock detection tool designed for use with pthread_mutex. It is based on the original code from https://ceph.io/en/news/blog/2008/lockdep-for-pthreads/](https://ceph.io/en/news/blog/2008/lockdep-for-pthreads/) and has been modified for enhanced functionality. This tool is distributed under the GNU Lesser General Public License version 2.1.

## What is lockdep?

`lockdep` is a software utility that helps developers detect potential deadlocks in their multithreaded applications. It works by tracking the locking dependencies between different mutexes in your application to identify circular dependencies that could lead to a deadlock.

## Integrating lockdep into Your Project

To integrate `lockdep` into your project, include the `lockdep.h` header file in your source code and link against the `lockdep` library when compiling your application. Ensure that you replace calls to `pthread_mutex_lock` and `pthread_mutex_unlock` with `mutex_lock` and `mutex_unlock` provided by `lockdep`.

## Usage Instructions

To use `lockdep`, initialize it in your application by including the `lockdep.h` header and replacing mutex lock and unlock calls:

```c
#include "lockdep.h"

mutex_lock(&my_mutex);
// Your critical section code here
mutex_unlock(&my_mutex);
```

`lockdep` will automatically track all mutex locking operations and report potential deadlocks.

## Examples

Below is an example demonstrating how to use `lockdep` in a simple application:

```c
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include "lockdep.h"

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t mutex2 = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *arg) {
    mutex_lock(&mutex1);
    // Simulate work
    sleep(1);
    mutex_lock(&mutex2);
    printf("Locked both mutexes\n");
    mutex_unlock(&mutex2);
    mutex_unlock(&mutex1);
    return NULL;
}

int main() {
    pthread_t thread;
    pthread_create(&thread, NULL, thread_function, NULL);
    pthread_join(thread, NULL);
    return 0;
}
```

This example demonstrates basic usage of `lockdep` to detect potential deadlocks.

## Building and Running the Example

To build and run the example provided in the `example` directory:

1. Navigate to the `example` directory.
2. Run `make` to compile the example application.
3. Execute `./lockdep_test` to run the application.

## Troubleshooting

If you encounter issues while using `lockdep`, ensure that you have replaced all instances of `pthread_mutex_lock` and `pthread_mutex_unlock` with `mutex_lock` and `mutex_unlock`. Also, verify that `lockdep` has been properly initialized in your application.

## Contributing

Contributions to `lockdep` are welcome! If you have improvements or bug fixes, please submit a pull request or open an issue on the project's GitHub page.

