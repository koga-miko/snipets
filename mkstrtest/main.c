#include<sys/time.h>
#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<unistd.h>
#include<string.h>
 
struct my_tm {
	time_t tim; // yyyymmddhhmmss
	long msec;	// milli sec
};
 
static struct my_tm *get_now_tm(){
	struct my_tm *qt;
	struct tm *tmp;
	struct timeval tv;
 
	qt=(struct my_tm*)malloc(sizeof(struct my_tm));
	if(qt == NULL)return NULL;
	gettimeofday(&tv,NULL);
	tmp=localtime(&tv.tv_sec);
	qt->tim=mktime(tmp);
	qt->msec=tv.tv_usec/1000;
	printf("%04d/%02d/%02d %02d:%02d:%02d:%3d\n",
		tmp->tm_year + 1900, tmp->tm_mon + 1,
		tmp->tm_mday, tmp->tm_hour,
		tmp->tm_min, tmp->tm_sec,
		tv.tv_usec/1000);
	return qt;
}
 
// return milli second
static inline long my_tm_cmptime(struct my_tm* now_t,struct my_tm* prev_t){
	long diff_sec;
	long diff_msec;
	diff_sec=difftime(now_t->tim,prev_t->tim);
	diff_msec=now_t->msec-prev_t->msec;
	return diff_sec*1000+diff_msec;
}

int main()
{
	/*volatile*/ int i;
	/*volatile*/ struct my_tm *q0;
	/*volatile*/ struct my_tm *q1;
	/*volatile*/ struct my_tm *q2;
	/*volatile*/ struct my_tm *q3;
	/*volatile*/ struct my_tm *q4;
	/*volatile*/ struct my_tm *q5;
	/*volatile*/ struct my_tm *q6;
	/*volatile*/ struct my_tm *q7;
	/*volatile*/ long dif;
	/*volatile*/ int size = 1000*500*4;
	/*volatile*/ int copysize = 1;
	/*volatile*/ int mallocnum = 10000;
	/*volatile*/ int malloctestsize = 1;

	q0=get_now_tm();
	/*volatile*/ char *adr_a = malloc(size);
	/*volatile*/ char *adr_b = malloc(size);
	/*volatile*/ char *adr_c = malloc(size);
	/*volatile*/ char *adr_d = malloc(size);

	/*volatile*/ char *adr_e[mallocnum];
	for(i=0;i<mallocnum;i++)
	{
		adr_e[i] = malloc(malloctestsize);
	}

	q1=get_now_tm();
	/*volatile*/ FILE *fp = fopen("/tmp/test","w");
	/*volatile*/ FILE *fp2 = fopen("/tmp/test2","r");
	/*volatile*/ FILE *fp3 = fopen("/home/yoshihama0141/test3","w");
	/*volatile*/ FILE *fp4 = fopen("/home/yoshihama0141/test4","r");
	
	q2=get_now_tm();
	fwrite(adr_a, size, 1, fp);
//	for(i=0;i<size/copysize;i++)
//	{
//		fwrite(&adr_a[i*copysize], copysize, 1, fp);
//	}

	q3=get_now_tm();
	fwrite(adr_c, size, 1, fp3);
//	for(i=0;i<size/copysize;i++)
//	{
//		fwrite(&adr_c[i*copysize], copysize, 1, fp3);
//	}

	q4=get_now_tm();
//	fread(adr_a, size, 1, fp2);
//	for(i=0;i<size/copysize;i++)
//	{
//		fread(&adr_a[i*copysize], copysize, 1, fp2);
//		memcpy(&adr_b[i*copysize], &adr_a[i*copysize], copysize);
//	}
	memcpy(adr_b, adr_a, size);
	q5=get_now_tm();
	fflush(fp);
	fflush(fp3);
	fclose(fp);
	fclose(fp2);
	fclose(fp3);
	fclose(fp4);
	q6=get_now_tm();
	free(adr_a);
	free(adr_b);
	free(adr_c);
	free(adr_d);
	for(i=0;i<mallocnum;i++)
	{
		free(adr_e[i]);
	}

	q7=get_now_tm();

	dif=my_tm_cmptime(q1,q0);
	printf("q0->q1: %d msec!\n",dif);

	dif=my_tm_cmptime(q2,q1);
	printf("q1->q2: %d msec!\n",dif);

	dif=my_tm_cmptime(q3,q2);
	printf("q2->q3: %d msec!\n",dif);

	dif=my_tm_cmptime(q4,q3);
	printf("q3->q4: %d msec!\n",dif);

	dif=my_tm_cmptime(q5,q4);
	printf("q4->q5: %d msec!\n",dif);

	dif=my_tm_cmptime(q6,q5);
	printf("q5->q6: %d msec!\n",dif);

	dif=my_tm_cmptime(q7,q6);
	printf("q6->q7: %d msec!\n",dif);

	free(q1);
	free(q2);
	free(q3);
	free(q4);
	free(q5);

	memcpy(&adr_d[i*copysize], &adr_c[i*copysize], copysize);
	return 0;
}

#define GCF_STR_XXX_1 "xxx"
#define GFC_STR_LEN(str) (sizeof(str) - 1)
#define GCF_STR_SEQNO_FMT   "%d"

int setSeqNo(int seqno, char *strbuf, int strsize)
{
    char seqnostr[16];
    int seqnolen = 0;
    memset(seqnostr, 0, 16);
    snprintf(seqnostr, sizeof(seqnostr),"%d", seqno);
    seqnolen = strlen(seqnostr);
    memcpy(strbuf, seqnostr, seqnolen);
    return seqnolen;
}

void mkGcfStrXXX(int seqno, char *strbuf, int strsize)
{
    int idx = 0;
    memcpy(&strbuf[idx], GCF_STR_XXX_1, GFC_STR_LEN(GCF_STR_XXX_1));
    idx += GFC_STR_LEN(GCF_STR_XXX_1);
    memcpy(&strbuf[idx], GCF_STR_XXX_1, GFC_STR_LEN(GCF_STR_XXX_1));
    idx += GFC_STR_LEN(GCF_STR_XXX_1);
}