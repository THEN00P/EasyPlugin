#ifndef __NET_H__
#define __NET_H__

void netInit();
void netTerm();
void httpInit();
void httpTerm();
void curlDownload(const char *url, const char *dest);

#endif