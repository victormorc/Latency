/***********************************************************************************************//**
 * Project: Latency
 * Description: Module used to measure latency in a computer.
 * Author: vmoreno
 * Date: 11/03/2019
 * Language: C++
 * Version: 1.0
 * History:
 *		   11/03/2019 - vmoreno - File creation
 **************************************************************************************************/
 
/******************************************* INCLUDES *********************************************/ 
#include <fstream>
#include <iostream>
#include <sstream>
#include <atomic>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/mman.h>


/******************************************* DEFINES *********************************************/
#define NAME_UNIX_IPC_SOCKET_16MS	"Thread_16ms"
#define NAME_UNIX_IPC_SOCKET_20MS	"Thread_20ms"
#define NAME_UNIX_IPC_SOCKET_50MS	"Thread_50ms"

typedef struct argsThread {
	struct timespec period;										/**< Period of thread */
	char nameUnixSocket [sizeof(NAME_UNIX_IPC_SOCKET_16MS)];	/**< Name of the UNIX IPC socket */
}argsThread_st;

std::atomic<bool> bStop {false};	/**< Flag used to notify threads to stop processing data */

void * PeriodicRoutine(void * args);	/**< Signature of function which run worker threads */

void * ProcessData(void * args);		/**< Signature of function which store data from worker threads */

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void signal_handler (int signum) {
	bStop = true;	/**< Set flag to notify threads that application should stop */
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
int main(void)
{

	/** Catch signals */
	signal(SIGINT, signal_handler);
	signal(SIGKILL, signal_handler);
	signal(SIGTERM, signal_handler);

	/** Three different threads, each one with its own period (16, 20 and 50 ms) */
	pthread_t 		thr_16ms, thr_20ms, thr_50ms;		/**< Threads */
	pthread_attr_t 	attr_16ms, attr_20ms, attr_50ms;	/**< Threads attributes */

	/** Thread to store data in file */
	pthread_t _thr;

	/** Priority of thread */
	constexpr sched_param priority_16ms = {49};
	constexpr sched_param priority_20ms = {49};
	constexpr sched_param priority_50ms = {49};

	/** Periods of threads */
	constexpr timespec Period_16ms = {0, 16000000};		/**< 16 ms */
	constexpr timespec Period_20ms = {0, 20000000};		/**< 20 ms */
	constexpr timespec Period_50ms = {0, 50000000};		/**< 50 ms */

	/** Arguments for threads */
	constexpr argsThread_st stArgsThr_16ms = {Period_16ms, NAME_UNIX_IPC_SOCKET_16MS};
	constexpr argsThread_st stArgsThr_20ms = {Period_20ms, NAME_UNIX_IPC_SOCKET_20MS};
	constexpr argsThread_st stArgsThr_50ms = {Period_50ms, NAME_UNIX_IPC_SOCKET_50MS};

    /** Unlink any previous socket created */
    unlink(NAME_UNIX_IPC_SOCKET_16MS);
    unlink(NAME_UNIX_IPC_SOCKET_20MS);
    unlink(NAME_UNIX_IPC_SOCKET_50MS);

	/** Create thread to store data */
	if (pthread_create(&_thr, nullptr, ProcessData, nullptr) < 0) {
		printf("Error creating reader thread\n");
		return EXIT_FAILURE;
	}

	/** Sleep for a while to allow the above thread to create sockets */
	usleep(1000);

	/** Lock memory to avoid page-faults */
	if (mlockall(MCL_CURRENT|MCL_FUTURE) < 0) {
		printf("Error locking memory\n");
		return EXIT_FAILURE;
	}

	/** Initialize threads attributes */
	if (pthread_attr_init(&attr_16ms) < 0) {
		printf("Error initializing thread attribute\n");
		return EXIT_FAILURE;
	}
	else if (pthread_attr_init(&attr_20ms) < 0) {
		printf("Error initializing thread attribute\n");
		return EXIT_FAILURE;
	}
	else if (pthread_attr_init(&attr_50ms) < 0) {
		printf("Error initializing thread attribute\n");
		return EXIT_FAILURE;
	}
	/** Set explicit attribute values */
	else if (pthread_attr_setinheritsched(&attr_16ms, PTHREAD_EXPLICIT_SCHED) < 0) {
		printf("Error setting explicit thread attributes\n");
		return EXIT_FAILURE;
	}
	else if (pthread_attr_setinheritsched(&attr_20ms, PTHREAD_EXPLICIT_SCHED) < 0) {
		printf("Error setting explicit thread attributes\n");
		return EXIT_FAILURE;
	}
	else if (pthread_attr_setinheritsched(&attr_50ms, PTHREAD_EXPLICIT_SCHED) < 0) {
		printf("Error setting explicit thread attributes\n");
		return EXIT_FAILURE;
	}
// 	/** Set policy */
// 	else if (pthread_attr_setschedpolicy(&attr_16ms, SCHED_FIFO) < 0) {
// 		printf("Error setting policy thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
// 	else if (pthread_attr_setschedpolicy(&attr_20ms, SCHED_FIFO) < 0) {
// 		printf("Error setting policy thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
// 	else if (pthread_attr_setschedpolicy(&attr_50ms, SCHED_FIFO) < 0) {
// 		printf("Error setting policy thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
// 	/** Set priority */
// 	else if (pthread_attr_setschedparam(&attr_16ms, &priority_16ms) < 0) {
// 		printf("Error setting priority thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
// 	else if (pthread_attr_setschedparam(&attr_20ms, &priority_20ms) < 0) {
// 		printf("Error setting priority thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
// 	else if (pthread_attr_setschedparam(&attr_50ms, &priority_50ms) < 0) {
// 		printf("Error setting priority thread attribute\n");
// 		return EXIT_FAILURE;
// 	}
	/** Create threads */
	else if (pthread_create(&thr_16ms, &attr_16ms, PeriodicRoutine, (void *)&stArgsThr_16ms) < 0) {
		printf("Error creating thread 16 ms\n");
		return EXIT_FAILURE;
	}
	else if (pthread_create(&thr_20ms, &attr_20ms, PeriodicRoutine, (void *)&stArgsThr_20ms) < 0) {
		printf("Error creating thread 20 ms\n");
		return EXIT_FAILURE;
	}
	else if (pthread_create(&thr_50ms, &attr_50ms, PeriodicRoutine, (void *)&stArgsThr_50ms) < 0) {
		printf("Error creating thread 50 ms\n");
		return EXIT_FAILURE;
	}

	/** Destroy attributes */
	pthread_attr_destroy(&attr_16ms);
	pthread_attr_destroy(&attr_20ms);
	pthread_attr_destroy(&attr_50ms);

	/** Join threads */
	pthread_join(thr_16ms, nullptr);
	pthread_join(thr_20ms, nullptr);
	pthread_join(thr_50ms, nullptr);
	pthread_join(_thr, nullptr);

	/** Unlink sockets */
    unlink(NAME_UNIX_IPC_SOCKET_16MS);
    unlink(NAME_UNIX_IPC_SOCKET_20MS);
    unlink(NAME_UNIX_IPC_SOCKET_50MS);

	return EXIT_SUCCESS;
}


/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void inc_period(struct timespec *pTime, const struct timespec *pPeriod)
{
	pTime->tv_nsec += pPeriod->tv_nsec;

	while (pTime->tv_nsec >= 1000000000) {
		/* timespec nsec overflow */
		pTime->tv_sec++;
		pTime->tv_nsec -= 1000000000;
	}
}

/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void * PeriodicRoutine(void * args) {

	/** Current time */
	timespec currentTime;

	/** Data buffer */
	char buffer[sizeof(timespec)];

	/** Get arguments */
	argsThread_st * argThr = static_cast<argsThread_st *>(args);

	/** Create Unix IPC socket to send data */
	int fdSocket = socket(AF_UNIX, SOCK_DGRAM, 0);	/**< Socket */
	struct sockaddr_un remote;
	memset(&remote, 0, sizeof(struct sockaddr_un));

	/** Check socket was created correctly */
	if (fdSocket < 0) {
		printf("Error creating socket\n");
		return nullptr;
	}

    /** Set up the UNIX sockaddr structure by using AF_UNIX for the family and giving it a filepath to send to. */
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, argThr->nameUnixSocket);

	/** Infinite loop */
	while(!bStop)
	{
		/** Get time */
		clock_gettime(CLOCK_MONOTONIC, &currentTime);

		/** Copy timestamp to buffer */
		memcpy(buffer, (void *)&currentTime, sizeof(timespec));

		/** Send timestamp */
		if (sendto(fdSocket, (void *)(&buffer), sizeof(buffer), 0,(sockaddr *)(&remote), sizeof(remote)) < 0) {
			printf("MESSAGE COULD NOT BE DELIVERED\n");
		}

		/** Calculate time to start next step */
		inc_period(&currentTime, &(argThr->period));


		/** HERE SHOULD DO THE REAL WORK */

		/** Sleep until next step */
		clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &currentTime, nullptr);
	}

	/** Close socket */
	close(fdSocket);

	return nullptr;
}


/**************************************************************************************************
 ******************************************* FUNCTION *********************************************
 *************************************************************************************************/
void * ProcessData(void * args) {
	/** Format file name */
	std::ostringstream oss;
	oss << "Latency.csv";
	std::fstream file = std::fstream(oss.str(), std::ios::out | std::ios::binary); /**< Open Latency file */

	/** Buffer to retrieve data from socket */
	char data_buffer [sizeof(struct timespec)];
	struct timespec timestamp;

	/** Open sockets */
	int fdSock_16ms = socket(AF_UNIX, SOCK_DGRAM, 0);
	int fdSock_20ms = socket(AF_UNIX, SOCK_DGRAM, 0);
	int fdSock_50ms = socket(AF_UNIX, SOCK_DGRAM, 0);

	/** Check sockets are initialize correctly */
	if ((fdSock_16ms < 0) || (fdSock_20ms < 0) || (fdSock_50ms < 0)) {
		printf("Error creating receiving sockets\n");
		return nullptr;
	}

	/** Bind socket for thread 16ms */
	struct sockaddr_un remote;
	memset(&remote, 0, sizeof(struct sockaddr_un));
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, NAME_UNIX_IPC_SOCKET_16MS);

    if (bind(fdSock_16ms, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
    	printf("Error binding socket\n");
    }

	/** Bind socket for thread 20ms */
    memset(&remote, 0, sizeof(remote));
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, NAME_UNIX_IPC_SOCKET_20MS);
    if (bind(fdSock_20ms, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
    	printf("Error binding socket\n");
    }

	/** Bind socket for thread 50ms */
    memset(&remote, 0, sizeof(remote));
    remote.sun_family = AF_UNIX;
    strcpy(remote.sun_path, NAME_UNIX_IPC_SOCKET_50MS);
    if (bind(fdSock_50ms, (struct sockaddr *)&remote, sizeof(remote)) < 0) {
    	printf("Error binding socket\n");
    }

	char time_string[50];	/**< String with time converted */
	/** Recorder loop */
	while(!bStop) {
		/** Get data from thread 16ms */
		if (recv(fdSock_16ms, (void *)(data_buffer), sizeof(data_buffer), MSG_DONTWAIT) > 0) {
			/** Set data */
			memcpy(&timestamp, (void *)&data_buffer, sizeof(timespec));
			/** Write data into file */
			memset((void *)&time_string, 0, sizeof(time_string));
			snprintf(time_string, sizeof(time_string), "2, %ld.%09ld\n", timestamp.tv_sec, timestamp.tv_nsec);
			std::string data = time_string;
			file.write(data.c_str(), data.length());
		}

		/** Get data from thread 20ms */
		if (recv(fdSock_20ms, (void *)(data_buffer), sizeof(data_buffer), MSG_DONTWAIT) > 0) {
			/** Set data */
			memcpy(&timestamp, (void *)&data_buffer, sizeof(timespec));
			/** Write data into file */
			memset((void *)&time_string, 0, sizeof(time_string));
			snprintf(time_string, sizeof(time_string), "1, %ld.%09ld\n", timestamp.tv_sec, timestamp.tv_nsec);
			std::string data = time_string;
			file.write(data.c_str(), data.length());
		}

		/** Get data from thread 50ms */
		if (recv(fdSock_50ms, (void *)(data_buffer), sizeof(data_buffer), MSG_DONTWAIT) > 0) {
			/** Set data */
			memcpy(&timestamp, (void *)&data_buffer, sizeof(timespec));
			/** Write data into file */
			memset((void *)&time_string, 0, sizeof(time_string));
			snprintf(time_string, sizeof(time_string), "3, %ld.%09ld\n", timestamp.tv_sec, timestamp.tv_nsec);
			std::string data = time_string;
			file.write(data.c_str(), data.length());
		}
	}

    /** Close file */
    file.flush();
    file.close();

    /** Close sockets */
    close(fdSock_16ms);
    close(fdSock_20ms);
    close(fdSock_50ms);

    return nullptr;
}
