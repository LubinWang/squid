#include "mod_billing_hashtable.h"
#include "squid.h"
//����mempool����Ҫ���뿴ͷ�ļ�squid.h
//����hash_table��squid.h��������ĳ��ͷ�ļ��еı�����ͻ�ʽ���������Ϊbilling_hash_table
static struct hash_entry*	billing_hash_table[TABLESIZE];

static MemPool * hash_entry_pool = NULL; //����MemPool����
uint32_t hashtable_entry_count = 0;

static inline int32_t ELFhash(char *host)
{
    uint32_t h = 0;
    uint32_t g;

    while( *host ) {
        h =( h<< 4) + *host++;
        g = h & 0xf0000000L;
        if( g ) h ^= g >> 24;
        h &= ~g;
    }

    return h;
}

static inline void entry_init(struct hash_entry *entry)
{
	memset(entry, 0, sizeof(struct hash_entry));
}

static void * hash_entry_pool_alloc(void)
{
	void * obj = NULL;
	if (NULL == hash_entry_pool)
	{
		hash_entry_pool	= memPoolCreate("mod_billing other_data hash_entry", sizeof(struct hash_entry));
	}
	return obj = memPoolAlloc(hash_entry_pool);
}

void hashtable_init()
{
	uint32_t i;
	for( i = 0 ; i < TABLESIZE ; i++ )
		billing_hash_table[i] = NULL;

	assert(hashtable_entry_count == 0);
}

void hashtable_dest()
{
//	assert(hashtable_entry_count == 0);
// when kill Runcache, when can't connect billingd, hashtable_entry_count!=0
	if(NULL != hash_entry_pool)
	{
		memPoolDestroy(hash_entry_pool);
	}
}

struct hash_entry* hashtable_create(const char* host)
{
	hashtable_entry_count++;
	assert(host);
	//Ϊ�ṹ��hash_entry��MemPool�з����ڴ�
	struct hash_entry *entry = hash_entry_pool_alloc();
	entry_init(entry);
	entry->key = ELFhash((char*)host);
	assert(strlen(host) < 4095);
	strcpy(entry->host, host);

	const uint32_t val = entry->key & (TABLESIZE - 1);

	struct hash_entry *ptr = billing_hash_table[val];

	//������Ϊ�գ������½�ڣ�����
	if(ptr == NULL) {
		billing_hash_table[val] = entry;
	//	billing_hash_table[val]->next = NULL;	//��ʼ����ΪNULL
		return entry;
	}

	struct hash_entry *prev = NULL;

	while( ptr != NULL ) { //��С��������

		int ret = strcmp(ptr->host, entry->host);

		if( ret > 0 ) {	//��ǰ�ڵ��key���ڲ���ڵ㣬���ڴ˴����룬break
			break;
		} else if( ret < 0 ) {	//δ�ҵ�λ�ã���������
			prev = ptr;
			ptr = ptr->next;
		} else {
			//�½�ǰ������ң��Ҳ������½����������host��ײ
			assert(0);
		}
	}

	entry->next = ptr;	// ptr�п���Ϊ��

	if (prev == NULL)	//newPtr��С��������ǰ��
		billing_hash_table[val] = entry;
	else
		prev->next = entry;

	return entry;
}

struct hash_entry* hashtable_get(const char* host)
{
	int32_t key = ELFhash((char*)host);

	const uint32_t val = key & (TABLESIZE-1);

	if( billing_hash_table[val] == NULL )
		return NULL;

	struct hash_entry *ptr = billing_hash_table[val];

		//����hashtable�ϵ�����
	while( ptr != NULL ) {

		int ret = strcmp(ptr->host, host);

		if(ret == 0) {
			//�ҵ�
            return ptr;
		} else if ( ret < 0 ) {
            ptr = ptr->next;
		} else {
			//δ�ҵ�
            return NULL;
		}
	}

	return NULL;
}


void hashtable_delete(struct hash_entry *entry)
{
	hashtable_entry_count--;

	const uint32_t val = entry->key & (TABLESIZE-1);

	struct hash_entry *ptr = billing_hash_table[val], *prev = NULL;

	assert(ptr);

	while(ptr != NULL) {

		int ret = strcmp(ptr->host, entry->host);

		if( ret == 0 ) {	//�ҵ��ýڵ�

			if( prev == NULL ) {
				billing_hash_table[val] = ptr->next;
				//��MemPool��free�ṹ��hash_entry
				memPoolFree(hash_entry_pool, ptr);
				return;
			}


			if( prev != NULL ) {
				prev->next = ptr->next;
				//��MemPool��free�ṹ��hash_entry
				memPoolFree(hash_entry_pool, ptr);
				return;
			}

			assert(0);

		} else if ( ret < 0 ) {
			prev = ptr;
			ptr = ptr->next;
		} else {
			break;
		}
	}

	//������δ�ҵ�
	assert(0);
	return;
}

uint32_t get_all_entry(struct hash_entry ** entry_array)
{
	uint32_t i;
	uint32_t offset = 0;

	for( i = 0 ; i < TABLESIZE ; i++ ) {

		if( billing_hash_table[i] == NULL )
			continue;

		struct hash_entry *entry = billing_hash_table[i];
		while( entry != NULL ) {
			entry_array[offset] = entry;
			offset++;
			entry = entry->next;
		}
	}

	assert( offset == hashtable_entry_count);

	return hashtable_entry_count;
}


