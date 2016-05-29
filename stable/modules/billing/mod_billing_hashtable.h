#ifndef HASHTABLE_H_
#define HASHTABLE_H_

#include <stdint.h>

//hashtable���ȣ������Ǽ�¼������entry��Ӧ�ò���ܴ�1024�㹻��
#define TABLESIZE	0x400UL	//1024
#define	HOSTSIZE	4096

extern uint32_t hashtable_entry_count;	//hasntable�ڽڵ�����

struct flux_connection
{
	unsigned long long int	read_size;
	unsigned long long int	write_size;
	uint32_t	connection_times;
};


struct billing_attr
{
	struct	flux_connection	client;
	struct	flux_connection	source;
};


//hash_entry�в�����*data֮���ָ��ָ����ʵ���ݣ��ﵽ�����
//��������ֱ�ӹ��ڽڵ��ϣ���ȫ�Ǵ��������Լ��ڴ���Ƭ�Ŀ���
struct hash_entry
{
	int32_t key;
	int32_t	used;					//bind��++��fd closeǰ����ɾ���ڵ�
	char	host[HOSTSIZE];			//�Ʒ�����
	struct	billing_attr local;		//���ؿͻ��ˣ���preload
	struct	billing_attr remote;	//Զ�̿ͻ���
	struct	billing_attr fc;		//�²�fc��������
	struct	hash_entry *next;
};


//��ʼ��hashtable
extern void hashtable_init();


//����hashtable
extern void hashtable_dest();


//����һ��entry
extern struct hash_entry* hashtable_create(const char* host);


//��һ��entry
extern struct hash_entry* hashtable_get(const char* host);


//
extern void hashtable_delete(struct hash_entry *entry);


//�ⲿͨ��hashtable_entry_count�õ�hashtable�ڽڵ�������������ڴ�
//�ٵ��øú������õ����нڵ�ĵ�ַ
extern uint32_t get_all_entry(struct hash_entry ** entry_array);


#endif /*HASHTABLE_H_*/
