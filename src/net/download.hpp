#pragma once

void netInit();
void netTerm();
void httpInit();
void httpTerm();
void curlDownload(const char *url, const char *dest);
char *curlDownloadKeepName(char const*const url);