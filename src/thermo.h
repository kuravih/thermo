#ifndef __THERMO_H__
#define __THERMO_H__

#pragma once

#include "kato/log.hpp"
#include "link/serial_link.hpp"
#include "ds2484/ds2484.hpp"

#include <atomic>

inline void onError()
{
    kato::log::cout << KATO_RED << "Error" << KATO_RESET << std::endl;
}
inline void write_sample(SerialLink &_link, const sample_t _sample)
{
    _link.write_object(&_sample, sizeof(sample_t));
}

void readRespond(SerialLink &, std::vector<DS2484> &);
void busSampleStream(SerialLink &, DS2484 &);
void read_core_temperature(float &);
void coreSampleStream(SerialLink &);

void onOrderStart();
void onOrderStop();
void onOrderScan(SerialLink &, DS2484 &);
void onOrderRead(SerialLink &, DS2484 &, const uint64_t);
void onOrderRead(SerialLink &, const uint64_t);
void onError();

#endif //__THERMO_H__