/*
 * =====================================================================================
 *
 *       Filename:  mail.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  04/06/2010 09:01:01 AM CST
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  kain (kain), kain@taomee.com
 *        Company:  TAOMEE
 *
 * =====================================================================================
 */

#include <statistic_agent/msglog.h>
#include "util.h"
#include "mail.h"
#include "dbproxy.h"
#include "exclu_things.h"

#define MAX_MAIL_SIZE  64

static int mail_count = 0;
static mail_t mail_all[MAX_MAIL_SIZE];
uint32_t now_mails_bit = 0;

static int search_mail(int mail_id)
{
	int L = 0;
	int H = mail_count;
	int M = mail_count / 2;

	while(L < H) {
		if (mail_id == mail_all[M].mail_id) {
			return M;
		} else if (mail_id > mail_all[M].mail_id) {
			L = M + 1;
			M = (L + H) / 2;
		} else {
			H = M;
			M = (L + H) / 2;
		}
	}

	return MAX_MAIL_SIZE + M;//此处返回用于查找插入位置
}

static int add_mail(mail_t *pmail)
{
	int index = search_mail(pmail->mail_id);
	if(index >= MAX_MAIL_SIZE) {
		index -= MAX_MAIL_SIZE;
		if(index < mail_count) {
			memmove(mail_all + index + 1, mail_all + index, (mail_count - index) * sizeof(mail_t));
		}
		mail_all[index] = *pmail;
		mail_count++;

		return 0;
	}

	return -1;
}

int load_mail_conf(const char *file)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	int err = -1;

	int today;
	//int max = -1;
	//int min = 32;
	uint32_t bitmap = 0;
	mail_t tmpmail = {0};

	doc = xmlParseFile (file);
	if (!doc) {
		ERROR_LOG ("parse %s failed", file);
		goto exit;
	}

	cur = xmlDocGetRootElement(doc);
	if (!cur) {
		ERROR_LOG ("parse %s get root failed", file);
		goto exit;
	}

    mail_count = 0;
	today = get_today();
	cur = cur->xmlChildrenNode;
	while (cur) {
		if ((!xmlStrcmp(cur->name, (const xmlChar *)"Mail"))) {
			memset(&tmpmail, 0, sizeof(mail_t));
			DECODE_XML_PROP_INT(tmpmail.date_end, cur, "EndDate");
			DECODE_XML_PROP_INT_DEFAULT(tmpmail.date_start, cur, "StartDate", today);
 			if(tmpmail.date_end > tmpmail.date_start && tmpmail.date_end > today) {
				DECODE_XML_PROP_INT(tmpmail.mail_id, cur, "MailID");
				DECODE_XML_PROP_INT_DEFAULT(tmpmail.mail_bit, cur, "MailBit",0);
				DECODE_XML_PROP_STR(tmpmail.send_by, cur, "SendBy");
				DECODE_XML_PROP_STR(tmpmail.msg_fmt, cur, "MsgFmt");
				if(mail_count >= MAX_MAIL_SIZE) {
					ERROR_LOG ("too many mails, Count=%d < MAX=%d, mail_id=%d", mail_count, MAX_MAIL_SIZE, tmpmail.mail_id);
					goto exit;
				}
				if(tmpmail.mail_bit > 32) {
					ERROR_LOG ("mail bit=%d > MAX=32", tmpmail.mail_bit);
					goto exit;
				}
				if(add_mail(&tmpmail)) {
					ERROR_LOG ("mail confict mail_id=%d", tmpmail.mail_id);
					goto exit;
				}
				if(tmpmail.mail_bit) {
					int bit = tmpmail.mail_bit - 1;

					//如果bit位和bit位偏移16的位置上已经有值的话
					if((bitmap & (1 << bit)) || (bitmap & (1 << ((bit+16) % 32)))) {
						ERROR_LOG ("mail confict mail_bit=%d", tmpmail.mail_bit);
						goto exit;
					}

					bitmap |= (1 << bit);
				}
			}
		}
		cur = cur->next;
	}

	now_mails_bit = bitmap;
	err = 0;
exit:
	xmlFreeDoc (doc);
	BOOT_LOG (err, "Load mails config file %s", file);
}

/*
 * sprite_t *p	:用户指针，非null,必须可读写
 * int check	:发送条件，为0时，直接返回，不发送邮件
 * int mail_id	:邮件的ID
 * args		:必须和配置文件的的参数个数及类型一致，避免不可预知的错误
 */

int send_mail_to_self(sprite_t *p, int check, int mail_id, ...)
{
	va_list ap;
	va_start(ap, mail_id);

	int today = get_today();
	char mail_msg[2048] = {0};

	if(!check) return 0;

	int index = search_mail(mail_id);
	if(index >= MAX_MAIL_SIZE) {
		ERROR_LOG ("[%u] not exsit mail_id=%d", p->id, mail_id);
		return 0;
	}

	mail_t *pmail = mail_all + index;

	if( today >= pmail->date_end ||
		today < pmail->date_start) {
		DEBUG_LOG("[%u] today=%d not in [%d,%d)", p->id, today, pmail->date_start, pmail->date_end);
		return 0;
	}

	if(pmail->mail_bit) {
		int bit = pmail->mail_bit - 1;
		if(p->sendmail_flag & (1 << bit)) {
			DEBUG_LOG("[%u] mail have send mail_bit=%d", p->id, bit);
			return 0;
		}
		p->sendmail_flag |= (1 << bit);

		bit = bit + 1;
		send_request_to_db(SVR_PROTO_SET_SENDMAIL_FLAG,NULL,sizeof(bit),&bit,p->id);
	}

	vsnprintf(mail_msg, 2047, pmail->msg_fmt,ap);
	va_end(ap);

	send_postcard(pmail->send_by, 0, p->id, pmail->mail_id, mail_msg, 0);
	return 0;
}


