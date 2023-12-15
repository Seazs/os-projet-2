#ifndef _ASYNC_CLIENT_H_
#define _ASYNC_CLIENT_H_

extern volatile sig_atomic_t signal_received;

void * sending_thread(void * arg);

void * receiving_thread(void * arg);


#endif // _ASYNC_CLIENT_H_