#include "thermo.h"

#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>
#include <fstream>

#define CORE_ROM 0xC012E001C012E001
static sample_t DEADBEEF(0xDEADBEEFDEADBEEF, 0.0);

std::mutex mtx;
std::atomic<bool> live;
extern std::atomic<bool> busy;

void onOrderScan(SerialLink &_link, DS2484 &_bus)
{
    // kato::log::cout << KATO_GREEN << "thermo.cpp::onOrderScan() bus " << _bus.dev << KATO_RESET << std::endl;
    std::lock_guard<std::mutex> lock(mtx);
    _link.write_uint8(_bus.samples.size());
    for (sample_t &sample : _bus.samples)
        _link.write_uint64(sample.rom);
}

void onOrderRead(SerialLink &_link, DS2484 &_bus, const uint64_t _rom)
{
    // kato::log::cout << KATO_GREEN << "thermo.cpp::onOrderRead() bus " << _bus.dev << " rom " << std::hex << std::setw(16) << std::setfill('0') << _rom << KATO_RESET << std::endl;
    for (sample_t &sample : _bus.samples)
    {
        if (sample.rom == _rom)
        {
            _bus.led(sample.rom, true);
            _bus.read_temperature(sample.rom, sample.temperature, true);
            sample.set_timestamp();
            _bus.led(sample.rom, false);
            std::lock_guard<std::mutex> lock(mtx);
            write_sample(_link, sample);
            return;
        }
    }
    std::lock_guard<std::mutex> lock(mtx);
    write_sample(_link, DEADBEEF);
}

void onOrderRead(SerialLink &_link, const uint64_t _rom)
{
    // kato::log::cout << KATO_GREEN << "thermo.cpp::onOrderRead()" << KATO_RESET << std::endl;
    if (_rom == CORE_ROM)
    {
        sample_t sample{CORE_ROM, 0};
        read_core_temperature(sample.temperature);
        std::lock_guard<std::mutex> lock(mtx);
        write_sample(_link, sample);
        return;
    }
    std::lock_guard<std::mutex> lock(mtx);
    write_sample(_link, DEADBEEF);
}

void busSampleStream(SerialLink &_link, DS2484 &_bus)
{
    kato::log::cout << KATO_GREEN << "thermo.cpp::busSampleStream() for " << _bus.dev << " Starting..." << KATO_RESET << std::endl;
    while (busy.load())
    {
        if (_link.is_connected && live.load())
        {
            if (_bus.samples.size() > 0)
            {
                _bus.start_conversion(true);
                for (sample_t &sample : _bus.samples)
                {
                    // _bus.led(sample.rom, true);
                    _bus.read_temperature(sample.rom, sample.temperature, false, false);
                    sample.set_timestamp();
                    // _bus.led(sample.rom, false);
                    std::lock_guard<std::mutex> lock(mtx);
                    write_sample(_link, sample);
                    kato::log::cout << KATO_BLUE << "thermo.cpp::busSampleStream()  " << "sample " << sample << KATO_RESET << std::endl;
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    kato::log::cout << KATO_GREEN << "thermo.cpp::busSampleStream() for " << _bus.dev << " Stopping..." << KATO_RESET << std::endl;
}

void read_core_temperature(float &_temperature_c)
{
    const std::string thermal_file = "/sys/class/thermal/thermal_zone0/temp";
    std::ifstream temp_file(thermal_file);
    if (!temp_file.is_open())
        kato::log::cout << KATO_RED << "thermo.cpp::coreSampleStream() - Core temperature could not be read." << KATO_RESET << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    int temp_milli_c = 0;
    temp_file >> temp_milli_c;
    temp_file.close();
    _temperature_c = 0.0625 * int(temp_milli_c / 62.5);
}

void coreSampleStream(SerialLink &_link)
{
    kato::log::cout << KATO_GREEN << "thermo.cpp::coreSampleStream() Starting..." << KATO_RESET << std::endl;
    while (busy.load())
    {
        if (_link.is_connected && live.load())
        {
            sample_t sample{CORE_ROM, 0};
            read_core_temperature(sample.temperature);
            std::lock_guard<std::mutex> lock(mtx);
            write_sample(_link, sample);
            kato::log::cout << KATO_BLUE << "thermo.cpp::coreSampleStream() " << "sample " << sample << KATO_RESET << std::endl;
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    kato::log::cout << KATO_GREEN << "thermo.cpp::coreSampleStream() Stopping..." << KATO_RESET << std::endl;
}

void onOrderStart()
{
    // kato::log::cout << KATO_GREEN << "thermo.cpp::onOrderStart()" << KATO_RESET << std::endl;
    live.store(true);
}

void onOrderStop()
{
    // kato::log::cout << KATO_GREEN << "thermo.cpp::onOrderStop()" << KATO_RESET << std::endl;
    live.store(false);
}

void readRespond(SerialLink &_link, std::vector<DS2484> &_buses)
{
    if (_link.is_data_available())
    {
        onOrderStop();
        // tcflush(_link.fd, TCIFLUSH);
        order_t order = _link.read_order(); // The first byte received is the order
        if (order == order_t::HELLO)
        {
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [HELLO]" << KATO_RESET << std::endl;
            if (!_link.is_connected) // If the cards haven't say hello, check the connection
            {
                _link.is_connected = true;
                _link.write_order(order_t::HELLO);
                // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - TX [HELLO]" << KATO_RESET << std::endl;
            }
            else
            {
                _link.write_order(order_t::ALREADY_CONNECTED); // If we are already connected do not send "hello" to avoid infinite loop
                // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - TX [ALREADY_CONNECTED]" << KATO_RESET << std::endl;
            }
        }
        else if (order == order_t::ALREADY_CONNECTED)
        {
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [ALREADY_CONNECTED]" << KATO_RESET << std::endl;
            _link.is_connected = true;
        }
        else if (order == order_t::SCAN)
        {
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [SCAN]" << KATO_RESET << std::endl;
            uint8_t channel = _link.read_uint8(); // channel to scan
            for (DS2484 &bus : _buses)
                if (bus.dev == DS2484_I2C_BUS_PREFIX + std::to_string((int)channel))
                    onOrderScan(_link, bus);
        }
        else if (order == order_t::READ)
        {
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [READ]" << KATO_RESET << std::endl;
            uint8_t channel = _link.read_uint8(); // channel to read
            uint64_t rom = _link.read_uint64();   // sensor to read
            if (rom == CORE_ROM)
                onOrderRead(_link, rom);
            else
                for (DS2484 &bus : _buses)
                    if (bus.dev == DS2484_I2C_BUS_PREFIX + std::to_string((int)channel))
                        onOrderRead(_link, bus, rom);
        }
        else if (order == order_t::START)
        {
            onOrderStart();
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [START]" << KATO_RESET << std::endl;
        }
        else if (order == order_t::STOP)
        {
            onOrderStop();
            // kato::log::cout << KATO_GREEN << "thermo.cpp::readRespond() - RX [STOP]" << KATO_RESET << std::endl;
        }
    }
}