#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include "child_mgr.h"
#include "misc.h"


//���ӽ��̣����ùܵ���ͨ
static int startupOneProcess(struct OneProcessInfo* pstOneProcessInfo, char* const argv[])
{
	int infd[2];
	int outfd[2];

	if(-1 == pipe(infd))
		return -1;

	if(-1 == pipe(outfd))
		return -1;

	//fork�ӽ���
	pstOneProcessInfo->pid = fork();

	if(-1 == pstOneProcessInfo->pid) {
		return -1;
	}

	if(0 == pstOneProcessInfo->pid) { //�ӽ���
		dup2(outfd[0], 0);
		dup2(infd[1], 1);
		int devnull = open("/dev/null", O_WRONLY);
		dup2(devnull, 2);
		close(devnull);

		int i;
		for(i=3 ; i<=getdtablesize() ; i++)
			close(i);
		execv(argv[0], argv);
		exit(-1);
	} else { //������
		debug(3, "Create child process pid(%d), fd=%d\n", pstOneProcessInfo->pid, infd[0]);
		pstOneProcessInfo->pipeout = outfd[1];
		pstOneProcessInfo->pipein = infd[0];
		close(outfd[0]);
		close(infd[1]);
		pstOneProcessInfo->time = 0;	//��ʾ������̿���
	}
	return 0;
}




/*��ʼ�������������ӽ���
 *���룺
 *    processNumber----�ӽ��̸���
 *    argv----�ӽ��������������
 *    callback----�õ�һ����������Ĵ�����
 *����ֵ��
 *    ProcessInfoָ��----�ɹ�
 *    NULL----ʧ��
*/
struct ProcessInfo* initProcess(int processNumber, char* const argv[], void (*callback)(void* task, int fd))
{
	//�õ��ӽ��̸�������Ϊ��¼����Ԫ�ط����ڴ�
	struct ProcessInfo* pstProcessInfo;

	if( (pstProcessInfo = (struct ProcessInfo*)malloc(sizeof(struct ProcessInfo))) == NULL )
		return NULL;

	debug(1, "Will init %d process for %s\n", processNumber, argv[0]);
	pstProcessInfo->iProcessNumber = processNumber;
	pstProcessInfo->iFreeProcessNumber = processNumber;
	pstProcessInfo->callback = callback;
	pstProcessInfo->pstOneProcessInfo = (struct OneProcessInfo*)calloc(processNumber, sizeof(struct OneProcessInfo));
	if(NULL == pstProcessInfo->pstOneProcessInfo) {
		free(pstProcessInfo);
		return NULL;
	}
	pstProcessInfo->piFreeProcessIndex = (int*)calloc(processNumber, sizeof(int));
	if(NULL == pstProcessInfo->piFreeProcessIndex) {
		free(pstProcessInfo->pstOneProcessInfo);
		free(pstProcessInfo);
		return NULL;
	}

	int i = 0;
	int ret = 0;
	//�����ӽ���
	for(i=0; i<processNumber; i++) {
		//����һ���ӽ���
		ret = startupOneProcess(pstProcessInfo->pstOneProcessInfo+i, argv);
		if(-1 == ret) {
			debug(1, "start the %d process failed ! for %s\n", i, argv[0]);
			free(pstProcessInfo->pstOneProcessInfo);
			free(pstProcessInfo);
			return NULL;
		}
		debug(1, "start the %d process ok ! for %s\n", i, argv[0]);
		//��¼����ӽ���Ϊ����
		pstProcessInfo->piFreeProcessIndex[i] = i;
	}

	return pstProcessInfo;
}



/*��һ�����̷������ȡ���һ�����еĽ���
 *���룺
 *    command----���ӽ����´������
 *    pstProcessInfo----������Ϣָ��
 *    data----�������������Ϣ
 *�����
 *    rset----��صĶ��ļ�����������
 *����ֵ��
 *    ������----������������ļ�����������
 *    -1----���ش���Ӧ��ֹͣ����
*/
int distributeOneCommand(const char* command, struct ProcessInfo* pstProcessInfo, void* data, fd_set* rset)
{
	//�õ����н���,�����н��̷�����
	int index = pstProcessInfo->piFreeProcessIndex[pstProcessInfo->iFreeProcessNumber-1];
	struct OneProcessInfo * pstOneProcessInfo = pstProcessInfo->pstOneProcessInfo + index;
	int length = strlen(command);
	int offset = 0;
	int ret=0;

	while(offset < length) {
		ret = write(pstOneProcessInfo->pipeout, command+offset, length-offset);
		if ((-1==ret) && (errno==EINTR))
			continue;
		if (-1 == ret)
			return -1;
		offset += ret;
	}

	//��������̱��Ϊ����״̬
	pstOneProcessInfo->time = time(NULL);
	pstOneProcessInfo->task = data;

	//�۲����Ը�fd
	debug(2, "FD Set for fd=%d, free child count =%d\n", pstOneProcessInfo->pipein, pstProcessInfo->iFreeProcessNumber);
	FD_SET(pstOneProcessInfo->pipein, rset);
	pstProcessInfo->iFreeProcessNumber--;

	return pstOneProcessInfo->pipein;
}



/*��һ�����̵�״̬��Ϊ���У���������̵�ʱ���Ϊ0�����Ұ���ŷŵ������б��
 *���룺
 *    pstProcessInfo----������Ϣָ��
 *    index----��������ڽ�����Ϣ�б��е�λ��
*/
static void freeOneProcess(struct ProcessInfo* pstProcessInfo, int index)
{
	if(0 == pstProcessInfo->pstOneProcessInfo[index].time)	//�Ѿ��ǿ��е���
		return;

	pstProcessInfo->pstOneProcessInfo[index].time = 0;
	pstProcessInfo->piFreeProcessIndex[pstProcessInfo->iFreeProcessNumber] = index;
	pstProcessInfo->iFreeProcessNumber++;
}



/*�ȴ��ӽ��������������ɵĻ��ߵ���timeout���أ���������
 *���룺
 *    rset----��ص��ļ����������ϣ�������ɵ�������������ļ���������
 *    maxFd----��ص��ļ������������������ļ�������ֵ
 *    timeout----�ȴ�ʱ�䣬null��ʾ����ʱ
 *    ppstProcessInfo----���������Ϣָ��
 *    processInfoNumber----���м�����ͬ�Ľ���
 *����ֵ��
 *    ������----���������Ŀ����ӽ��̸���
 *    0----timeout����
 *    -1----���ش���
*/
int waitFreeProcess(fd_set* rset, int maxFd, struct timeval* timeout, struct ProcessInfo ** ppstProcessInfo, int processInfoNumber)
{
	int ret = 0;
	struct ProcessInfo * pstProcessInfo = NULL;
	int i=0, j=0;
	int count = 0;

	fd_set rtempset;
	memcpy(&rtempset, rset, sizeof(fd_set));

	//Xcell add
	struct timeval to;
	to.tv_sec = 60;
	to.tv_usec = 0;
	if (timeout != NULL) {
		memcpy(&to, timeout, sizeof(struct timeval));
	}
	//end

	while (1) {
		ret = select(maxFd+1, &rtempset, NULL, NULL, &to);
		debug(2, "select rtn=%d\n", ret);
		if ((-1==ret) && (errno==EINTR))
			continue;
		if (-1 == ret)
			return -1;
		if (0 == ret) {
			//ʱ�䵽��
			debug(2, "timeout for select\n");
			return 0;
		}

		count = 0;

		for (i=0; i<processInfoNumber; i++) {
			pstProcessInfo = ppstProcessInfo[i];
			for (j=0;j<pstProcessInfo->iProcessNumber;j++) {
				if (pstProcessInfo->pstOneProcessInfo[j].time > 0) {
					//ֻҪ�������������Ϊ�ǿ��е�
					//FIXME:���Ӹý��̶�ȡ�������᲻��һֱ��Ϊ�ǿ��е�?
					if(FD_ISSET(pstProcessInfo->pstOneProcessInfo[j].pipein, &rtempset)) {
						count++;

						//������processDetectResult
						pstProcessInfo->callback(pstProcessInfo->pstOneProcessInfo[j].task, pstProcessInfo->pstOneProcessInfo[j].pipein);
						//��־�ý��̿���
						freeOneProcess(pstProcessInfo, j);
						FD_CLR(pstProcessInfo->pstOneProcessInfo[j].pipein, rset);
					}
				}
			}
		}
		return count;
	}
}


/*�ȴ����е��ӽ����������
 *���룺
 *    rset----��ص��ļ�����������
 *    maxFd----��ص��ļ������������������ļ�������ֵ
 *    ppstProcessInfo----���������Ϣָ��
 *    processInfoNumber----���м�����ͬ�Ľ���
*/
void waitAllFreeProcess(fd_set* rset, int maxFd, struct ProcessInfo** ppstProcessInfo, int processInfoNumber)
{
	int i=0;
	int rtn = 0;
	struct ProcessInfo * pstProcessInfo = NULL;

	for(i=0;i<processInfoNumber;i++) {
		pstProcessInfo = ppstProcessInfo[i];
		while (pstProcessInfo->iFreeProcessNumber < pstProcessInfo->iProcessNumber) {
			rtn = waitFreeProcess(rset, maxFd, NULL, ppstProcessInfo, processInfoNumber);
			if (rtn > 0)
				continue;
			else
				return;
		}
	}
}



/*�õ�һ���ӽ���������ӽ��̵ĸ���
 *���룺
 *    pstProcessInfo----������Ϣָ��
 *����ֵ��
 *    ����----�����ӽ��̵ĸ���
*/
int getFreeProcessNumber(const struct ProcessInfo* pstProcessInfo)
{
	return pstProcessInfo->iFreeProcessNumber;
}



/*����һ���ӽ�����������ܵ��ļ���������
 *���룺
 *    pstProcessInfo----������Ϣָ��
 *����ֵ��
 *    ����----�����ܵ��ļ���������
*/
int getMaxFileno(const struct ProcessInfo* pstProcessInfo)
{
	int maxFd = 0;
	int i = 0;

	for(i=0;i<pstProcessInfo->iProcessNumber;i++) {
		if(pstProcessInfo->pstOneProcessInfo[i].pipein > maxFd) {
			maxFd = pstProcessInfo->pstOneProcessInfo[i].pipein;
		}
	}
	return maxFd;
}


void killAllProcess(struct ProcessInfo** ppstProcessInfo, int processInfoNumber)
{
	int i, j;
	struct OneProcessInfo * pstOneProcessInfo = NULL;
	for(i=0;i<processInfoNumber;i++) {
		pstOneProcessInfo = ppstProcessInfo[i]->pstOneProcessInfo;
		for(j=0;j<ppstProcessInfo[i]->iProcessNumber;j++) {
			close(pstOneProcessInfo[j].pipein);
			close(pstOneProcessInfo[j].pipeout);
			kill(pstOneProcessInfo[j].pid, 9);
		}
	}
}
