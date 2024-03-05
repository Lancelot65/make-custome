#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <vector>
#include <thread>
#include <sstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
namespace fs = std::filesystem;

class custom_make
{
private:
    std::string source;
    int number_core;

    bool debug;
    std::vector<std::string> linker;
    std::vector<fs::path> cpp_file;

public:
    custom_make(std::string source = "src", int number_core = 4) :
            source(source), number_core(number_core)
            {
        if (!this->check_folder_integrity()) 
        {
            throw std::runtime_error("can't find folder");
        }

        for (const auto &entry : fs::directory_iterator(this->source))
        {
            if (fs::is_regular_file(entry.path()))
            {
                this->cpp_file.push_back(entry.path());
            }
        }
        this->compilation_obj();
        this->final_compilation();
    }

    bool check_folder_integrity()
    {
        bool find_source = false;

        if (!fs::exists(this->source))
        {
            return false;
        }

        for (auto file : fs::directory_iterator(this->source))
        {
            if (file.is_regular_file())
            {
                find_source = true;
                break;
            }
        }

        if (!find_source)
        {
            std::cout << "src is empty" << std::endl;
            return false;
        }

        if (!fs::exists("bin"))
        {
            fs::create_directories("bin/release");
            fs::create_directories("bin/debug");
        }

        if (!fs::exists("obj"))
        {
            fs::create_directories("obj/release");
            fs::create_directories("obj/debug");
        }
        return true;
    }

    void processElements(int start, int end)
    {
        std::cout << "thread open\n";
        for (int i = start; i <= end; ++i)
        {
            std::stringstream command_stream;
            command_stream << "g++ -c " << (this->debug ? "-g" : "") << " " << this->cpp_file[i].string()
                           << " -o obj/" << this->cpp_file[i].stem().string() << ".o";
            std::string command = command_stream.str();
            int result = system(command.c_str());
            if (result != 0)
            {
                std::cerr << "Error compiling: " << command << std::endl;
            }
        }
    }

    void compilation_obj() {
        std::vector<std::thread> threads;

        int total_elements = this->cpp_file.size();
        int elements_per_list = total_elements / (this->number_core - 1);
        int extra = total_elements % (this->number_core - 1);

        int index = 0;
        for (int i = 0; i < number_core - 1; ++i)
        {
            if (index < total_elements) {
                threads.push_back(std::thread(&custom_make::processElements, this, index,
                                              index + elements_per_list - 1));
                index += elements_per_list;
            }
        }

        if (total_elements > this->number_core - 1)
        {
            threads.push_back(std::thread(&custom_make::processElements, this,
                                          total_elements - extra, total_elements - 1));
        }

        for (auto &thread : threads)
        {
            thread.join();
        }
    }

    void final_compilation()
	{
	    std::string final_compilation = "g++ obj//*.o -o foo.exe";
	    system(final_compilation.c_str());
	}

};

int main(int argc, char const *argv[])
{
    custom_make();
    return 0;
}
