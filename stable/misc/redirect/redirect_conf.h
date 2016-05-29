#ifndef REDIRECT_CONF_H
#define REDIRECT_CONF_H

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "regex.h"


enum {
	FAIL_DST_TYPE_NODE = 0,
	FAIL_DST_TYPE_CODE,
	FAIL_DST_TYPE_URL,
};

/*�����ļ���һ����¼*/
struct redirect_conf
{
	char* conf_string;
	char* domain;	//����
	char* cust;		//���ΪNULL����ʾ���Ƚϣ����ΪNONE����ʾurlû����һ��
	int cust_size;	//cult���ȣ�Ϊ��Ч��
	int operate;	//������չ��������other��Ŀǰȡֵ"ip"��"ip_abort"��otherΪNULL��ȡֵ"time"��"ip_time"��"ip_time_abort"��otherΪstruct valid_period
	char* key;		//���룬ΪNULL��ʾû����
	char* key2;		//ͬ�ϣ�Ϊ������ı�ʱ���õ�
	int md5_start;	//urlֻ����md5��32bytes��һС�Σ����ǿ�ʼ
	int md5_length;	//url������md5��32bytes���ĳ���
	int decode;		//�Ƿ�����ˣ������ģ�
	char* replace_host;	//�滻����
	char* replace_dir;	//����һ��Ŀ¼
	char* fail_dst;		//��ƥ���Ĭ��·��
	int fail_dst_type;  //code(403/401) or url
	void* other;		//���ݲ�ͬ��operate�����ӵ�����
	int range_flag;  /* ip filter range */
	int cookie_flag; /* ip filter cookie */
	int cookie_pass; /* ip filter cookie password */
	int return_code; /* return code optimise*/
	int regex_incase;
};
void freeSomeRedirectConf(struct redirect_conf* pstRedirectConf, int number);

#ifdef DUOWAN

struct duowanData
{
	char percentValue;
	char* replaceUrl;
};
#endif
struct valid_period
{
	int seconds_before;
	int seconds_after;
};

struct str_and_length
{
	char* str;
	int len;
};

#define REPLACE_REGEX_SUBSTR_NUMBER 99

struct replace_regex
{
	regex_t reg;
	char** replace;
};

typedef enum {
	IP_FILTER,
	IP_ABORT_FILTER,
	TIME_FILTER,
	TIME1_FILTER,
	TIME2_FILTER,
	IP_TIME_FILTER,
	IP_TIME_ABORT_FILTER,
	REPLACE_HOST_FILTER,
	DENY_FILTER,
	BYPASS_FILTER,
	RID_QUESTION_FILTER,
	OUOU_DECODE_FILTER,
	REPLACE_HOST_ALL_FILTER,
	REPLACE_REGEX_FILTER,
	QQ_MUSIC_FILTER,
	MYSPACE_FILTER,
	SINA_FILTER,
	COOKIE_MD5_FILTER,
	QQ_TOPSTREAM_FILTER,
	TUDOU_FILTER,
	DUOWAN_FILTER,
	QQ_GREEN_HIGH_PERFORM_FILTER,
	KEY_FILTER,
	LIGHTTPD_SECDOWNLOAD_FILTER,
	SDO_FILTER,
	DECODE_FILTER,
	IQILU_FILTER,
	MUSIC163_FILTER,
	SDO_MUSIC_FILTER,
	BAIDU_XCODE_FILTER,
	BOKECC_FILTER,
	PPTV_ANTIHIJACK_FILTER,
	WANGLONG_FILTER,
    OUPENG_FILTER,
    NINETY_NINE_FILTER,
    MICROSOFT_FILTER,
    MICROSOFT_MD5_FILTER,
    MSN_DOWNLOAD_FILTER,
    MFW_MUSIC_FILTER,
    NOKIA_FILTER,
	CWG_FILTER,
	OOYALA_FILTER,
    LONGYUAN_FILTER,
} cc_antihijack_type; 

static const char IP_FILTER_CHAR[] = "ip";	//ip���ˣ�url����ip
static const char IP_ABORT_FILTER_CHAR[] = "ip_abort";	//ip���ˣ�url������ip
static const char TIME_FILTER_CHAR[] = "time";	//ʱ����ˣ�url����ʱ��
static const char TIME1_FILTER_CHAR[] = "time1";	//ʱ����ˣ�url����ʱ�� case3789 url����һ��������ַ���
static const char TIME2_FILTER_CHAR[] = "time2";	//ʱ����ˣ�url����ʱ�� case3789 url����һ��������ַ���
static const char IP_TIME_FILTER_CHAR[] = "ip_time";	//ip���ˣ�url����ip��ʱ��
static const char IP_TIME_ABORT_FILTER_CHAR[] = "ip_time_abort";	//ip���ˣ�url������ip���ǰ���ʱ��
static const char REPLACE_HOST_FILTER_CHAR[] = "replace_host";
static const char DENY_FILTER_CHAR[] = "deny";
static const char BYPASS_FILTER_CHAR[] = "bypass";
static const char RID_QUESTION_FILTER_CHAR[] = "rid_question";
static const char OUOU_DECODE_FILTER_CHAR[] = "ouou_decode";
static const char REPLACE_HOST_ALL_FILTER_CHAR[] = "replace_host_all";
static const char REPLACE_REGEX_FILTER_CHAR[] = "replace_regex";
static const char QQ_MUSIC_FILTER_CHAR[] = "qq_music";
static const char MYSPACE_FILTER_CHAR[] = "myspace";
static const char SINA_FILTER_CHAR[] = "sina";
static const char COOKIE_MD5_FILTER_CHAR[] = "cookie_md5";
static const char QQ_TOPSTREAM_FILTER_CHAR[] = "qq_topstream";
static const char TUDOU_FILTER_CHAR[] = "tudou";
static const char DUOWAN_FILTER_CHAR[] = "random_rewrite";
static const char QQ_GREEN_HIGH_PERFORM_CHAR[] = "qq_green_mp3";
static const char KEY_FILTER_CHAR[] = "key";
static const char LIGHTTPD_SECDOWNLOAD_FILTER_CHAR[] = "lighttpd_secdownload";
static const char SDO_CHAR[] = "sdo";
static const char IQILU_CHAR[] = "iqilu";
static const char SDO_MUSIC_FILTER_CHAR[] = "sdo_music";
static const char ICONV_FILTER_CHAR[] = "iconv";
static const char MUSIC163_FILTER_CHAR[] = "163music";
static const char BAIDU_XCODE_CHAR[] = "baidu_xcode";
static const char WANGLONG_FILTER_CHAR[] = "wanglong";
static const char BOKECC_FILTER_CHAR[] = "bokecc_download";
static const char OUPENG_FILTER_CHAR[] = "oupeng";
static const char NINETY_NINE_CHAR[] = "99pan";
static const char MICROSOFT_FILTER_CHAR[] = "microsoft";
static const char MICROSOFT_MD5_FILTER_CHAR[] = "microsoft_md5";
static const char MSN_FILTER_CHAR[] = "msn_download";
static const char MFW_MUSIC_FILTER_CHAR[] = "mfw_music";
/*added for pptv by chenqi 20120515 end*/
static const char PPTV_ANTIHIJACK_CHAR[] = "pptv_antihijack";
static const char NOKIA_FILTER_CHAR[] = "nokia";
static const char CWG_FILTER_CHAR[] = "TTplay_music";
static const char OOYALA_FILTER_CHAR[] = "ooyala";
static const char LONGYUAN_FILTER_CHAR[] ="longyuan";
int init_redirect_conf(const char* filename, int confNumber);
struct redirect_conf* findOneRedirectConf(const char* domain, const char* cust);
extern void log_output(int level, char * format, ...);

int analyseCookieMd5Cfg(struct redirect_conf *pstRedirectConf);
int analyseQQTopStreamCfg(struct redirect_conf *pstRedirectConf);
int analyseTudouCfg(struct redirect_conf *pstRedirectConf);
int analysisQQGreenMP3Cfg(struct redirect_conf *pstRedirectConf);
int analysisSDOCfg(struct redirect_conf *pstRedirectConf);
int analysisIQILUCfg(struct redirect_conf *pstRedirectConf);
int analysisSDOMusicCfg(struct redirect_conf *pstRedirectConf);
int analysis163MusicCfg(struct redirect_conf *pstRedirectConf);
int analysisBaiduXcodeCfg(struct redirect_conf *pstRedirectConf);
int analysisBokeccCfg(struct redirect_conf *pstRedirectConf);
int analysisOupengCfg(struct redirect_conf *pstRedirectConf);
int analysisNinetyNineCfg(struct redirect_conf *pstRedirectConf);
int analysisMicrosoftCfg(struct redirect_conf *pstRedirectConf);
int analysisMicrosoftCfg_MD5(struct redirect_conf *pstRedirectConf);
int analysisMSNCfg(struct redirect_conf *pstRedirectConf);
int analysisMFWMusicCfg(struct redirect_conf *pstRedirectConf);
int analysisPPTVCfg(struct redirect_conf *pstRedirectConf);
int analysisWanglongCfg(struct redirect_conf *pstRedirectConf);
int analysisNokiaCfg(struct redirect_conf *pstRedirectConf);
int analysisCWGCfg(struct redirect_conf *pstRedirectConf);
int analysisOoyalaCfg(struct redirect_conf *pstRedirectConf);
int analysisLongyuanCfg(struct redirect_conf *pstRedirectConf);
int cc_redirect_check_close(void);
extern int g_iRedirectConfCheckFlag;       //Just for check redirect.conf
extern inline char * xstrtok(char * buf, char * dem);
extern int cc_redirect_check_start(int no_count, int column_num,  char *line);
extern void cc_redirect_check_clean(void);


#endif	//REDIRECT_CONF_H
