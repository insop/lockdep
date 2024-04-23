#ifndef _LOCKDEP_H_
#define _LOCKDEP_H_

#ifdef __cplusplus
extern "C"
{
#endif


#include <pthread.h>

int mutex_lock(pthread_mutex_t *mutex);
int mutex_unlock(pthread_mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* _LOCKDEP_H_ */
