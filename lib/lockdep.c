/*
 * pthread lockdep
 *
 * modified code from http://ceph.com/dev-notes/lockdep-for-pthreads
 *
 * This is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License version 2.1, as published by the Free Software
 * Foundation.  See file COPYING.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "lockdep.h"

#define	MAXPID	40
#define	MAXLOCK	30


typedef	struct	Btrace	Btrace;

struct	Btrace
{
	unsigned long	pid;
	int	deadlock;
	unsigned char	payload[1];
};

enum
{
	btsize	 = 600,
};

static	pthread_mutex_t lockdep_lk = PTHREAD_MUTEX_INITIALIZER;
static	pthread_mutex_t*	held[MAXPID][MAXLOCK];
static	pthread_mutex_t*	lockid[MAXLOCK];
static	pthread_t	pid_tab[MAXPID];
static	Btrace*		follows[MAXLOCK][MAXLOCK];
static	int		detected = 0;
static	int		init = 0;


static void
lock_lockdep(void)
{
	pthread_mutex_lock(&lockdep_lk);
}

static void
unlock_lockdep(void)
{
	pthread_mutex_unlock(&lockdep_lk);
}

static int
get_lockid(pthread_mutex_t *q)
{
	int i, found;

	found = 0;
	for(i = 0; i < MAXLOCK && lockid[i] != NULL; i++) {
		if(lockid[i] == q) {
			found = 1;
			break;
		}
	}
	if(found == 0)
		lockid[i] = q;

	return i;
}


static pthread_mutex_t *
get_lock(int i)
{
	if(i < 0 || i >=MAXLOCK)
		return NULL;

	return lockid[i];
}



static int
get_internal_pid()
{
	int i, found;
	pthread_t tid;

	tid = pthread_self();

	found = 0;
	for(i = 0; i < MAXPID && pid_tab[i] != (pthread_t)-1; i++) {
		if(pid_tab[i] == tid) {
			found = 1;
			break;
		}
	}
	if(found == 0)
		pid_tab[i] = tid;

	return i;
}

static pthread_t
get_pid(int i)
{
	if(i < 0 || i >=MAXPID)
		return (pthread_t) 0;

	return pid_tab[i];
}

static int
does_follow(int a, int b)
{
	int i;

	if(a < 0 || a >= MAXLOCK || b < 0 || b >= MAXLOCK)
		return 0;

	if(follows[a][b])
		return 1;

	for(i=0; i<MAXLOCK; i++) {
		if(follows[a][i] && does_follow(i, b)) {
			printf("existing intermediate dependency \n");
			return 1;
		}
	}

	return 0;
}

static void
lockdep_init(void)
{
	int i, j;

#ifdef DEBUG
	printf("lockdep_init <<< \n");
#endif
	if(init == 0) {
		for(i = 0; i < MAXLOCK; i++)
			for(j = 0; j < MAXLOCK; j++)
				follows[i][j] = NULL;

		for(i = 0; i < MAXLOCK; i++)
			lockid[i] = NULL;

		for(i = 0; i < MAXPID; i++)
			pid_tab[i] = (pthread_t)-1;

		for(i = 0; i < MAXPID; i++)
			for(j = 0; j < MAXLOCK; j++)
				held[i][j] = NULL;

		init = 1;
	}

#ifdef DEBUG
	printf("lockdep_init >>> \n");
#endif

}


static void
dump_lockdep(int dmpbt)
{
	int i, j;
	Btrace	*bt;
	pthread_mutex_t	*qlk;

	printf("current mutex with holding threads (PID)\n");
	printf("========================================\n");
	for(i = 0; i < MAXPID; i++)
		for(j = 0; j < MAXLOCK; j++)
			if(held[i][j] != NULL) {
				qlk = get_lock(j);
				printf("pid 0x%x holds lock 0x%x (id=%d)\n",
					i, qlk, j);
			}

	printf("\n\n");

	printf("Recorded lock dependency\n");
	printf("========================================\n");
	for(i = 0; i < MAXLOCK; i++)
		for(j = 0; j < MAXLOCK; j++) {
			bt = follows[i][j];
			if(bt != NULL) {
				printf("pid 0x%x lock 0x%x (id=%d) -> lock 0x%x (id=%d) %s\n",
					bt->pid, get_lock(i), i, get_lock(j), j,
					bt->deadlock==1?"deadlock":"ok");
				//if(dmpbt == 1)
					//printf("%s\n\n", (char *) bt->payload);
			}
		}

}

static int
will_lock(pthread_mutex_t *mutex, int pid)
{
	unsigned long	sp;
	int	lockid, i;
	Btrace	*bt;

	if(pid < 0 || pid >= MAXPID || mutex == NULL)
		return -1;

	lock_lockdep();

#ifdef DEBUG
	printf("will_lock >>>\n");
#endif

	lockid = get_lockid(mutex);

	for(i = 0; i < MAXLOCK; i++) {
		if(held[pid][i] == NULL)
			continue;

		if(!follows[i][lockid]) {
			bt = (Btrace*)malloc((sizeof(Btrace) + btsize));
			//backtrace(bt->payload, btsize, &sp);
			if(bt == NULL)
				printf("will_lock: *** malloc error \n");

			follows[i][lockid] = bt;
			(follows[i][lockid])->pid = pid;

			if(does_follow(lockid, i)) {

				if(detected == 0) {
					printf(">>> will_lock: *** pid 0x%x deadlock lock 0x%lux (id=%d) -> lock 0x%lux (id=%d)\n",
						get_pid(pid), (unsigned long)get_lock(i), i, (unsigned long)mutex, lockid);
					detected = 1;
					(follows[i][lockid])->deadlock = 1;

					dump_lockdep(1);
				}

				break;
			}
			else
			{
				(follows[i][lockid])->pid = pid;
				//printf("%t will_lock: pid %lud lock 0x%lux (id=%d) -> lock 0x%lux (id=%d, %s)\n",
					//pid, (unsigned long)get_lock(i), i, (unsigned long)mutex, lockid, q->name);
			}
		}
	}
#ifdef DEBUG
	printf("will_lock <<<\n");
#endif
	unlock_lockdep();
	return 0;
}



static int
locked(pthread_mutex_t *mutex, int pid)
{
	int	lockid;

#ifdef DEBUG
	printf("locked >>> \n");
#endif

	if(pid < 0 || pid >= MAXPID || mutex == NULL)
		return -1;

	lock_lockdep();

	lockid = get_lockid(mutex);
	held[pid][lockid] = mutex;

	unlock_lockdep();

#ifdef DEBUG
	printf("locked <<< \n");
#endif

	return 0;
}

static int
unlocked(pthread_mutex_t *mutex, int pid)
{

	int	lockid;

#ifdef DEBUG
	printf("unlocked >>>\n");
#endif

	if(pid < 0 || pid >= MAXPID || mutex == NULL)
		return -1;

	lock_lockdep();

	lockid = get_lockid(mutex);
	held[pid][lockid] = NULL;

	unlock_lockdep();

#ifdef DEBUG
	printf("unlocked <<<\n");
#endif

	return 0;
}

int
mutex_lock(pthread_mutex_t *mutex)
{
	int r, pid;

	if(init == 0)
		lockdep_init();

	pid = get_internal_pid();
	will_lock(mutex, pid);
	r = pthread_mutex_lock(mutex);
	locked(mutex, pid);
	return r;
}

int
mutex_unlock(pthread_mutex_t *mutex)
{
	int r, pid;

	if(init == 0)
		lockdep_init();

	pid = get_internal_pid();
	r = pthread_mutex_unlock(mutex);
	unlocked(mutex, pid);
	return r;
}
