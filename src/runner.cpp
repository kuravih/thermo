#include "thermo.h"

#include <thread>
#include <csignal>

volatile std::atomic<bool> busy{true};

void sigint_handler(int signal)
{
    if (signal == SIGINT)
    {
        busy = false;
        kato::log::cout << KATO_RED << "runner.cpp::sigint_handler() - Terminating readout ..." << KATO_RESET << std::endl;
    }
}

int main(int argc, char const *argv[])
{
    kato::log::cout << KATO_GREEN << "runner.cpp::main() - Starting listen..." << KATO_RESET << std::endl;

    std::signal(SIGINT, sigint_handler);

    kato::SerialLink link("/dev/ttyGS0", B115200, onError);

    DS2484 bus_11((DS2484_I2C_BUS_PREFIX + std::to_string(11)).c_str());
    DS2484 bus_12((DS2484_I2C_BUS_PREFIX + std::to_string(12)).c_str());
    DS2484 bus_13((DS2484_I2C_BUS_PREFIX + std::to_string(13)).c_str());
    DS2484 bus_14((DS2484_I2C_BUS_PREFIX + std::to_string(14)).c_str());
    DS2484 bus_15((DS2484_I2C_BUS_PREFIX + std::to_string(15)).c_str());
    DS2484 bus_16((DS2484_I2C_BUS_PREFIX + std::to_string(16)).c_str());
    DS2484 bus_17((DS2484_I2C_BUS_PREFIX + std::to_string(17)).c_str());
    DS2484 bus_18((DS2484_I2C_BUS_PREFIX + std::to_string(18)).c_str());
    DS2484 bus_19((DS2484_I2C_BUS_PREFIX + std::to_string(19)).c_str());
    DS2484 bus_21((DS2484_I2C_BUS_PREFIX + std::to_string(21)).c_str());
    DS2484 bus_22((DS2484_I2C_BUS_PREFIX + std::to_string(22)).c_str());
    DS2484 bus_23((DS2484_I2C_BUS_PREFIX + std::to_string(23)).c_str());

    std::vector<DS2484> buses = {bus_11, bus_12, bus_13, bus_14, bus_15, bus_16, bus_17, bus_18, bus_19, bus_21, bus_22, bus_23};

    std::vector<std::thread> threads;
    for (DS2484 &bus : buses)
        threads.emplace_back(std::thread(busSampleStream, std::ref(link), std::ref(bus)));
    threads.emplace_back(std::thread(coreSampleStream, std::ref(link)));

    // ---- establish connection --------------------------------------------------------------------------------------
    while (!link.is_connected && busy.load())
    {
        while (!link.is_data_available() && busy.load())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        link.read_and_respond();
        onOrderStop();
    }
    // ---- establish connection --------------------------------------------------------------------------------------

    if (link.is_connected)
        kato::log::cout << KATO_GREEN << "runner.cpp::main() - Connected..." << KATO_RESET << std::endl;

    // ---- serial communication --------------------------------------------------------------------------------------
    while (link.is_connected && busy.load())
    {
        while (!link.is_data_available() && busy.load())
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        readRespond(link, buses);
    }
    // ---- serial communication --------------------------------------------------------------------------------------

    // onOrderStart();

    kato::log::cout << KATO_GREEN << "runner.cpp::main() - Stopping listen..." << KATO_RESET << std::endl;

    for (std::thread &thread : threads)
        thread.join();

    kato::log::cout << "runner.cpp::" << "main() " << "Stop logging." << std::endl;

    return 0;
}
