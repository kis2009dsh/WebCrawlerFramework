//
//  WebCrawler.h
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __WebCrawler__WebCrawler__
#define __WebCrawler__WebCrawler__

#include <iostream>
#include <queue>
#include <algorithm>
#include <functional>
#include <string>
#include <set>
#include <map>
#include <iterator>
#include <vector>
#include <regex>
#include <thread>
#include <mutex>
#include <chrono>
#include <initializer_list>

#include "HttpServer.h"
#include "UserData.h"

class WebCrawler
{
public:
    typedef std::vector<std::string> ListType;
    typedef int                      ValueType;
    typedef ValueType                HashType ;
    typedef std::set<HashType>       SetType ;
    
private:
    ListType      m_Init_Vec ;
    SetType       m_Set      ;
    MonitorLog    m_Log      ;
    
    static const int MAX_DEPTH = 17;
    
    ///
    /// @brief  depth == 5 : 500  Pages ;
    ///         depth == 6 : 1700 Pages ;
    ///         depth == 7 : 3500 Pages ;
    static const int USER_MAX_DEPTH = 3 ;
    
    static const int USER_INTERVALMILLSECOND = 0 ;
    
public:
    template<typename InIt>
    WebCrawler( InIt first , InIt last )
    :m_Init_Vec(),m_Set() , m_Log()
    {
        for(auto i = first ; i != last;  ++i)
            m_Init_Vec.push_back(*i);
    }
    
    WebCrawler()
    :m_Init_Vec(),m_Set() , m_Log()
    { }
    
    ///
    /// @brief  Clear Hash Set
    ///
    inline void ClearCache() {
        m_Set.clear();
    }
    
    ///
    /// @brief  Get Hash Value Number
    ///
    inline size_t GetHashNumber() const {
        return m_Set.size();
    }
    
    ///
    /// @brief  Save Monitor Log
    ///
    inline void SaveLog() const {
        MonitorLogDao::Save(m_Log);
    }
    
    ///
    /// @brief  Only in one thread!!!!
    /// @param  intervalMillSecond : time interval of download web page. Dont Sleep if less or equal 0
    /// @param  max_deep           : max deep of depth first search { max_deep <= 17 }
    /// @return success or fail
    ///
    bool StartThreadProc( const int max_deep = USER_MAX_DEPTH , const int intervalMillSecond = USER_INTERVALMILLSECOND );
    
    template<typename InIt>
    void assign(InIt first , InIt last )
    {
        for(auto i = first ; i != last;  ++i)
            m_Init_Vec.push_back(*i);
    }
    
    void assign( std::initializer_list<std::string> init )
    {
        for(const auto& i : init)
            m_Init_Vec.push_back(i);
    }
    
protected:
    ///
    /// @param base_str :  Source code of Page
    /// @param userdata    : vector of userdata
    /// @param childdata   : vector of child node
    /// @param url_value   : current url
    /// @return True
    ///
    inline static
    bool solvePageData(const std::string& base_str ,std::vector<UserData>&userdata,std::vector<ValueType>&childdata ,const ValueType& url_value) ;
    
    ///
    /// @param  hash : value to check if visit again
    /// @return True / False
    ///
    inline
    bool checkHashValue( const HashType& hash ) const ;
    
    ///
    /// @param hash : value to insert
    /// @return True if not exist , otherwise False
    inline
    bool insertHashValue( const HashType& hash );
    
    ///
    /// @param hash : value of hash
    /// @return http://www.acfun.tv/u/ + to_string(hash) + .aspx
    ///
    inline static
    std::string getURLFromHash(const HashType& hash );
    
    ///
    /// @param url  : http://www.acfun.tv/u/12345.aspx
    /// @return     : 12345
    ///
    inline static
    HashType    getHashFromURL(const std::string& url);
    
    ///
    /// @param  intervalMillSecond : time interval of download web page
    /// @param  max_deep           : max deep of depth first search { max_deep <= 17 }
    /// @param  url_value          : current url
    /// @param  current_deep       : current deep of search
    /// @return success or fail
    ///
    bool depthFirstSearch(const ValueType& url_value ,const int current_deep ,const int intervalMillSecond ,const int max_deep );
    
    ///
    /// @param intervalMillSecond : time interval of download web page
    /// @return Null
    ///
    inline
    void delayForMillSecond(const int intervalMillSecond) const ;
};


#endif /* defined(__WebCrawler__WebCrawler__) */
