#ifndef __THERMO_H__
#define __THERMO_H__

#pragma once

#include "kato/log.h"
#include "kato/serial_link.h"
#include "ds2484/ds2484.h"

#include <atomic>

inline void onError()
{
    kato::log::cout << KATO_RED << "Error" << KATO_RESET << std::endl;
}
inline void write_sample(kato::SerialLink &_link, const sample_t _sample)
{
    _link.write_object(&_sample, sizeof(sample_t));
}

void readRespond(kato::SerialLink &, std::vector<DS2484> &);
void busSampleStream(kato::SerialLink &, DS2484 &);
void read_core_temperature(float &);
void coreSampleStream(kato::SerialLink &);

void onOrderStart();
void onOrderStop();
void onOrderScan(kato::SerialLink &, DS2484 &);
void onOrderRead(kato::SerialLink &, DS2484 &, const uint64_t);
void onOrderRead(kato::SerialLink &, const uint64_t);
void onError();

#endif //__THERMO_H__