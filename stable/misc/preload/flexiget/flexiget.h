#ifndef __FLEXIGET_H
#define __FLEXIGET_H

#include "config.h"
#include "lib.h"

#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <netdb.h>
#ifndef	NOGETOPTLONG
#define _GNU_SOURCE
#include <getopt.h>
#endif
#include <limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in_systm.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <pthread.h>

#define _( x )			x

/* Compiled-in settings							*/
#define MAX_STRING		256
#define MAX_REDIR		5
#define flexiget_VERSION_STRING	"3.0"
#define USER_AGENT		"flexiget " flexiget_VERSION_STRING " (" ARCH ")"

#ifdef CC_X86_64
#define PRINTF_UINT64_T "lu"
#else
#define PRINTF_UINT64_T "llu"
#endif


typedef struct
{
	void *next;
	char text[MAX_STRING];
} message_t;

typedef message_t url_t;
//typedef message_t if_t;

#include "conf.h"
#include "tcp.h"
#include "ftp.h"
#include "http.h"
#include "conn.h"
#include "search.h"

#define min( a, b )		( (a) < (b) ? (a) : (b) )
#define max( a, b )		( (a) > (b) ? (a) : (b) )

extern int debug_level;
extern FILE * debug_ouputfp;

/*
#define debug(b, args...) \
		{	\
			if (debug_level || debug_ouputfp!=stderr) {\
				fprintf(debug_ouputfp, b, ## args);\
			}	\
		}
*/
void debug(char * format, ...);

typedef struct
{
	conn_t *conn;	//Xcell ������ӵ���Ϣ����һ������
	conf_t * conf;	//�����ļ���Ϣ
	char filename[MAX_STRING];	//��ŵ����ļ�
	double start_time;			//����ʼʱ��
	uint32_t next_state, finish_time;	//
	uint64_t bytes_done, start_byte, size;
	uint32_t bytes_per_second;		//ͳ����������
	uint32_t delay_time;				//�Ƽ����ص�ʱ��
	//int outfd;					//output file fd
	FILE * outfp;					//output file fd
	int ready;					//�Ƿ������ֹʱ���Ѿ��������
	message_t *message;
	url_t *url;					//�������ϵ�url
} flexiget_t;		//һ�����������ȫ����Ϣ



/*
 * Xcell
 * ��ʼ��flexiget_t������������ȫ����Ϣ�������ݽṹ
 * ���л��ȡһ��filesize�����Ƚ���һ����������
 */
flexiget_t *flexiget_new( conf_t *conf, int count, void *url );


/*
 * Xcell
 * 	��ʼ��ÿһ�������̵߳�������Ϣ�����ܴ���ʱ�ļ��л�ȡ�ϵ�������Ϣ
 */
int flexiget_open( flexiget_t *flexiget );



/*
 * Xcell ���������ʼ�������������������������̣߳�ÿ���߳̽���TCP����������
 */
void flexiget_start( flexiget_t *flexiget );



/*
 * Xcell
 * 	���߳����صĽ��չ���
 *
 * ͳһ��һ���߳��н��ն��TCP���ӵ����ݣ���ͳһд�ļ�
 * ������Ʒǳ�����ڲ�����̫�����ܵ�����£�����Ƽ�
 */
void flexiget_do( flexiget_t *flexiget );



/*
 * Xcell ���߳����ؽ����������Ϣ������ϵ��ļ���
 */
void flexiget_close( flexiget_t *flexiget );


/* Xcell ��ȡ��ǰʱ�� */
double gettime();

#endif

