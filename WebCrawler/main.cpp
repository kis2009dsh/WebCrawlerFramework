//
//  main.cpp
//  WebCrawler
//
//  Created by dongshihao on 13-11-1.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include <iostream>
#include <regex>
#include <map>
#include <set>
#include <vector>
#include <iterator>
#include <algorithm>
#include <functional>
#include <vector>
#include <forward_list>
#include <thread>
#include <mutex>
#include <atomic>
#include <limits>
#include <memory>

#include "WebCrawler.h"
#include "Timer.h"
#include "CodeConvert.h"
#include "ThreadPool.h"


int main(int argc, const char * argv[])
{
    WebCrawler webCrawler ;
    
    /// Assign Initial Seed
    webCrawler.assign( {
        "http://www.acfun.tv/u/640423.aspx"
    } );
    
    /// Clear Hash Set
    webCrawler.ClearCache();
    
    /// Start Crawler Proc
    webCrawler.StartThreadProc();
    
    std::cout << "------- End -------" << std::endl;
    
    webCrawler.SaveLog();
    
    return 0;
}