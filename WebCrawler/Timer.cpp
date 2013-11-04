//
//  Timer.cpp
//  WebCrawler
//
//  Created by dongshihao on 13-11-4.
//  Copyright (c) 2013年 dongshihao. All rights reserved.
//

#include "Timer.h"

timer::timer()
{
    resetStartTime();
}

timer::~timer()
{
    // Nothing
}

void timer::Reset()
{
    resetStartTime();
}

double timer::ElapsedSeconds() const
{
    return std::chrono::duration_cast<std::chrono::seconds>( elapsedDuration() ).count();
}

double timer::ElapsedMilliseconds() const
{
    return std::chrono::duration_cast<std::chrono::milliseconds>( elapsedDuration() ).count();
}

void timer::resetStartTime()
{
    m_Start = std::chrono::system_clock::now();
}

std::chrono::system_clock::duration timer::elapsedDuration() const
{
    return std::chrono::system_clock::now() - m_Start ;
}
