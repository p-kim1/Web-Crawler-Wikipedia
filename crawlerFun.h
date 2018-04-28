#ifndef CRAWLERFUN_H
#define CRAWLERFUN_H

#include <curl/curl.h> //URL requests
#include <libxml/HTMLparser.h> //Parse HTML DOM
#include <queue> //Crawl Frontier
#include <tuple> //Link-Depth pair
#include <vector> //Container for discovered links
#include <string.h> //String functions
#include <dirent.h> //File deletion
#include <unistd.h> //Sleep

extern const xmlChar *wiki;
extern std::queue<std::tuple<const xmlChar*,int>> crawlFrontier;
extern std::vector<const xmlChar *> seenLinks;

void emptyLinks();

std::string replaceFS(std::string fname);

bool dupCheck(const xmlChar *link);

void urlPush(const xmlChar *url,int depth);

std::string writeHTML(std::string link, CURL *curl);

xmlNode* readHTML(const char *fname);

#endif
