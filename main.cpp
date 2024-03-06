#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <vector>
#include <thread>
#include <sstream>
#include <nlohmann/json.hpp>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <ctime>
#include <sys/stat.h>
#include <iomanip>
#include <cstring>


using json = nlohmann::json;
namespace fs = std::filesystem;

void System(std::string commande)
{
    std::cout << commande << std::endl;
    system(commande.c_str());
}

void deleteFilesInDirectory(const std::string& directoryPath) {
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (fs::is_regular_file(entry.path())) {
            fs::remove(entry.path()); 
        }
    }
}


bool file_exist(std::string path)
{
    std::ifstream file(path);
    if (!file.good())
    {
        return false;
    }
    file.close();
    return true;
}

class manage_json
{
private:
    std::string path = ".info.json";
    json json_data;
public:
    manage_json()
    {
        if (!file_exist(this->path))
        {
            this->create_json();
        }
    }

    json return_json()
    {
        std::ifstream f(path, std::ios_base::in);
        json_data = json::parse(f);
        f.close();
        return json_data;
    }

    void write_json(json new_json)
    {
        std::ofstream out(this->path);
        out << std::setw(4) << new_json;
        out.close();
    }

    void create_json()
    {
        std::ofstream out(this->path);
        out << "{\"release\" : {}, \"debug\" : {}}";
        out.close();
    }

    
};

class custom_make
{
private:
    std::string source = "src";
    int number_core = 4;
    std::string lib_source = "lib";

    int argc;
    const char **argv;

    bool debug;
    std::vector<std::string> linker;
    std::vector<fs::path> cpp_file;

    json json_data;

public:
    custom_make(int argc, char const *argv[]) :
        argc(argc), argv(argv)
        {
            if (!this->check_folder_integrity()) 
            {
                throw std::runtime_error("can't find folder");
            }
            manage_json mj;
            this->json_data = mj.return_json(); 

            this->check_parametre();


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
            fs::create_directories("bin\\release");
            fs::create_directories("bin\\debug");
        }

        if (!fs::exists("obj"))
        {
            fs::create_directories("obj\\release");
            fs::create_directories("obj\\debug");
        }
        return true;
    }

    void processElements(int start, int end)
    {
        std::cout << "thread open\n";

        for (int i = start; i <= end; ++i)
        {
            struct stat fileInfo;

            if(stat(this->cpp_file[i].string().c_str(), &fileInfo) != 0)
            {
                return;
            }
            int64_t actuel_time_file = fileInfo.st_mtime;

            std::stringstream path_obj;
            path_obj << "obj\\" << (this->debug ? "debug" : "release") << "\\" << this->cpp_file[i].stem().string();

            if (file_exist(path_obj.str()) && this->json_data[(this->debug ? "debug" : "release")].find(this->cpp_file[i].stem().string()) != this->json_data[(this->debug ? "debug" : "release")].end())
            {
                
                int64_t last_time = this->json_data[(this->debug ? "debug" : "release")][this->cpp_file[i].stem().string()];

                if (actuel_time_file == last_time)
                {
                    continue;
                }
                else
                {
                    std::stringstream command_stream;
                    command_stream << "g++ -c" << (this->debug ? " -g " : " ") << this->cpp_file[i].string()
                                   << " -o " << (this->debug ? "obj\\debug\\" : "obj\\release\\") << this->cpp_file[i].stem().string() << ".o";
                    std::string command = command_stream.str();
                    System(command.c_str());
                    this->json_data[(this->debug ? "debug" : "release")][this->cpp_file[i].stem().string()] = actuel_time_file;
                }
            }
            else
            {
                std::stringstream command_stream;
                command_stream << "g++ -c" << (this->debug ? " -g " : " ") << this->cpp_file[i].string()
                               << " -o " << (this->debug ? "obj\\debug\\" : "obj\\release\\") << this->cpp_file[i].stem().string() << ".o";
                std::string command = command_stream.str();
                System(command.c_str());
                this->json_data[(this->debug ? "debug" : "release")][this->cpp_file[i].stem().string()] = actuel_time_file;
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
            if (index < total_elements)
            {
                threads.push_back(std::thread(&custom_make::processElements, this, index, index + elements_per_list - 1));
                index += elements_per_list;
            }
        }

        threads.push_back(std::thread(&custom_make::processElements, this, total_elements - extra, total_elements - 1));

        for (auto &thread : threads)
        {
            thread.join();
        }

        manage_json mj;
        mj.write_json(this->json_data);
    }

    void final_compilation()
	{
        std::string v_lib;


        if (fs::exists("lib"))
        {
            v_lib = "lib\\*.a";
        }

        std::string liste_linkers;
        for (auto i : this->linker)
        {
            liste_linkers += i;
            liste_linkers += " ";
        }

        std::stringstream ss;
        ss << "g++" << (this->debug ? " -g " : " ") << (this->debug ? "obj\\debug\\*.o" : "obj\\release\\*.o") << " -o " << (this->debug ? "bin\\debug\\a.exe" : "bin\\release\\a.exe") << " " << liste_linkers << " " << v_lib;
	    System(ss.str().c_str());
	}

    void check_parametre()
    {
        for (int i = 0; i < argc; ++i)
        {
            if (std::strcmp(argv[i], "-d") == 0) 
            {
                this->debug = true;
            }
            else if (std::strncmp(argv[i], "-l", 2) == 0)
            {
                this->linker.push_back(argv[i]);
            }
            else if (std::strncmp(argv[i], "-t", 2) == 0)
            {
                const char* number_str = argv[i] + 2;
                this->number_core = std::atoi(number_str);
            }
            else if (std::strncmp(argv[i], "-s", 2) == 0)
            {
                const char* new_path = argv[i] + 2;
                this->source = std::string(new_path);   
            }
            else if (std::strncmp(argv[i], "-c", 2) == 0)
            {
                const char* mode = argv[i] + 2;
                clear_obj(std::string(mode));
            }
        }
    }

    void clear_obj(std::string who)
    {
        if (who == "debug")
        {
            deleteFilesInDirectory("obj/debug");
        }
        else if (who == "release")
        {
            deleteFilesInDirectory("obj/release");   
        }
    }

};

int main(int argc, const char *argv[])
{
    custom_make(argc, argv);
    return 0;
}