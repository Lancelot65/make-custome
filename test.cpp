#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <ctime>

int main()
{
    std::filesystem::path p = "src/main.cpp";

    std::filesystem::file_time_type ftime = std::filesystem::last_write_time(p);
    std::time_t cftime = decltype(ftime)::clock::to_time_t(ftime);
    std::string str_time = std::ctime(&cftime);
    std::cout << "File write time is " << str_time << std::endl;
}
