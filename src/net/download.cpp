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

int plFD;
int logFile;
unsigned char logs[64*1024];
CURL *curl;
CURLcode res;

static size_t write_data_to_disk(void *ptr, size_t size, size_t nmemb, void *stream){
  size_t written = sceIoWrite(   *(int*) stream , ptr , size*nmemb);
  return written;
}

void curlDownloadKeepName(const char *url) {
	//do when you have a vita to test;
}

void curlDownload(const char *url, const char *dest) {

	plFD = sceIoOpen( dest , SCE_O_WRONLY | SCE_O_CREAT, 0777);
	logFile = sceIoOpen( "ux0:data/Easy_Plugins/head.txt" , SCE_O_WRONLY | SCE_O_CREAT, 0777);

	curl_global_init(CURL_GLOBAL_ALL);
	curl = curl_easy_init();
	if(curl) {
		curl_easy_reset(curl);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data_to_disk);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &plFD);
		curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, write_data_to_disk);
		curl_easy_setopt(curl, CURLOPT_HEADERDATA, &logFile);
		res = curl_easy_perform(curl);
	}

	sceIoRead(logFile, logs, sizeof(logs));
	std::string	logString(reinterpret_cast< char const* >(logs));
	logString = logString.substr (0,16);
	int logF2 = sceIoOpen( "ux0:data/Easy_Plugins/logF2.txt", SCE_O_WRONLY | SCE_O_CREAT | SCE_O_RDONLY , 0777 );
	sceIoWrite( logF2 , logString.c_str() , sizeof(logString.c_str()));
	sceIoClose(logF2);
	sceIoClose(plFD);
	sceIoClose(logFile);
	curl_easy_cleanup(curl);
	curl_global_cleanup();
}