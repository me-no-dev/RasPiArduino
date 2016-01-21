/*
  Arduino.c -  Partial implementation of the Wiring API for the Raspberry Pi
  Copyright (c) 2015 Hristo Gochkov.  All right reserved.

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#include "Arduino.h"
#include "bcm2835_registers.h"
#include <pthread.h>

static pthread_mutex_t thread_mutexes[10];

void thread_yield(){
  pthread_yield();
}

pthread_t thread_self(){
  return pthread_self();
}

pthread_t thread_create(thread_fn fn, void * arg){
    pthread_t myThread ;
    if(pthread_create(&myThread, NULL, fn, arg) != 0)
      return 0;
    pthread_detach(myThread);
    return myThread;
}

int thread_set_name(pthread_t t, const char *name){
  return pthread_setname_np(t, name);
}

int thread_set_priority(const int pri){
  struct sched_param sched ;
  memset (&sched, 0, sizeof(sched)) ;
  if (pri > sched_get_priority_max(SCHED_OTHER))
    sched.sched_priority = sched_get_priority_max(SCHED_OTHER);
  else
    sched.sched_priority = pri ;
  return sched_setscheduler(0, SCHED_OTHER, &sched) ;
}

int thread_detach(pthread_t t){
  return pthread_detach(t);
}

int thread_terminate(pthread_t t){
  return pthread_cancel(t);
}

uint8_t thread_running(pthread_t t){
  int r = pthread_tryjoin_np(t, NULL);
  return (r==0 || r==EBUSY);
}

uint8_t thread_equals(pthread_t t){
  return pthread_equal(pthread_self(),t);
}

void thread_lock(int index){
  pthread_mutex_lock(&thread_mutexes[index]);
}

void thread_unlock(int index){
    pthread_mutex_unlock(&thread_mutexes[index]);
}
