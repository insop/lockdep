#ifndef _LOCKDEP_H_
#define _LOCKDEP_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <pthread.h>

// Locks a mutex, ensuring exclusive access to a resource.
// This function should be used to avoid data races when multiple threads attempt to access the same resource.
int mutex_lock(pthread_mutex_t *mutex);

// Unlocks a mutex, releasing exclusive access to a resource.
// This function should be called after `mutex_lock` to signal that the resource is no longer being accessed by the current thread.
int mutex_unlock(pthread_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* _LOCKDEP_H_ */
