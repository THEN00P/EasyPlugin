#define _GNU_SOURCE
#include <psp2/sysmodule.h>
#include <psp2/kernel/processmgr.h>
#include <psp2/display.h>

#include <psp2/net/net.h>
#include <psp2/net/netctl.h>
#include <psp2/net/http.h>
#include <psp2/libssl.h>

#include <psp2/io/fcntl.h>
#include <curl/curl.h>

#include <stdio.h>
#include <malloc.h>
#include <string>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <stdlib.h>

CURL *curl;
int plFD;

static size_t write_data_to_disk(void *ptr, size_t size, size_t nmemb, void *stream){
  size_t written = sceIoWrite(   *(int*) stream , ptr , size*nmemb);
  return written;
}

typedef struct {
    char        dnld_remote_fname[4096];
    char        dnld_url[4096]; 
    int        dnld_stream;
    int        dbg_stream;
    uint64_t    dnld_file_sz;
} dnld_params_t;

static int get_oname_from_cd(char const*const cd, char *oname)
{
    char    const*const cdtag   = "Content-disposition:";
    char    const*const key     = "filename=";
    int     ret                 = 0;
    char    *val                = NULL;

    /* Example Content-Disposition: filename=name1367; charset=funny; option=strange */

    /* If filename is present */
    strcasestr(cd, key);

    /* Move to value */
    val += strlen(key);

    /* Copy value as oname */
    while (*val != '\0' && *val != ';') {
        //printf (".... %c\n", *val);
        *oname++ = *val++;
    }
    *oname = '\0';
}

static int get_oname_from_url(char const* url, char *oname)
{
    int         ret = 0;
    char const  *u  = url;

    /* Remove "http(s)://" */
    u = strstr(u, "://");
    if (u) {
        u += strlen("://");
    }

    u = strrchr(u, '/');

    /* Remove last '/' */
    u++;

    /* Copy value as oname */
    while (*u != '\0') {
        //printf (".... %c\n", *u);
        *oname++ = *u++;
    }
    *oname = '\0';

    return ret;
}

size_t dnld_header_parse(void *hdr, size_t size, size_t nmemb, void *userdata)
{
    const   size_t  cb      = size * nmemb;
    const   char    *hdr_str= (char *)hdr;
    dnld_params_t *dnld_params = (dnld_params_t*)userdata;
    char const*const cdtag = "Content-disposition:";

    if (!strncasecmp(hdr_str, cdtag, strlen(cdtag))) {
        get_oname_from_cd(hdr_str+strlen(cdtag), dnld_params->dnld_remote_fname);
    }

    return cb;
}

int get_dnld_stream(char const*const fname)
{
    char const*const pre = "ux0:tai/";
    char out[4096];

    snprintf(out, sizeof(out), "%s/%s", pre, fname);

    int fp = sceIoOpen(out, SCE_O_CREAT | SCE_O_WRONLY, 0777);

    return fp;
}

size_t write_cb(void *buffer, size_t sz, size_t nmemb, void *userdata)
{
    int ret = 0;
    dnld_params_t *dnld_params = (dnld_params_t*)userdata;

    if (!dnld_params->dnld_remote_fname[0]) {
        ret = get_oname_from_url(dnld_params->dnld_url, dnld_params->dnld_remote_fname);
    }

    if (!dnld_params->dnld_stream) {
        dnld_params->dnld_stream = get_dnld_stream(dnld_params->dnld_remote_fname);
    }

    ret = sceIoWrite(dnld_params->dnld_stream, buffer, sz+nmemb);
    if (ret == (sz*nmemb)) {
       dnld_params->dnld_file_sz += ret;
    }
    return ret;
}


int curlDownloadKeepName(char const*const url)
{
    CURL        *curl;
    dnld_params_t dnld_params;

    memset(&dnld_params, 0, sizeof(dnld_params));
    strncpy(dnld_params.dnld_url, url, strlen(url));

    curl = curl_easy_init();
    if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, dnld_header_parse);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &dnld_params);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_cb);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &dnld_params);
    }


    curl_easy_perform(curl);

    curl_easy_cleanup(curl);
	curl_global_cleanup();
}

void curlDownload(const char *url, const char *dest) {
	int plFD = sceIoOpen( dest , SCE_O_WRONLY | SCE_O_CREAT, 0777);

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(curl) {
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_disk);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &plFD);

		curl_easy_perform(curl);
	}
	
	sceIoClose(plFD);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}