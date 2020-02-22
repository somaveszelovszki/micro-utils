#pragma once

#include <micro/utils/units.hpp>
#include <micro/utils/CarProps.hpp>
#include <ProgramState.hpp>

namespace micro {

struct TrackSpeeds {
    m_per_sec_t fast;
    m_per_sec_t slow_1;
    m_per_sec_t slow_2_begin;
    m_per_sec_t slow_2;
    m_per_sec_t slow_3;
    m_per_sec_t slow_3_end;
    m_per_sec_t slow_4;
};
DEFINE_TYPEINFO(TrackSpeeds);

} // namespace micro
