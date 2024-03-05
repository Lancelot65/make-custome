


input gestion_input(int argc, char *argv[])
{
    input data;

    for (int i = 1; i < argc; ++i) 
    {
        std::string arg = argv[i];
        if (arg.substr(0, 2) == "-l")
        {
            data.linker.push_back(arg);
        }
    }

    data.debug = std::find(options.begin(), options.end(), "-d") != options.end();
    return data;
}

int main(int argc, char *argv[]) {
    if (!check_folder_entingrity()) {
        return 0;
    }

    input data = gestion_input(argc, argv);

    compilation_obj(data.debug);
    final_compilation();

    

    return 0;
}
 