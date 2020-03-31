/*
    Huffman coding.
    Ivan Rybin 2019.
*/

#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <vector>

#include "huffman.hpp"

enum Errors {
    OK,
    INVALID_ARGS_COUNT,
    INVALID_FLAG,
    WITHOUT_FLAG,
    NO_INPUT_FILE,
    NO_OUTPUT_FILE
};

enum Flag {
    NOTHING,
    ENCODE,
    DECODE
};

int process(int argc, char **argv) {
    std::string                 input_file  {};
    std::string                 output_file {};

    memory_vector               nodes       {};
    std::vector<const char*>    strings     {};
    std::vector<std::string>    commands    {};

    bool is_console      = false;

    Flag flag            = NOTHING;
    int  fst_arg_pos     = 1;

    commands.reserve(argc);

    for (int i = 0; i < argc; ++i) {
        commands.emplace_back(argv[i]);
    }

    // args count test
    switch (argc) {
    case 4:

        fst_arg_pos = 1;
        break;

    case 5:
        if (commands[1] == "-v") {

            is_console = true;

        } else {

            std::cout << "INVALID FIRST FLAG: must be -v for 5 args, -c or -d for 4 args" << std::endl;
            return INVALID_FLAG;
        }

        fst_arg_pos = 2;
        break;
    default:
        std::cout << "INVALID ARGS COUNT: must be 4 or 5" << std::endl;
        return INVALID_ARGS_COUNT;
    }

    // flag test
    if (commands[fst_arg_pos] == "") {

        std::cout << "WITHOUT FLAG: must be (-v) -c or -d)" << std::endl;
        return WITHOUT_FLAG;

    } else if (commands[fst_arg_pos] != "-c" && commands[fst_arg_pos] != "-d") {

        std::cout << "INVALID FLAG: must be (-v) -c or -d" << std::endl;
        return INVALID_FLAG;

    } else if (commands[fst_arg_pos] == "-c") {

        flag = ENCODE;
        
    } else if (commands[fst_arg_pos] == "-d") {

        flag = DECODE;
    }

    // files
    input_file    = commands[fst_arg_pos + 1];
    output_file   = commands[fst_arg_pos + 2];

    // files test
    if (input_file == "") {
        std::cout << "NO INPUT FILE" << std::endl;
        return NO_INPUT_FILE;
    }

    if (output_file == "") {
        std::cout << "NO OUTPUT FILE" << std::endl;
        return NO_OUTPUT_FILE;
    }

    // char
    size_t     input_size   = get_file_size(input_file);
    const char* input_str   = get_char_content(input_file, input_size);

    try {

        switch (input_size) {
            case 0: {
                std::cout << 0 << std::endl << 0 << std::endl << 0 << std::endl;
                std::ofstream output(output_file, std::ios_base::trunc);

                break;
            }
            default: {

                switch (flag) {
                    case ENCODE:

                        encoding(input_str, input_size, output_file, is_console, nodes);
                        break;

                    case DECODE: {

                        size_t alphabet_size = 0;
                        size_t tree_size = 0;
                        std::vector<std::string> v_alphabet_tree_str = get_alphabet_tree_str(input_str,
                                input_size,
                                                                                             alphabet_size, tree_size);

                        decoding(v_alphabet_tree_str, input_size, alphabet_size, tree_size, output_file, is_console,
                                 nodes);
                        break;
                    }
                    default:
                        break;
                }
                break;
            }
        }
    } catch(...) {

        delete input_str;
        free_memory(nodes);

    }

    return OK;
}

int main(int argc, char **argv) {
    process(argc, argv);
}
