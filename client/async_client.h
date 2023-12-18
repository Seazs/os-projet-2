#ifndef _ASYNC_CLIENT_H_
#define _ASYNC_CLIENT_H_

extern volatile sig_atomic_t signal_received ;
extern volatile sig_atomic_t sigpipe_receive ;

void * sending_thread(void * arg);

void * receiving_thread(void * arg);

void Thread_Signal_Handler(int signal);

#endif // _ASYNC_CLIENT_H_