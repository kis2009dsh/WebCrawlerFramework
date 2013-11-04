//
//  UserData.cpp
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "UserData.h"
void UserDataDao::Save(const UserData& user_data)
{
    /// Save to DataBase or File
    std::cout << "http://www.acfun.tv/u/" + std::to_string(user_data.GetHash()) + ".aspx" << "    " << user_data.GetUserName();
    
    if( user_data.GetSex() != "" )
        std::cout << "                " << user_data.GetSex() ;
    
    if( user_data.GetLocation() != "")
        std::cout << "           所在地: " << user_data.GetLocation() ;
    
    if( user_data.GetRealName() != "")
        std::cout << "           真实姓名: " << user_data.GetRealName() ;
    
    if( user_data.GetQQ() != "")
        std::cout << "               qq: " << user_data.GetQQ() ;
    
    if( user_data.GetProfile() != "")
        std::cout << "            个人主页: " << user_data.GetProfile() ;
    
    std::cout << std::endl;
}

void UserDataDao::Update(const UserData &userdata)
{
    // Nothing
}

void UserDataDao::Delete(const UserData &userdata)
{
    // Nothing
}

///////////////////////////////////////////////////////////

void MonitorLogDao::Save(const MonitorLog &log)
{
    std::cout << "总用时      : " << log.GetTotalTime() << " 秒" << std::endl;
    std::cout << "最大抓取页面用时 : " << log.GetParseTimeInf() << " 毫秒" << std::endl;
    std::cout << "最大解析页面用时 : " << log.GetFetchTimeInf() << " 毫秒" << std::endl;
    std::cout << "遍历总用户数 : " << log.GetTotalUser() << std::endl;
    std::cout << "公开男性 : " << log.GetManUser() << std::endl;
    std::cout << "公开女性 : " << log.GetWomanUser() << std::endl;
    std::cout << "公开联系方式男性 : " << log.GetManValidUser() << std::endl;
    std::cout << "公开联系方式女性 : " << log.GetWomanValidUser() << std::endl;
}

void MonitorLogDao::Update(const MonitorLog &log)
{
    // Nothing
}

void MonitorLogDao::Delete(const MonitorLog &log)
{
    // Nothing
}

///////////////////////////////////////////////////////////

void UserDataLogStrategy::operator()(MonitorLog &log) const
{
    size_t total_user = log.GetTotalUser();
    size_t man_user   = log.GetManUser();
    size_t woman_user = log.GetWomanUser();
    size_t man_valid_user = log.GetManValidUser();
    size_t woman_valid_user = log.GetWomanValidUser();
    
    ++total_user;
    
    if(user_data.GetSex() == "男")
    {
        ++man_user;
        
        if ( user_data.GetQQ() != "" || user_data.GetProfile() != "" )
        {
            ++man_valid_user;
        }
    }
    else if(user_data.GetSex() == "女")
    {
        ++woman_user;
        
        if ( user_data.GetQQ() != "" || user_data.GetProfile() != "" )
        {
            ++woman_valid_user;
        }
    }
    else
    {
        // Nothing
    }
    
    log.SetTotalUser(total_user);
    log.SetManUser(man_user);
    log.SetWomanUser(woman_user);
    log.SetManValidUser(man_valid_user);
    log.SetWomanValidUser(woman_valid_user);
}

void PerformanceLogStrategy::operator()(MonitorLog &log) const
{
    switch (perf_value)
    {
        case TotalTime:
            log.SetTotalTime(last_time);
            break;
            
        case ParseTime:
            if( last_time > log.GetParseTimeInf() )
                log.SetParseTimeInf(last_time);
            break;
            
        case FetchTime:
            if( last_time > log.GetFetchTimeInf() )
                log.SetFetchTimeInf(last_time);
            break;
            
        default:
            break;
    }
}

