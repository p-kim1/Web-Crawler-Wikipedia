#include "crawlerFun.h"
using namespace std;

//Deletes all files in "links" folder
void emptyLinks()
{
  DIR *folder = opendir("./links");
  struct dirent *file;
  char filePath[256];
  while((file = readdir(folder)) != NULL)
    {
      if(strcmp(file->d_name,".") == 0 || strcmp(file->d_name,"..") == 0)
	continue;
      sprintf(filePath, "%s%s", "./links/", file->d_name);
      remove(filePath);
    }
  closedir(folder);
}


//Replaces '/' with '_' in filename to prevent write errors
string replaceFS(string fname)
{
  int fs = fname.find("/");
  while(fs != -1)
    {
      fname.replace(fs,1,"_");
      fs = fname.find("/");
    }
  return fname;
}


//Checks for duplicate links
bool dupCheck(const xmlChar *link)
{
  for(vector<const xmlChar *>::iterator iter = seenLinks.begin(); iter != seenLinks.end(); ++iter)
    {
      if(xmlStrcmp(link,*iter) == 0)
	return true;
    }
  return false;
}


//Checks if current link belongs to Wikipedia
void urlPush(const xmlChar *url,int depth)
{
  xmlChar *link = xmlCharStrdup("https://en.wikipedia.org");
  const xmlChar *in = xmlStrstr(url,wiki);
  if(url - in == 0 and !dupCheck(url))
    {
      seenLinks.push_back(url);
      link = xmlStrcat(link,url);
      crawlFrontier.emplace(make_tuple(link,depth+1));
    }
}


//Creates html file from given link
//usleep makes the thread wait 0.25 seconds before getting the next link
string writeHTML(string link, CURL *curl)
{
  string fname = link.substr(link.find("/wiki/")+6) + ".html";
  fname = "links/" + replaceFS(fname);
  FILE *output = fopen(fname.c_str(),"wb");
  usleep(250000);
  curl_easy_setopt(curl, CURLOPT_URL, link.c_str());
  curl_easy_setopt(curl,CURLOPT_NOPROGRESS,1L);
  curl_easy_setopt(curl,CURLOPT_WRITEDATA,output);
  curl_easy_perform(curl);
  fclose(output);
  return fname;
}


//Opens an html file and returns the root node
xmlNode* readHTML(const char *fname)
{
  htmlDocPtr html;
  html = htmlReadFile(fname,NULL,HTML_PARSE_NOBLANKS | HTML_PARSE_NOERROR);
  return xmlDocGetRootElement(html);
}
