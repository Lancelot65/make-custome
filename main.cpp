#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <vector>
#include <thread>

namespace fs = std::filesystem;

const std::string PATH_SOURCE = "src";
int const number_core = 4;

bool check_folder_entingrity() {
    bool find_source = false;

    if (!fs::exists("src")) {
        std::cout << "could not find folder src" << std::endl;
        return false;
    }

    for (auto file : fs::directory_iterator(PATH_SOURCE)) {
        if (file.is_regular_file()) {
            find_source = true;
            break; // Exit loop early if any regular file is found
        }
    }

    if (!find_source) {
        std::cout << "src is empty" << std::endl;
        return false;
    }

    if (!fs::is_directory("bin")) {
        system("mkdir bin\\release");
        system("mkdir bin\\debug");
    }
    if (!fs::is_directory("obj")) {
        system("mkdir obj\\release");
        system("mkdir obj\\debug");
    }
    return true;
}

void processElements(const std::vector<fs::path>& files, int start, int end)
{
	std::cout << "thread open\n";
    for (int i = start; i < end; ++i)
    {
        std::cout << start << " " <<end;
        std::string commande = "g++ -c " + files[i].string() + " -o obj\\" + files[i].stem().string() + ".o";
        system(commande.c_str());
    }
}

int main() {
    if (!check_folder_entingrity()) {
        return 0;
    }

    std::vector<fs::path> fichiers;

    for (const auto& entry : fs::directory_iterator("src")) {
        if (fs::is_regular_file(entry.path())) {
            fichiers.push_back(entry.path());
        }
    }

    std::vector<std::thread> threads;

    int total_elements = fichiers.size();
    int elements_par_liste = total_elements / (number_core - 1);
    int en_plus = total_elements % (number_core - 1);

    int index = 0;
    for (int i = 0; i < number_core - 1; ++i)
    {
    	if (index < total_elements)
    	{
        	threads.push_back(std::thread(processElements, fichiers, index, index + elements_par_liste));
        	index += elements_par_liste + 1;
        }
    }

    if (total_elements > number_core - 1)
    {
		threads.push_back(std::thread(processElements, fichiers, total_elements - en_plus, total_elements));
	}

    for (auto& thread : threads)
    {
        thread.join();
    }

    return 0;
}
