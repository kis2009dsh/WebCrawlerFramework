WebCrawlerFrame
===============

A Web Crawler Framework , which can let programmers fetch and analyze web-pages easily. 


|---- UserData 
         |------- UserData (CPP Class)   Data Logger Model of 'MVC'
         
|---- HttpWrapper
         |------- HttpServer (CPP Class)  Wrapper of curl API 
         
|---- Common
         |------- Algorithm (CPP Class)  common algorithm 
         |------- ThreadPool (CPP Class) lite thread pool 
         |------- CodeConvert (CPP Class) Wrapper of iconv API
         |------- Timer (CPP Class)       Wrapper of std::chrono to count time

|---- WebCrawler (CPP Class)
Main FrameWork of web crawler . you can use regex to define what you want to fetch 

 
