#include "crawlerFun.h"
#include <iostream>
#include <time.h>
using namespace std;

const xmlChar *rootLink = xmlCharStrdup("https://en.wikipedia.org/wiki/Web_crawler");
const xmlChar *wiki = xmlCharStrdup("/wiki/");
const xmlChar *href = xmlCharStrdup("href");
const xmlChar *a = xmlCharStrdup("a");
std::queue<std::tuple<const xmlChar*,int>> crawlFrontier;
std::vector<const xmlChar *> seenLinks;
int max_depth = 1;

//Breadth First Search through the current HTML file to obtain links
void crawl(xmlNode *node, int depth)
{
  const xmlChar *url;
  xmlNode *cur_node = NULL;

  if(NULL == node)
    {
      return;
    }
  for (cur_node = node; cur_node; cur_node = cur_node->next) 
    {
      if (cur_node->type == XML_ELEMENT_NODE and xmlStrEqual(cur_node->name,a)) 
	{
	  for(xmlAttrPtr attr = cur_node->properties; NULL != attr; attr = attr->next)
	    {
	      if(xmlStrEqual(attr->name,href))
		{
		  url = xmlGetProp(cur_node,attr->name);
		  urlPush(url,depth);
		}
	    }
	}
      crawl(cur_node->children,depth);
    }
}


//A serial web crawler for Wikipedia that saves all discovered article links as HTML files
//Note that the crawling time is affected by both bandwidth and thread count
int main(int argc, char** argv)
{
  emptyLinks();

  string inputLink = "";
  cout << "Enter a wikipedia article link (or press enter to use default link)" << endl;
  cout << ">";
  getline(cin,inputLink);
  if(!inputLink.empty())
    rootLink = xmlCharStrdup(inputLink.c_str());
  const xmlChar *rootPath = xmlStrstr(rootLink,wiki);
  
  string inputDepth = "";
  cout << "Enter a maximum depth to crawl (or press enter to use default depth)" << endl;
  cout << ">";
  getline(cin,inputDepth);
  if(!inputDepth.empty())
    max_depth = stoi(inputDepth);

  crawlFrontier.emplace(make_tuple(rootLink,0));
  seenLinks.push_back(rootPath);
  curl_global_init(CURL_GLOBAL_ALL);
  CURL *curl = curl_easy_init();
  time_t start, end, crawlTime;

  time(&start);
  while(!crawlFrontier.empty())
    {
      tuple<const xmlChar*,int> link_depth = crawlFrontier.front();
      crawlFrontier.pop();
      const xmlChar* link = get<0>(link_depth);
      int depth = get<1>(link_depth);
      if(depth <= max_depth)
	{
	  string sLink((char *)link);
	  string fname = writeHTML(sLink,curl);
	  xmlNode *node = readHTML(fname.c_str());
	  crawl(node,depth);
	  cout << sLink << " | Depth: " << depth << endl;
	}
    }
  time(&end);
  crawlTime = difftime(end,start);
    
  cout << "---CRAWLING COMPLETED---" << endl;
  cout << "Threads: 1 | Time: " << crawlTime << " secs" << endl;
  curl_easy_cleanup(curl);
  curl_global_cleanup();
  return 0;
}
