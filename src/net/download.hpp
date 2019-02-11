#pragma once

void netInit();
void netTerm();
void httpInit();
void httpTerm();
void curlDownload(const char *url, const char *dest);
std::string curlDownloadKeepName(char const*const url, std::string dst);