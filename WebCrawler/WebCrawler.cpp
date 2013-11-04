//
//  WebCrawler.cpp
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "WebCrawler.h"
bool WebCrawler::StartThreadProc(const int max_deep ,const int intervalMillSecond )
{
    if( max_deep > MAX_DEPTH ) return false ;
    
    std::chrono::system_clock::time_point start_time = std::chrono::system_clock::now();
    
    /// Search
    bool flag(true);
    for(auto init_url : m_Init_Vec)
    {
        HashType hash_value(getHashFromURL(init_url));
        flag = depthFirstSearch(hash_value, 0 ,  intervalMillSecond , max_deep ) && flag;
    }
    
    /// Save Total Time
    MonitorLogOperator::Log(
                            PerformanceLogStrategy(
                                                   std::chrono::duration_cast<std::chrono::seconds>(
                                                                                                    std::chrono::system_clock::now() - start_time
                                                                                                    ).count(),
                                                   TotalTime ),
                            m_Log );
    
    return flag ;
}

bool WebCrawler::depthFirstSearch(const ValueType &url_value, const int current_deep, const int interval_millisecond, const int max_deep)
{
    if( current_deep >= max_deep )
        return true ;
    
    if (!checkHashValue(HashType(url_value)))
    {
        insertHashValue(HashType(url_value));
        
        /// Fetch Page Info;
        std::string complete_url(getURLFromHash(HashType(url_value)));
        std::string page_data ;
        std::string httpErrorMessage ;
        
        HttpRequest  httpRequest( complete_url , HttpRequestMethod::GET );
        HttpResponse httpResponse ;
        
        {/// Performance Monitor
            std::chrono::system_clock::time_point start_fetch = std::chrono::system_clock::now();
            
            page_data = HttpConnection::SendSyncHttpRequest(httpRequest, httpResponse, httpErrorMessage);
            
            MonitorLogOperator::Log(
                                    PerformanceLogStrategy(
                                                           std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                                                                 std::chrono::system_clock::now() - start_fetch
                                                                                                                 ).count(),
                                                           FetchTime ),
                                    m_Log );
        }
        
        /// Sleep for intervalMillSecond;
        if( interval_millisecond > 0)
            delayForMillSecond(interval_millisecond);
        
        if( httpResponse.GetStatusCode() < 400 )
        {
            std::vector<UserData>     user_data_vec ;
            std::vector<ValueType>    child_data_vec ;
            
            /// Parse from page_data;
            
            { /// Performance Monitor
                std::chrono::system_clock::time_point start_parse = std::chrono::system_clock::now();
                
                if (!solvePageData(page_data, user_data_vec, child_data_vec , url_value))
                    return false ;
                
                MonitorLogOperator::Log(
                                        PerformanceLogStrategy(
                                                               std::chrono::duration_cast<std::chrono::milliseconds>(
                                                                                                                     std::chrono::system_clock::now() - start_parse
                                                                                                                     ).count(),
                                                               ParseTime ),
                                        m_Log );
            }
            
            /// Save user_data into database / Log
            for(const auto& user_data : user_data_vec)
            {
                UserDataDao::Save(user_data);
                MonitorLogOperator::Log(UserDataLogStrategy(user_data), m_Log);
            }
            
            /// Deallocate Memory
            user_data_vec.clear();
            user_data_vec.shrink_to_fit();
            
            complete_url.clear();
            complete_url.shrink_to_fit();
            
            page_data.clear();
            page_data.shrink_to_fit();
            
            /// Recursion;
            while( !child_data_vec.empty() )
            {
                ValueType child_data( child_data_vec.back() );
                
                depthFirstSearch( child_data , current_deep + 1 , interval_millisecond  , max_deep);
                
                /// Remove Useless Memory
                child_data_vec.pop_back();
                child_data_vec.shrink_to_fit();
            }
        }
    }
    
    return true ;
}

bool WebCrawler::solvePageData(const std::string &base_str,
                               std::vector<UserData> &userdata,
                               std::vector<ValueType>&childdata,
                               const ValueType& url_value)
{
    userdata.clear();
    childdata.clear();
    
    std::string base_bak_str(base_str);
    
    /// Remove '\n' and '\r'
    base_bak_str.erase(std::remove(std::begin(base_bak_str), std::end(base_bak_str), '\n'), std::end(base_bak_str));
    base_bak_str.erase(std::remove(std::begin(base_bak_str), std::end(base_bak_str), '\r'), std::end(base_bak_str));
    
    /// Fetch User Data
    ///
    /// @brief <a href="/u/${url_value}.aspx" class="name">UserName</a>
    ///
    ///    <li>真实姓名：曹芝源</li>
    ///    <li>性别：男</li>
    ///    <li>性取向：异性恋</li>
    ///    <li>所在地：河南鹤壁</li>
    ///    <li>注册时间：2012-06-19 18:54:40</li>
    ///    <li>联系电话：未公开</li>
    ///    <li>个人主页：http://weibo.com/czy1sakura</li>
    ///    <li>QQ号码：417620470</li>
    {
        std::string url_value_str(std::to_string(url_value));
        std::string base_user_data_str("<a href=\"/u/" + url_value_str + "\\.aspx\" class=\"name\">([^<]+)</a>") ;
        std::regex base_user_regex( base_user_data_str );
        std::smatch base_user_result ;
        
        if( !std::regex_search(base_bak_str, base_user_result , base_user_regex) )
            return false;
        
        std::string user_name = base_user_result[1];
        
        /// Remove UserData from base_bak_str
        base_bak_str = std::regex_replace(base_bak_str,
                                          base_user_regex ,
                                          "<a href=\"/u/#####\\.aspx\" class=\"name\">#####</a>" );
        
        /// Location
        std::string base_location_str("<li>所在地：([^<]+)</li>");
        std::regex  base_location_regex( base_location_str );
        std::smatch base_location_result;
        
        if( !std::regex_search(base_bak_str , base_location_result , base_location_regex) )
            return false ;
        
        std::string location = base_location_result[1];
        
        
        /// Profile
        std::string base_profile_str("<li>个人主页：([^<]+)</li>");
        std::regex  base_profile_regex( base_profile_str );
        std::smatch base_profile_result ;
        
        if( !std::regex_search(base_bak_str, base_profile_result , base_profile_regex) )
            return false;
        
        std::string profile = base_profile_result[1];
        
        
        /// QQ
        std::string base_qq_str("<li>QQ号码：([^<]+)</li>");
        std::regex  base_qq_regex( base_qq_str );
        std::smatch base_qq_result ;
        
        if (!std::regex_search(base_bak_str , base_qq_result , base_qq_regex) )
            return false ;
        
        std::string qq = base_qq_result[1];
        
        /// RealName
        std::string base_real_str("<li>真实姓名：([^<]+)</li>");
        std::regex  base_real_regex( base_real_str );
        std::smatch base_real_result ;
        
        if (!std::regex_search(base_bak_str, base_real_result , base_real_regex) )
            return false;
        
        std::string realname = base_real_result[1];
        
        /// Sex
        std::string base_sex_str("<li>性别：([^<]+)</li>");
        std::regex  base_sex_regex( base_sex_str );
        std::smatch base_sex_result ;
        
        if(!std::regex_search(base_bak_str, base_sex_result , base_sex_regex))
            return false;
        
        std::string sex = base_sex_result[1];
        
        userdata.push_back(UserData(HashType(url_value), user_name , location ,  profile , qq , realname , sex));
    }
    
    /// Fetch Child Node
    ///
    /// @brief <a href="/u/(\\d+).aspx" class="name">UserName</a>
    ///
    {
        std::string url_value_str("(\\d+)");
        std::string base_child_data_str("<a href=\"/u/" + url_value_str + "\\.aspx\" class=\"name\">([^<]+)</a>");
        std::regex  base_child_regex( base_child_data_str );
        std::smatch base_child_result ;
        
        while ( std::regex_search(base_bak_str, base_child_result , base_child_regex) )
        {
            std::string url       = base_child_result[1];
            ValueType   url_value = std::stoi(url);
            
            childdata.push_back(url_value);
            
            base_bak_str = base_child_result.suffix().str();
        }
    }
    
    return true;
}

bool WebCrawler::checkHashValue(const HashType &hash) const
{
    return m_Set.find(hash) != m_Set.end();
}

bool WebCrawler::insertHashValue(const HashType &hash)
{
    return m_Set.insert(hash).second;
}

std::string WebCrawler::getURLFromHash(const HashType &hash)
{
    //    return std::string("http://www.acfun.tv/u/" + std::to_string(hash) + ".aspx");
    return std::move(std::string("http://www.acfun.tv/u/" + std::to_string(hash) + ".aspx"));
}

WebCrawler::HashType WebCrawler::getHashFromURL(const std::string& url)
{
    std::string base_regex_str("http://www\\.acfun\\.tv/u/(\\d+)\\.aspx");
    
    std::regex base_regex( base_regex_str );
    std::smatch base_result ;
    
    std::regex_search( url , base_result , base_regex) ;
    
    return std::stoi(base_result[1].str());
}

void WebCrawler::delayForMillSecond(const int intervalMillSecond) const
{
    std::this_thread::sleep_for(std::chrono::milliseconds(intervalMillSecond));
}
