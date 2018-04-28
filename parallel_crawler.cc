#include "crawlerFun.h"
#include <iostream>
#include <omp.h>
using namespace std;

string rootLink = "https://en.wikipedia.org/wiki/Web_crawler";
const xmlChar *wiki = xmlCharStrdup("/wiki/");
const xmlChar *href = xmlCharStrdup("href");
const xmlChar *a = xmlCharStrdup("a");
queue<tuple<const xmlChar*,int>> crawlFrontier;
vector<const xmlChar *> seenLinks;
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

		  //One thread must push to queue and vector at a time
		  #pragma omp critical
		  {
		    urlPush(url,depth);
		  }

		}
	    }
	}
      crawl(cur_node->children,depth);
    }
}


//A multi-threaded web crawler for Wikipedia that saves all discovered article links as HTML files
//Note that the crawling time is affected by both bandwidth and thread count
int main(int argc, char** argv)
{
  xmlInitParser();
  int threads = atoi(argv[1]);
  omp_set_num_threads(threads);
  
  emptyLinks();

  string inputLink = "";
  cout << "Enter a wikipedia article link (or press enter to use default link)" << endl;
  cout << ">";
  getline(cin,inputLink);
  if(!inputLink.empty())
    rootLink = inputLink;
  
  string inputDepth = "";
  cout << "Enter a maximum depth to crawl (or press enter to use default depth)" << endl;
  cout << ">";
  getline(cin,inputDepth);
  if(!inputDepth.empty())
    max_depth = stoi(inputDepth);

  double start, crawlTime;
  int depth = 1;
  curl_global_init(CURL_GLOBAL_ALL);

  start = omp_get_wtime();
  #pragma omp parallel
  {
    CURL *curl = curl_easy_init();
 
    //One thread must crawl the initial link to fill the shared queue with links
    #pragma omp master
    {
      string fnameInit = writeHTML(rootLink,curl);
      xmlNode *nodeInit = readHTML(fnameInit.c_str());
      crawl(nodeInit,0);
      cout << rootLink << " | Depth: " << 0 << endl;
    }
    #pragma omp barrier
    
    while(1)
      {
	tuple<const xmlChar*,int> link_depth;
	const xmlChar* link;

	//Only one thread should pop the queue at a time
        #pragma omp critical
	{
	  if(!crawlFrontier.empty())
	    {
	      link_depth = crawlFrontier.front();
	      crawlFrontier.pop();
	      link = get<0>(link_depth);
	      depth = get<1>(link_depth);
	    }
	}
	if(depth <= max_depth)
	  {
	    string sLink((char *)link);
	    string fname = writeHTML(sLink,curl);
	    xmlNode *node = readHTML(fname.c_str());
	    cout << sLink << " | Depth: " << depth << " | Thread: " << omp_get_thread_num() << endl;
	    crawl(node,depth);
	  }
	else
	    break;
      }
    curl_easy_cleanup(curl);
  }
  crawlTime = omp_get_wtime() - start;
  
  curl_global_cleanup();
  cout << "---CRAWLING COMPLETED---" << endl;
  cout << "Threads: " << threads << " | Time: " << crawlTime << " secs" << endl;
  return 0;
}
