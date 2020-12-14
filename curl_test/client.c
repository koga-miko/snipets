#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <zlib.h>
#include <curl/curl.h>

#define MAX_BUF 			65535
#define GZIP_FILE_NAME		"postdata.gz"

static char wr_buf[MAX_BUF];
static int	wr_index = 0;
static const unsigned char postdata[] = "{\"name\":\"Test\"}\n";
// [参考] gzip圧縮した内容
//{
//	0x1f, 0x8b, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xab, 0x56, 0xca, 0x4b, 0xcc, 0x4d, 
//	0x55, 0xb2, 0x52, 0xf2, 0x48, 0xcc, 0x4d, 0x54, 0xaa, 0xe5, 0x02, 0x00, 0xca, 0x96, 0x05, 0x16, 
//	0x10, 0x00, 0x00, 0x00
//};

static
void dump(const char *text,
          FILE *stream, unsigned char *ptr, size_t size)
{
  size_t i;
  size_t c;
  unsigned int width=0x10;

  fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
          text, (long)size, (long)size);

  for(i=0; i<size; i+= width) {
    fprintf(stream, "%4.4lx: ", (long)i);

    /* show hex to the left */
    for(c = 0; c < width; c++) {
      if(i+c < size)
        fprintf(stream, "%02x ", ptr[i+c]);
      else
        fputs("   ", stream);
    }

    /* show data on the right */
    for(c = 0; (c < width) && (i+c < size); c++)
  	fputc((ptr[i+c]>=0x20) && (ptr[i+c]<0x80)?ptr[i+c]:'.', stream);

    fputc('\n', stream); /* newline */
  }
}

static
int my_trace(CURL *handle, curl_infotype type,
             char *data, size_t size,
             void *userp)
{
  const char *text;
  (void)handle; /* prevent compiler warning */

  switch (type) {
  case CURLINFO_TEXT:
    fprintf(stderr, "== Info: %s", data);
  default: /* in case a new one is introduced to shock us */
    return 0;

  case CURLINFO_HEADER_OUT:
    text = "=> Send header";
    break;
  case CURLINFO_DATA_OUT:
    text = "=> Send data";
    break;
  case CURLINFO_SSL_DATA_OUT:
    text = "=> Send SSL data";
    break;
  case CURLINFO_HEADER_IN:
    text = "<= Recv header";
    break;
  case CURLINFO_DATA_IN:
    text = "<= Recv data";
    break;
  case CURLINFO_SSL_DATA_IN:
    text = "<= Recv SSL data";
    break;
  }

  dump(text, stderr, (unsigned char *)data, size);
  return 0;
}

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp)
{
	 int segsize = size * nmemb;

	 if ((wr_index + segsize) > MAX_BUF) {
		 *(int *)userp = 1;
		 return 0;
	 }

	 memcpy((void *)&wr_buf[wr_index], buffer, (size_t)segsize);

	 wr_index += segsize;

	 wr_buf[wr_index] = '\0';

	 return segsize;
}

int main(void)
{
	CURL *curl;
	CURLcode ret;
	int wr_error = 0;
	struct curl_slist *slist=NULL;
	struct curl_slist *temp=NULL;
	struct stat statBuf;
	gzFile gzf;
	unsigned char *postgzdata = NULL;
	int postgzdata_sz = 0;
	FILE *fp;

	curl = curl_easy_init();
	if (curl == NULL) {
		fprintf(stderr, "curl_easy_init failed.\n");
		return 1;
	}

	// gzip圧縮ファイル化★
	gzf = gzopen(GZIP_FILE_NAME,"w");
	gzputs(gzf, postdata);
	gzclose(gzf);
	
	// gzipファイルサイズ取得★
    if (stat(GZIP_FILE_NAME, &statBuf) != 0)
	{
		return -1;
	}

	postgzdata_sz = statBuf.st_size;
	postgzdata = (unsigned char *)malloc(postgzdata_sz);
	if (NULL == postgzdata)
	{
		return -1;
	}

	// gzipファイルのメモリ展開★
	fp = fopen(GZIP_FILE_NAME, "rb");
	if(NULL == fp)
	{
		free(postgzdata);
		return -1;
	}
	
	fread(postgzdata, postgzdata_sz, 1, fp);
	fclose(fp);

	// 通信デバッグ用コールバック設定
	curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:5000/post");
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&wr_error);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	// ヘッダへ"Accept-Encoding: gzip..."(詳細省略)を付与するための設定★
	curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");

	// ヘッダへのUser-Agentの設定
	slist = curl_slist_append(slist, "User-Agent: curl/7.72.0");

	// ヘッダへの Content-Encoding: gzip の設定★
	slist = curl_slist_append(slist, "Content-Encoding: gzip");

	// ヘッダへの Content-Type: application/json の設定
	slist = curl_slist_append(slist, "Content-Type: application/json");
	
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, slist);

	// POST用データ設定(gzip圧縮データの先頭アドレスとサイズを設定）
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, postgzdata);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, postgzdata_sz);
	
	// 通信内容トレースデバッグ用コールバック設定
	curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

	// 通信実行
	ret = curl_easy_perform(curl);

	printf("ret=%d (write_error=%d)\n", ret, wr_error);

	if (ret == 0) {
		printf("%s\n", wr_buf);
	}

	curl_slist_free_all(slist);
	curl_easy_cleanup(curl);
	free(postgzdata);

	return 0;
}
