#include "mod_billing_cal.h"
#include "mod_billing_hashtable.h"

/*
 *	��Ӧ�ò��¼ÿ��host�������Ƿǳ����ѵģ���Ϊ��Ҫ������
 *	fdtable��¼ÿ��fd������������socket��filefd��pipe ...
 *	��Ӧ�ò�ֻҪ��host����Ӧfd���󶨼���
 */
struct fd2host
{
	struct hash_entry	*billing_node;
	int32_t type;		//�������������ͣ�fc��preload��client ...
	unsigned long long int read_size;
	unsigned long long int write_size;
};


//����¼fdtable_size��fd�������Ļ��������
#define fdtable_size  (1024 * 1024)

struct fd2host  fdtable[fdtable_size];


//mereg ���̵�ַ�˿�
char billing_host[16] = {0};
unsigned short billing_port = 0;

/* �ܵüƷѶ�д���� */
unsigned long long int read_count = 0;
unsigned long long int write_count = 0;

static int sync_socket = -1;			//ͬ��socket���ر�ʱһ��Ҫ��Ϊ-1
static int sync_socket_flag = -1;		//-1׼�����ӣ�0�����У�1���ӳɹ�


MemBuf	billing_send_mb;	//�Ʒ���Ϣ����,ʹ��squid�ṩ�Ļ���MemPool���Ƶ�MemBuf����
static char*	current_send_buff = NULL;	//��ǰ���͵�����

bool inited = false;

static time_t last_ok = 0;  //���һ�η��ͳɹ���ʱ��

extern struct mod_conf_param *dir_config;


//��ʼ���Ʒѹ��ܣ�sync_timeΪÿ������ͬ��һ�μƷ���Ϣ������ÿ���Ӷ�������ͬ��һ�μƷ���Ϣ
//sync_time == 0ʱ�����Ը�������sync_size == 0ʱ�����Ը�����
//time_out��
//squid��reloadʱ���������µ����ú���
void billing_init(const char* host_ip, const unsigned short port)
{
	assert(inited == false);
	assert( host_ip );
	assert(strlen(host_ip) <= 15);

	uint32_t i;
	for( i = 0 ; i < strlen(host_ip) ; i++ ) {
		assert( (host_ip[i] == '.') || ((host_ip[i] >= '0') && (host_ip[i] <= '9')) );
	}

	for( i = 0 ; i < fdtable_size ; i++ ) {
		fdtable[i].billing_node = NULL;
		fdtable[i].type = -1;
		fdtable[i].read_size = 0;
		fdtable[i].write_size = 0;
	}

	hashtable_init();

	strcpy(billing_host, host_ip);
	billing_port = port;

	inited = true;
	last_ok = time(0);
	memBufDefInit(&billing_send_mb); //MemBuf��ʼ��
}


//���ټƷѹ��ܣ�squid��reloadʱ���������µ��ñ�����
void billing_destroy()
{
	assert( inited == true );

	//cleanup ��hook��ǰ, ���´�ʱfdû�ر�.
	//��billing_destroyǰ��Ҫ��֤���е�fd���ر�
	uint32_t i;
//	for( i = 0 ; i < fdtable_size ; i++ ) {
		//��destroyǰ�����мƷ�(ִ�й�bind)��fd��Ӧ�ùرգ�
//		assert( fdtable[i].billing_node == NULL );
//		assert( fdtable[i].type == -1 );

		//������Ӧ�ó����ǼǷѵ�fd�������ڴ��У����п��ܵ�
		//����������Ӧ�ùرա�������������е�fd��closeʱ����destroy������Ҳ�������
		//assert( fdtable[i].read_size == 0 );
		//assert( fdtable[i].write_size == 0 );
//	}

	for( i = 0 ; i < fdtable_size ; i++ ) {
		billing_unbind(i);
	}

	//5�λ�������
	for( i = 0 ; i < 20  ; i++ ) {
		//100ms��ʱ
		if( billing_sync(true) == 4 ) //build_buff() return 4;
			break;
		cc_usleep(100000);
	}

	for( i = 0 ; i < 100 ; i++ ) {
		if( billing_sync(true) == 3 ) //send ok
			break;
		cc_usleep(100);
	}

	hashtable_dest();

	memBufClean(&billing_send_mb); //����MemBuf
	current_send_buff = NULL;
	inited = false;
}


//
//�����Ӵ���
//����ǳ����ӵĻ�����һ��fd�ϻ��в�ͬ��host
//Ҳ����˵����fd close֮ǰ�����ж��host
//��Ҫ��ʾ����bind����
//
void billing_unbind(int fd)
{
	//��fd����û����
	if( fdtable[fd].billing_node == NULL ) {
		//httprequest�쳣������£��ᷢ����δ��bind()���̣�������unbind����
		return;
	}

	//ִ�й�bind���������ƽ���hashtable��
	assert( fdtable[fd].billing_node );
	assert(fdtable[fd].read_size == 0);
	assert(fdtable[fd].write_size == 0);
	assert(fdtable[fd].billing_node->used > 0);

	fdtable[fd].billing_node->used--;
	fdtable[fd].billing_node = NULL;
	fdtable[fd].type = -1;
}


//billing��ʱͬ�����������ӣ�ά����һϵͳ������Լÿ���ӵ���һ��Ϊ��
//time_out Ϊms����time_out�ڣ����뷵��
//����˵��ֻ��ֻӦ�ù�bind��������ʱЧ��Ҫ����bindǰ����������fdtable�У�һ��bind��
//�����ͼ���hashtable�У�������ʱ��������������fd close��ʱ�����Ƶ�hashtable�С�
void billing_bind(const char* host, int fd, int type)
{
	assert(type); //type��offset��Ϊ0���
	//assert( fdtable[fd].billing_node == NULL );
	//assert( fdtable[fd].type == -1 );


//һ����˵�����ֵΪ�ա� ������ĳ��ԭ�� һ���󶨹��ˣ���ô�͸ı�ԭ�а󶨡� ����Ŀǰ�����Ϊ��
	if( fdtable[fd].billing_node != NULL )
		billing_unbind(fd);

	fdtable[fd].billing_node = hashtable_get(host);

	if( fdtable[fd].billing_node == NULL ) {
		fdtable[fd].billing_node = hashtable_create(host);
	}

	fdtable[fd].billing_node->used++;

	struct flux_connection *flux = (void*)fdtable[fd].billing_node + type;
	flux->read_size += fdtable[fd].read_size;
	flux->write_size += fdtable[fd].write_size;
	flux->connection_times++;
	fdtable[fd].read_size = 0;
	fdtable[fd].write_size = 0;
	fdtable[fd].type = type;

	//������������ûֱ����read()��write()����ڴ��ƣ�����Ϊ��Щ��socket������
	//һ��Ҫ��bind֮�������������Ч�ģ��ż���������
	read_count += flux->read_size;
	write_count += flux->write_size;
}



char* fd2host(int fd)
{
	//�鿴��һ���ǰ󶨹���
	assert( fdtable[fd].billing_node );

	return fdtable[fd].billing_node->host;
}


void billing_open(int fd)
{
	assert(fdtable[fd].billing_node == NULL);
	assert(fdtable[fd].read_size == 0);
	assert(fdtable[fd].write_size == 0);
	assert(fdtable[fd].type = -1);
}


void   billing_flux_read(int fd, uint32_t read_size)
{
	//��squid����fd����fd��Ӧ��
	if( fdtable[fd].billing_node == NULL ) {
		fdtable[fd].read_size += read_size;
		return;
	}

	//open��ʱ���Ѿ�fd������ת��host����
	assert(fdtable[fd].read_size == 0);
	assert(fdtable[fd].write_size == 0);
	assert(fdtable[fd].billing_node->used > 0);

	unsigned long long int *flux = (void*)(fdtable[fd].billing_node) + fdtable[fd].type + offsetof(struct flux_connection, read_size);

	*flux = *flux + read_size;

	//������������ûֱ����read()��write()����ڴ��ƣ�����Ϊ��Щ��socket������
	//һ��Ҫ��bind֮�������������Ч�ģ��ż���������
	read_count += read_size;
}


void   billing_flux_write(int fd, uint32_t write_size)
{
	//��squid����fd����fd��Ӧ��
	if( fdtable[fd].billing_node == NULL ) {
		fdtable[fd].write_size += write_size;
		return;
	}

	//open��ʱ���Ѿ�fd������ת��host����
	assert(fdtable[fd].read_size == 0);
	assert(fdtable[fd].write_size == 0);
	assert(fdtable[fd].billing_node->used > 0);

	unsigned long long int *flux = (void*)(fdtable[fd].billing_node) + fdtable[fd].type + offsetof(struct flux_connection, write_size);

	*flux = *flux + write_size;

	//������������ûֱ����read()��write()����ڴ��ƣ�����Ϊ��Щ��socket������
	//һ��Ҫ��bind֮�������������Ч�ģ��ż���������
	write_count += write_size;
}


void billing_close(int fd)
{
	if( fdtable[fd].billing_node == NULL ) {	//�ļ���pipe�ȷǼƷ�(δִ�й�bind)������
		fdtable[fd].read_size = 0;
		fdtable[fd].write_size = 0;
		fdtable[fd].type = -1;
		return;
	}

	//ִ�й�bind���������ƽ���hashtable��
	assert(fdtable[fd].read_size == 0);
	assert(fdtable[fd].write_size == 0);
	assert(fdtable[fd].billing_node->used > 0);



	fdtable[fd].billing_node->used--;
	fdtable[fd].billing_node = NULL;
	fdtable[fd].type = -1;
}


//===========================================================================
//===========================================================================


static void sync_connect()
{
	debug(92,3)("sync_connect enter\n");
	struct sockaddr_in  maddress;
	memset(&maddress, 0, sizeof(struct sockaddr_in));
	inet_aton(billing_host, &maddress.sin_addr);
	maddress.sin_family = AF_INET;
	maddress.sin_port   = htons(billing_port);

	assert(sync_socket == -1);
	assert(sync_socket_flag == -1);

	if( (sync_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
		debug(92,0)("[sync_connect] socket creat error\n");
		sync_socket = -1;
		assert(sync_socket_flag == -1);
		return;
	}

#ifndef	TCP_NODELAY
#define TCP_NODELAY	1
#endif
	int nodelay = 1;
	if( setsockopt(sync_socket, IPPROTO_TCP, TCP_NODELAY, (char*)&nodelay, sizeof(nodelay)) == -1 ) {
		debug(92,0)("[sync_connect] socket setsockopt error\n");
		close(sync_socket);
		sync_socket = -1;
		assert(sync_socket_flag == -1);
		return;
	}

	int socket_status = 0;
	if( (socket_status = fcntl(sync_socket, F_GETFL, 0)) < 0 ) {
		debug(92,0)("[sync_connect] fcntl(sync_socket, F_GETFL, 0) error\n");
		close(sync_socket);
		sync_socket = -1;
		assert(sync_socket_flag == -1);
		return;
	}

	if( fcntl(sync_socket, F_SETFL, socket_status | O_NONBLOCK) != 0 ) {
		debug(92,0)("[sync_connect] fcntl(sync_socket, F_SETFL, socket_status | O_NONBLOCK) error\n");
		close(sync_socket);
		sync_socket = -1;
		assert(sync_socket_flag == -1);
		return;
	}

	int ret;
	if( (ret = connect(sync_socket, (struct sockaddr *)&maddress, sizeof(struct sockaddr))) != 0) {

		if( errno != EINPROGRESS ) { //�첽���ӳ�����ͷ��
			debug(92,0)("[sync_connect] connect error\n");
			close(sync_socket);
			sync_socket = -1;
			assert(sync_socket_flag == -1);
			return;
		}
	}

	//��ʱ�����Ƿ����ӳɹ�������������check_connect���̣�����Ҫ��socket��ط�����
		// 0 - connecting, 1 - ready to write/read
	assert(sync_socket);
	sync_socket_flag = 0;
}

static void check_connect()
{
	assert(sync_socket >= 0);
	assert(sync_socket_flag == 0);

	int error;
	socklen_t e_len = sizeof(error);

	if( getsockopt(sync_socket,SOL_SOCKET,SO_ERROR,&error,&e_len) < 0 || error != 0 ) {
		debug(92,0)("[check_connect] getsockopt error\n");
		close(sync_socket);
		sync_socket_flag = -1;
		sync_socket = -1;
		return;
	}


	struct sockaddr_in sin;
	int len = sizeof(struct sockaddr_in);
	if(getsockname(sync_socket,(struct sockaddr *)&sin, (socklen_t *)&len) == 0)
	{
		int port = ntohs(sin.sin_port);
		if(port == billing_port)
		{
			debug(92,0)("[check_connect] getsockname error\n");
			close(sync_socket);
			sync_socket_flag = -1;
			sync_socket = -1;
			return;
		}
	}


/*  ԭ���Ƿ��������ӣ�����д����select��֤��ʱ
    �ָ�Ϊ���������ӣ���������д�������������

	int socket_status = 0;
	if( (socket_status = fcntl(sync_socket, F_GETFL, 0)) < 0 ) {
		close(sync_socket);
		sync_socket = -1;
		sync_socket_flag = -1;
		return;
	}

	if( fcntl(sync_socket, F_SETFL, socket_status & (!O_NONBLOCK)) != 0 ) {
		close(sync_socket);
		sync_socket = -1;
		sync_socket_flag = -1;
		return;
	}
*/
	//���ӳɹ�
    sync_socket_flag = 1;
}


static int is_entry_empty(struct hash_entry *entry)
{
    if (NULL == entry)
        return -1;
    if (entry->local.client.read_size != 0)
        return -1;
    if (entry->local.client.write_size != 0)
        return -1;
    if (entry->local.source.read_size != 0)
        return -1;
    if (entry->local.source.write_size != 0)
        return -1;
    if (entry->remote.client.read_size != 0)
        return -1;
    if (entry->remote.client.write_size != 0)
        return -1;
    if (entry->remote.source.read_size != 0)
        return -1;
    if (entry->remote.source.write_size != 0)
        return -1;
    if (entry->fc.client.read_size != 0)
        return -1;
    if (entry->fc.client.write_size != 0)
        return -1;
    if (entry->fc.source.read_size != 0)
        return -1;
    if (entry->fc.source.write_size != 0)
        return -1;
    return 0;
}

static void reset_entry(struct hash_entry *entry)
{
    if (NULL == entry)
        return;
    entry->local.client.read_size = 0;
    entry->local.client.write_size = 0;
    entry->local.source.read_size = 0;
    entry->local.source.write_size = 0;
    entry->remote.client.read_size = 0;
    entry->remote.client.write_size = 0;
    entry->remote.source.read_size = 0;
    entry->remote.source.write_size = 0;
    entry->fc.client.read_size = 0;
    entry->fc.client.write_size = 0;
    entry->fc.source.read_size = 0;
    entry->fc.source.write_size = 0;
    entry->local.client.connection_times = 0;
    entry->local.source.connection_times = 0;
    entry->remote.client.connection_times = 0;
    entry->remote.source.connection_times = 0;
    entry->fc.client.connection_times = 0;
    entry->fc.source.connection_times = 0;
}


static void write_buff(struct hash_entry *entry)
{
    // filter the channels configured
    if (!strcmp(dir_config->type,"filter")){
        wordlist *pC = dir_config->channel;
        while (pC)
        {   
            if ( !strncmp(entry->host, pC->key, sizeof(entry->host)) )
                return;
            pC = pC->next;
        }  
    }

    // memBufPrintf called
    memBufPrintf(&billing_send_mb, "%s\t",   entry->host);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->local.client.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->local.client.write_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->local.source.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->local.source.write_size);
    memBufPrintf(&billing_send_mb, "%u\t",   entry->local.client.connection_times);
    memBufPrintf(&billing_send_mb, "%u\t",   entry->local.source.connection_times);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->remote.client.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->remote.client.write_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->remote.source.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->remote.source.write_size);
    memBufPrintf(&billing_send_mb, "%u\t",   entry->remote.client.connection_times);
    memBufPrintf(&billing_send_mb, "%u\t",   entry->remote.source.connection_times);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->fc.client.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->fc.client.write_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->fc.source.read_size);
    memBufPrintf(&billing_send_mb, "%llu\t", entry->fc.source.write_size);
    memBufPrintf(&billing_send_mb, "%u\t",   entry->fc.client.connection_times);
    memBufPrintf(&billing_send_mb, "%u\n",   entry->fc.source.connection_times);
}

void build_buff()
{
	assert(billing_send_mb.size == 0);
	assert(current_send_buff == NULL);

	if( hashtable_entry_count == 0 )
		return;


	struct hash_entry **entry_array;
	entry_array = xmalloc(sizeof(struct hash_entry*) * hashtable_entry_count);

//	printf("hashtable_entry_count = %u\n", hashtable_entry_count);
	debug(92,3)("build_buff:hashtable_entry_count = %d \n",hashtable_entry_count);

	get_all_entry(entry_array);

	current_send_buff = billing_send_mb.buf;

	uint32_t i;
	uint32_t entry_count = hashtable_entry_count;
	for( i = 0 ; i < entry_count ; i++ ) 
    {
        if (-1 == is_entry_empty(entry_array[i]))
            write_buff(entry_array[i]);
		if (entry_array[i]->used == 0) 
			hashtable_delete(entry_array[i]);
        else
            reset_entry(entry_array[i]);
	}

	xfree(entry_array);
	//assert(billing_send_mb.size); 
	//assert(current_send_buff);   
    if (0 == billing_send_mb.size)      // buff can be empty
        current_send_buff = NULL;
    else
        assert(current_send_buff);   

}


int send_buff()
{
	assert(sync_socket >= 0);
	assert(sync_socket_flag == 1);
	assert(billing_send_mb.size);
	assert(current_send_buff);


	debug(92,3)("send buffer enter\n");
	ssize_t ret;
	if( (ret = write(sync_socket, current_send_buff, billing_send_mb.size)) < 0 ) {

		debug(92, 3) ("(mod_billing: send_buff(): %s\n",xstrerror());
		if( errno == EINTR )
			return -1;

		if( errno == EAGAIN )
			return -1;

		close(sync_socket);
		sync_socket = -1;
		sync_socket_flag = -1;
		return -1;
	}

	debug(92,5)("billing_left_len =%lu , ret =%ld\n",(long unsigned int)billing_send_mb.size,(long)ret);
	//û�з���
	if( ret > 0 ) {

		current_send_buff += ret;
		billing_send_mb.size -= ret;
	}
	if (billing_send_mb.size > 0)
		return ret;

	debug(92,5)("A:billing_left_len =%lu , ret =%ld\n",(long unsigned int)billing_send_mb.size,(long)ret);

	//buff�������
	assert(billing_send_mb.size == 0);
	memBufReset(&billing_send_mb);
	current_send_buff = NULL;

	return ret;
}

int32_t billing_sync(bool donow)
{
	int32_t ret = 0;

	if( (time(0) - last_ok) > 300)
	{
		debug(92,3)("billing_sync error override 300 seconds\n");
	}

	//δ���ᣬ������
	if( sync_socket_flag == -1 ) {
		assert(sync_socket == -1);	//socketһ���ǹر���
		sync_connect();
		ret = 1;
		goto out;
	}

	assert( sync_socket_flag >= 0 );
	assert(sync_socket >= 0);

	//ִ�й����ӣ���������Ƿ�ɹ�
	if( sync_socket_flag == 0 ) {
		assert(sync_socket >= 0);	//socketһ���Ǵ���
		check_connect();
		ret = 2;
		goto out;
	}

	assert(sync_socket_flag == 1);
	assert(sync_socket >= 0);

	//������������δ�����ߵ�����
	if( billing_send_mb.size != 0 ) {
		if(send_buff() != -1)
		{
			last_ok = time(0);

		}
		ret = 3;
		goto out;
	}


	assert(billing_send_mb.size == 0);
	assert(sync_socket >= 0);
	assert(sync_socket_flag == 1);

	build_buff();
	ret = 4;
	goto out;


	//������ɣ�����û���µ�������
out:
	return ret;
}

/*
 * case 2954: customize for wasu TV 
 * forbbiden billing serveral channels
 * written by chenqi @ Mar. 29, 2012
*/
void parse_list()
{
    if (NULL == dir_config)
        return;
    if (strcmp(dir_config->type, "filter"))
    {   
        debug(92,1)("mod_billing: Cannot recognize keyword: %s\n",dir_config->type);
        return;
    }  

    FILE *fp = NULL;
    if ( NULL == (fp = fopen(dir_config->black_list_path, "r")) )
    {   
        debug(92,1)("mod_billing: Cannot open file %s\n",dir_config->black_list_path);
        return;
    }   

    char config_input_line[256];
    char *ptr;
    int channel_num = 0;
    while (fgets(config_input_line, BUFSIZ, fp) != NULL)
    {
        // ignore black line
        if ( NULL == (ptr = strtok(config_input_line, w_space)))
            continue;
        wordlistAdd(&dir_config->channel,ptr);
        channel_num++;
    }

    fclose(fp);
    fp = NULL;

    if (!channel_num)
        debug(92,2)("mod_billing: the file %s is empty\n", dir_config->black_list_path);
    else
        debug(92,3)("mod_billing: we have read %d channel from file %s\n", channel_num, dir_config->black_list_path);
    return;
}


