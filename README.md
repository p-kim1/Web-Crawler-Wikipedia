# Web-Crawler-Wikipedia
A web crawler that downloads Wikipedia article pages as HTML files.

---INSTALLATION---
* This project uses libcurl for URL transfer requests and libxml2 for HTML parsing. Install both with the following commands:
  * sudo apt-get install libcurl-dev
  * sudo apt-get install libxml2-dev
* Also note that the crawler compiles with C++11 to use queues and vectors.


---RUNNING THE SCRIPTS---
* ./serial_crawler
* ./parallel_crawler <number of threads>


---NOTE ABOUT DEPTH---
* Both serial_crawler and parallel_crawler start by requesting a Wikipedia article URL and the maximum crawl depth. The default link is "https://en.wikipedia.org/wiki/Web_crawler" and the default depth is 1. These are sufficient for testing purposes, since increasing the depth causes the number of links to rise exponentially. For example, with a maximum depth of 2, the serial crawler ran for ~30 minutes before needing to be stopped manually due to throttling from Wikipedia. It had obtained 1921 pages totalling 247.0 MB.
