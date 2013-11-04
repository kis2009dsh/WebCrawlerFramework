//
//  UserData.h
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#ifndef __WebCrawler__UserData__
#define __WebCrawler__UserData__

#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
#include <memory>
#include <chrono>

class UserData
{
private:
    int url_hash           ;
    std::string m_username , m_location , m_profile , m_qq , m_realname , m_sex ;
public:
    UserData(){
        // Nothing
    }
    
    UserData(int hash ,
             const std::string& name ,
             const std::string& location,
             const std::string& profile ,
             const std::string& qq ,
             const std::string& realname,
             const std::string& sex)
    :url_hash(hash),m_username(name) // , m_location(location) , m_profile(profile),m_qq(qq),m_realname(realname),m_sex(sex)
    {
        SetLocation(location);
        SetProfile(profile);
        SetQQ(qq);
        SetRealName(realname);
        SetSex(sex);
    }
    
    inline int GetHash() const {
        return url_hash;
    }
    
    inline bool SetHash(int hash) {
        url_hash = hash;
        
        return true ;
    }
    
    inline std::string GetUserName() const {
        return m_username;
    }
    
    inline bool SetUserName(const std::string& name) {
        m_username = name;
        return true;
    }
    
    inline std::string GetProfile() const {
        return m_profile;
    }
    
    inline bool SetProfile(const std::string& profile) {
        m_profile = ((profile == "未公开")?(""):(profile));
        return m_profile != "";
    }
    
    inline std::string GetQQ() const {
        return m_qq;
    }
    
    inline bool SetQQ(const std::string& qq) {
        m_qq = ((qq == "未公开")?(""):(qq));
        return m_qq != "";
    }
    
    inline std::string GetRealName() const {
        return m_realname;
    }
    
    inline bool SetRealName(const std::string& realname) {
        m_realname = ((realname == "未公开")?(""):(realname));
        return m_realname != "";
    }
    
    inline std::string GetLocation() const {
        return m_location;
    }
    
    inline bool SetLocation(const std::string& location) {
        m_location = ((location == "未知地理位置")?(""):(location));
        return m_location != "";
    }
    
    inline std::string GetSex() const {
        return m_sex;
    }
    
    inline bool SetSex(const std::string& sex) {
        m_sex = ((sex == "未公开")?(""):(sex));
        return m_sex != "";
    }
};

class UserDataDao
{
public:
    static void Save(const UserData& userdata);
    static void Update(const UserData& userdata);
    static void Delete(const UserData& userdata);
};


/////////////////////////////////////////////////////////

class MonitorLog
{
private:
    size_t total_user , man_user , woman_user , man_valid_user , woman_valid_user , total_last_time , parse_time_inf , fetch_time_inf ;
    
public:
    MonitorLog( int total , int man , int woman , int man_valid , int woman_valid )
    :
    total_user(total) ,
    man_user(man) ,
    woman_user(woman) ,
    man_valid_user(man_valid) ,
    woman_valid_user(woman_valid),
    total_last_time(0),
    parse_time_inf(0),
    fetch_time_inf(0)
    {  }
    
    MonitorLog()
    :
    total_user(0) ,
    man_user(0) ,
    woman_user(0) ,
    man_valid_user(0) ,
    woman_valid_user(0),
    total_last_time(0),
    parse_time_inf(0),
    fetch_time_inf(0)
    {  }
    
public:
    inline
    size_t GetTotalUser() const {
        return total_user;
    }
    
    inline
    bool SetTotalUser(size_t total) {
        total_user = total;
        return true ;
    }
    
    inline
    size_t GetManUser() const {
        return man_user;
    }
    
    inline
    bool SetManUser(size_t man) {
        man_user = man;
        return true;
    }
    
    inline
    size_t GetWomanUser() const {
        return woman_user;
    }
    
    inline
    bool SetWomanUser(size_t woman){
        woman_user = woman;
        return true;
    }
    
    inline
    size_t GetManValidUser() const {
        return man_valid_user;
    }
    
    inline
    bool SetManValidUser(size_t man_valid) {
        man_valid_user = man_valid;
        return true;
    }
    
    inline
    size_t GetWomanValidUser() const {
        return woman_valid_user;
    }
    
    inline
    bool SetWomanValidUser(size_t woman_valid) {
        woman_valid_user = woman_valid;
        return true;
    }
    
    inline
    size_t GetTotalTime() const {
        return total_last_time;
    }
    
    inline
    bool SetTotalTime(size_t total) {
        total_last_time = total;
        return true ;
    }
    
    inline
    size_t GetParseTimeInf() const {
        return parse_time_inf;
    }
    
    inline
    bool SetParseTimeInf(size_t parse_time) {
        parse_time_inf = parse_time;
        return true;
    }
    
    inline
    size_t GetFetchTimeInf() const {
        return fetch_time_inf;
    }
    
    inline
    bool SetFetchTimeInf( size_t fetch_time ){
        fetch_time_inf = fetch_time;
        return true;
    }
};

class MonitorLogDao
{
public:
    static void Save  (const MonitorLog& log);
    static void Update(const MonitorLog& log);
    static void Delete(const MonitorLog& log);
};

class MonitorLogOperator
{
public:
    
    ///
    /// @brief  Use Strategy to update log
    /// @param  strategy : void operator()(MonitorLog&) const ;
    ///
    template<typename Strategy>
    inline static
    void Log( Strategy strategy , MonitorLog& log )
    {
        strategy(log);
    }
};

///
///  @brief   UserDataLog
///
class UserDataLogStrategy
{
public:
    UserDataLogStrategy() = delete ;
    UserDataLogStrategy(const UserData& userdata)
    :user_data(userdata)
    { }
    
    void operator()(MonitorLog& log) const ;
    
private:
    const UserData& user_data ;
};

///
/// @brief   Time Log
///
typedef enum {
    TotalTime ,
    ParseTime ,
    FetchTime
} PerformanceValue;

class PerformanceLogStrategy
{
public:
    
    
    PerformanceLogStrategy() = delete ;
    PerformanceLogStrategy(size_t time , PerformanceValue value)
    :last_time(time) , perf_value(value)
    { }
    
    void operator()(MonitorLog& log) const ;
    
private:
    size_t last_time ;
    PerformanceValue perf_value ;
};

#endif /* defined(__WebCrawler__UserData__) */
