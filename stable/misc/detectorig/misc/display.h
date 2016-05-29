/*
 *������Ե�ʱ���ӡ��Ϣ��
 *P�����ӡ��E�������ܵ���N��ʾ�޶����ȣ�U����ʮ�����ƣ�X����unsigned char
 *���Ҫȥ����ӡ��ֻҪ��#define DEBUG_PRINT_INFO����
*/

#ifndef DISPLAY_H
#define DISPLAY_H

#include <time.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#ifndef DEBUG_PRINT_INFO
#define PNOW() {time_t now = time(NULL);printf("%s", ctime(&now));}
#define PENOW() {time_t now = time(NULL);fprintf(stderr, "%s", ctime(&now));}
//��ӡһ������
#define PINT(x) printf(#x"=[%d] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
#define PEINT(x) fprintf(stderr, #x"=[%d] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
//��ӡһ����\0��β���ַ���
#define PSTR(x) printf(#x"=[%s] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
#define PESTR(x) fprintf(stderr, #x"=[%s] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
//��ӡ�����ַ�����nΪ����
#define PNSTR(x,n) printf(#x"=[%.*s] in file=[%s] line=[%d]\n", n, x, __FILE__, __LINE__)
#define PENSTR(x,n) fprintf(stderr, #x"=[%.*s] in file=[%s] line=[%d]\n", n, x, __FILE__, __LINE__)
//��ӡһ��������16������ʽ
#define PINTU(x) printf(#x"=[%u] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
#define PEINTU(x) fprintf(stderr, #x"=[%u] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
//��ӡһ��char��16��������ֵ
#define PINTX(x) printf(#x"=[%x] in file=[%s] line=[%d]\n", x&0xff, __FILE__, __LINE__)
#define PEINTX(x) fprintf(stderr, #x"=[%x] in file=[%s] line=[%d]\n", x&0xff, __FILE__, __LINE__)
//��ӡ�Ը�������
#define PFLOAT(x) printf(#x"=[%f] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
#define PEFLOAT(x) printf(stderr, #x"=[%f] in file=[%s] line=[%d]\n", x, __FILE__, __LINE__)
//��ӡһ�������ַ�����ÿһλΪ��Ӧ��16��������ֵ
#define PNSTRX(x,n) {printf("****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);\
					int i;for(i=0;i<n;i++){printf(" %x", x[i]&0xff);}\
					printf("\n****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);}
#define PENSTRX(x,n) {fprintf(stderr, "****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);\
					int i;for(i=0;i<n;i++){fprintf(stderr, " %x", x[i]&0xff);}\
					fprintf(stderr, "\n****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);}
//��ӡһ�������ַ�����ÿһλΪ��Ӧ��16��������ֵ������ַ�
#define PNSTRX2(x,n) {printf("****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);\
					 int i;for(i=0;i<n;i++){printf(" [%x %c]", x[i]&0xff, x[i]);}\
					 printf("\n****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);}
#define PENSTRX2(x,n) {fprintf(stderr, "****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);\
					int i;for(i=0;i<n;i++){fprintf(stderr, " [%x %c]", x[i]&0xff, x[i]);}\
					fprintf(stderr, "\n****** in file=[%s] line=[%d]\n", __FILE__, __LINE__);}
//��ӡһ���޷���������Ӧ���ַ�����ʽ��ip��ַ
#define PIP(x) {char szStr[20];if(NULL!=inet_ntop(AF_INET, &x, szStr, 20))\
					{printf("ip=[%s] in file=[%s] line=[%d]\n", szStr, __FILE__, __LINE__);}\
					else{printf("ip=[error] in file=[%s] line=[%d]\n", __FILE__, __LINE__);}}
#define PEIP(x) {char szStr[20];if(NULL!=inet_ntop(AF_INET, &x, szStr, 20))\
					{fprintf(stderr, "ip=[%s] in file=[%s] line=[%d]\n", szStr, __FILE__, __LINE__);}\
					else{fprintf(stderr, "ip=[error] in file=[%s] line=[%d]\n", __FILE__, __LINE__);}}
//��ӡһ���˿ںţ�����Ϊ�����ֽ���Ķ˿ں�
#define PPORT(x) printf(#x"=[%u] in file=[%s] line=[%d]\n", ntohs(x), __FILE__, __LINE__)
#define PEPORT(x) fprintf(stderr, #x"=[%u] in file=[%s] line=[%d]\n", ntohs(x), __FILE__, __LINE__)

#else	//DEBUG_PRINT_INFO

#define PINT(x)
#define PEINT(x)

#define PSTR(x)
#define PESTR(x)
#define PNSTR(x,n)
#define PENSTR(x,n)

#define PINTU(x)
#define PEINTU(x)

#define PINTX(x)
#define PEINTX(x)

#define PFLOAT(x)
#define PEFLOAT(x)

#define PNSTRX(x,n)
#define PENSTRX(x,n)

#define PNSTRX2(x,n)
#define PENSTRX2(x,n)

#define PIP(x)
#define PEIP(x)

#endif	//DEBUG_PRINT_INFO

#endif	//DISPLAY_H
