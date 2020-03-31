/*
    Huffman coding.
    Ivan Rybin 2019.
*/

#include <iostream>
#include <fstream>
#include <queue>
#include <bitset>
#include <algorithm>
#include <experimental/filesystem>

#include "huffman.hpp"

namespace fs = std::experimental::filesystem;

bool operator < (const CharData& lhs, const CharData& rhs) {
    return lhs.frequency > rhs.frequency;
}

bool chars_sort(const std::pair<const unsigned char, std::string>& lhs, const std::pair<const unsigned char, std::string>& rhs) {
    return lhs.first < rhs.first;
}

bool codes_sort(const std::pair<unsigned char, std::string>& lhs, const std::pair<unsigned char, std::string>& rhs) {
    return lhs.second < rhs.second;
}

size_t get_file_size(const std::string& file_name) {
    return fs::file_size(fs::path(file_name));
}

const char* get_char_content(const std::string& file_name,
                                        size_t  size)
{
     std::ifstream input_file(file_name, std::ios_base::binary);
     size = get_file_size(file_name);

     char * buffer = new char [size + 1]{};

     input_file.seekg(0, std::ios::beg);
     input_file.read(buffer, size);

     return buffer;
}

std::vector<std::string> get_alphabet_tree_str(const char*      content,
                                                   size_t       content_size,
                                                   size_t&      alphabet_size,
                                                   size_t&      tree_size) {

    if (content_size == 0) {
        return {};
    }

    size_t alph_size  = static_cast<size_t> (static_cast<unsigned char> (content[0])) + 1;
    size_t bits_tree  = static_cast<size_t> (content[1]);
    size_t bits_data  = static_cast<size_t> (content[2]);
    size_t tree_end = 0;

    for (size_t i = 1; i + 3 <= content_size; ++i) {
        if (content[i] == '!' && content[i + 1] == 'T' && content[i + 2] == '^') {
            tree_end = i;
            break;
        }
    }

    alphabet_size  = alph_size;
    
    std::string alphabet(content + 3, content + alph_size + 3);

    std::string tree{};
    for (auto i = content + alph_size + 3; i < content + tree_end; ++i) {
        std::bitset<8> byte(*i);
        tree += byte.to_string();
        ++tree_size;
    }

    tree = tree.empty() ? tree : std::string(tree.begin(), tree.end() - bits_tree);

    std::string str{};
    for (auto i = content + tree_end + 3; i < content + content_size; ++i) {
        std::bitset<8> byte(*i);
        str += byte.to_string();
    }

    str = str.empty() ? str : std::string(str.begin(), str.end() - bits_data);

    return {alphabet, tree, str};  // alphabet, huffman tree, encoded string
}

std::string get_out_str(const std::string&      alphabet,
                        const std::string&      tree,
                        const std::string&      str,
                                   size_t       bits_tree,
                                   size_t       bits_data) {

    std::string out{};
    out += static_cast<unsigned char> (alphabet.length() - 1);
    out += static_cast<unsigned char> (bits_tree);
    out += static_cast<unsigned char> (bits_data);    

    return out + alphabet + tree + "!T^" + str; // !T^ -- separator between tree and encoded string
}

void print_statistics(size_t                input_size,
                      size_t                output_size,
                      size_t                help_size,
                      const char_code_map&  chars_codes,
                      bool                  is_console) {

    std::cout << input_size  << std::endl << 
                 output_size << std::endl << 
                 help_size   << std::endl;

    if (is_console) {

        std::vector<std::pair<unsigned char, std::string>> codes(chars_codes.begin(), chars_codes.end());

        std::sort(codes.begin(), codes.end(), codes_sort);

        for (const auto& item: codes) {
            std::cout << item.second << " " << static_cast<uint16_t> (item.first) << std::endl;
        }
    }
}

void write_file(const std::string&      file_name,
                const std::string&      output_str) {

    std::ofstream  output_file(file_name,  std::ios_base::binary);
    output_file.clear();

    output_file << output_str;
}

void free_memory(std::vector<const Node*>& nodes) {

    for (const auto& item: nodes) {
        delete item;
    }

    nodes = {};
}

std::vector<CharData> chars_frequencies(const char*     content,
                                            size_t      size) {

    size_t i = 0;

    char_freq_map chars_freq_map{};

    while (i != size) {
        ++chars_freq_map[content[i]];
        ++i;
    }

    std::vector<CharData> chars_freq_vec{};
    for (const auto& item: chars_freq_map) {
        chars_freq_vec.push_back({std::string(1, item.first), item.second});
    }

    return chars_freq_vec;
}

std::string huffman_decoding(const std::string&     encoded_str,
                             const Node*            root,
                             char_code_map&         chars_codes) {
    // decoding via tree

    std::stringstream ss{};

    const Node* curr = root;

    size_t i    = 0;
    size_t size = encoded_str.length();

    while (i != size) {
        std::string path = {};

        while (!curr->is_leaf) {
            switch (encoded_str[i]) {
            case '0': {
                curr = curr->left;
                path += '0';
                ++i;
                break;
            }
            case '1': {
                curr = curr->right;
                path += '1';
                ++i;
                break;
            }
            default:
                break;
            }
        }
        if (curr != nullptr && curr->is_leaf) {
            ss << curr->letter;
            chars_codes[curr->letter] = path;

            if (root->is_leaf) {
                chars_codes[curr->letter] = '0';
            }
        }
        if (root->is_leaf) {
            ++i;
        }
        curr = root;
    }

    return ss.str();
}

char_code_map huffman_encoding(const std::vector<CharData>& input_chars) { 

    char_code_map encoded_chars{};

    if (input_chars.size() == 1) {
        encoded_chars[*input_chars[0].chars.c_str()] = "0";
        
        return encoded_chars;
    }

    std::priority_queue<CharData, std::vector<CharData>> chars_codes_tree(input_chars.begin(), input_chars.end());

    while (chars_codes_tree.size() >= 2) {

        CharData first_child = chars_codes_tree.top();
        chars_codes_tree.pop();

        CharData second_child = chars_codes_tree.top();
        chars_codes_tree.pop();

        size_t fst_child_elem_cnt = first_child.chars.size();
        for (size_t i = 0; i < fst_child_elem_cnt; ++i) {
            encoded_chars[first_child.chars[i]] = "0" + encoded_chars[first_child.chars[i]];
        }
        
        size_t snd_child_elem_cnt = second_child.chars.size();
        for (size_t i = 0; i < snd_child_elem_cnt; ++i) {
            encoded_chars[second_child.chars[i]] = "1" + encoded_chars[second_child.chars[i]];
        }

        CharData new_tree_node{first_child.chars + second_child.chars, first_child.frequency + second_child.frequency};
        chars_codes_tree.push(new_tree_node);
    }
    
    return encoded_chars; 
}

Node* build_tree_with_map(const char_code_map& encoded_chars, memory_vector& nodes) {

    Node* root = new Node; // new
    nodes.push_back(root); // caught

    if (encoded_chars.size() == 1) {
        root->letter    = encoded_chars.begin()->first;
        root->is_leaf   = true;

        return root;
    }

    std::vector<std::pair<unsigned char, std::string>> codes(encoded_chars.begin(), encoded_chars.end());
    std::sort(codes.begin(), codes.end(), codes_sort);

    Node* curr = root;

    for (const auto& item: codes) {
        size_t cur_depth = item.second.length();

        for (size_t i = 0; i < cur_depth; ++i) {

            if (item.second[i] == '0') {
                if (curr->left == nullptr) {
                    Node* node = new Node;  // new
                    nodes.push_back(node);  // caught

                    curr->left = node;
                    node->prev = curr;
                    curr = node;
                } else {
                    curr = curr->left;
                }
            } 

            if (item.second[i] == '1') {
                if (curr->right == nullptr) {
                    Node* node = new Node; // new
                    nodes.push_back(node); // caught

                    curr->right = node;
                    node->prev = curr;
                    curr = node;
                } else {
                    curr = curr->right;
                }
            }

            if (i == cur_depth - 1) {
                curr->is_leaf = true;
                curr->letter = item.first;
            }
        }
        curr = root;
    }

    return root;
}

void dfs(Node*                  root,
         std::stringstream&     tree,
         std::string&           alphabet) {

    root->visited = true;

    if (root->left != nullptr && !root->left->visited) {
        tree << "0";
        dfs(root->left, tree, alphabet);
    }

    if (root->right != nullptr && !root->right->visited) {
        dfs(root->right, tree, alphabet);
    }

    if (root->is_leaf) {
        tree << "1";
        alphabet += root->letter;
    }
}

std::string encode_tree(Node*           root,
                        size_t&         bits_tree,
                        std::string&    alphabet) {

    std::stringstream  ss{};
    dfs(root, ss, alphabet);

    size_t len = ss.str().length();

    while (len % 8 != 0) {
        ss << "0";
        ++len;
        ++bits_tree;
    }

    std::string tmp = ss.str();
    ss.clear();

    std::string bytes_encoded_tree{};

    for (size_t i = 0; i < len; i += 8) {
        std::bitset<8> byte(std::string(tmp.begin() + i, tmp.begin() + i + 8));
        bytes_encoded_tree += static_cast<unsigned char> (byte.to_ulong());
    }

    return bytes_encoded_tree;
}

Node* build_alphabet_tree(const std::string&    alphabet,
                          const std::string&    encoded_tree,
                          memory_vector&        nodes) {

    Node* root      = new Node; // new
    Node* curr      = root;
    nodes.push_back(root);      // caught

    if (encoded_tree.length() == 1) {
        root->is_leaf = true;
        root->letter  = alphabet[0];
        
        return root;
    }

    size_t i = 0;
    size_t j = 0;
    int32_t size = encoded_tree.length();

    while(size > 0) {
        if (encoded_tree[i] == '0') {

            Node* node = new Node; // new
            nodes.push_back(node); // caught

            if (curr->left == nullptr) {
                curr->left  = node;
            } else if (curr->right == nullptr) {
                curr->right = node;
            }

            node->prev = curr;
            curr = node;
            ++i;
            --size;
        }
        if (encoded_tree[i] == '1') {

            curr->letter    = alphabet[j];
            ++j;
            curr->is_leaf   = true;
            curr = curr->prev;

            if (curr->left != nullptr && curr->left->is_leaf && curr->right == nullptr) {
                Node* node = new Node; // new
                nodes.push_back(node); // caught

                curr->right = node;
                node->prev  = curr;
                curr        = node;
            }

            i    += 1;
            size -= 1;

            if (curr->left != nullptr && curr->right != nullptr && size > 0) {

                while (curr->right != nullptr) {
                    curr = curr->prev;
                }

                Node* node = new Node; // new
                nodes.push_back(node); // caught

                curr->right = node;
                node->prev  = curr;
                curr        = node;
            }
        }
    }

    return root;
}

std::string encode_string(const char*       content,
                         size_t             size,
                         char_code_map&     char_freq_map,
                         size_t&            bits_data) {

    std::string bit_encoded_str{};

    size_t len = size;

    for (size_t i = 0; i < len; ++i) {
        bit_encoded_str += char_freq_map[content[i]];
    }

    len = bit_encoded_str.length();
    while (len % 8 != 0) {
        bit_encoded_str += "0";
        ++len;
        ++bits_data;
    }

    std::string bytes_encoded_str{};

    for (size_t i = 0; i < len; i += 8) {
        std::bitset<8> byte(std::string(bit_encoded_str.begin() + i, bit_encoded_str.begin() + i + 8));
        bytes_encoded_str += static_cast<unsigned char> (byte.to_ulong());
    }

    return bytes_encoded_str;
}

void encoding(const char*           input_str,
              size_t                input_size,
              const std::string&    output_file,
              bool                  is_console,
              memory_vector&        nodes) {

    size_t      bits_data = 0;
    size_t      bits_tree = 0;
    std::string alphabet  = {};

    std::vector<CharData>   chars_freqs   = chars_frequencies   (input_str, input_size);
    char_code_map           chars_codes   = huffman_encoding    (chars_freqs);
    std::string             encoded_str   = encode_string       (input_str, input_size, chars_codes, bits_data);
    Node*                   root          = build_tree_with_map (chars_codes, nodes);
    std::string             encoded_tree  = encode_tree         (root, bits_tree, alphabet);

    print_statistics(input_size, encoded_str.length(), alphabet.length() + encoded_tree.length() + 6, chars_codes, is_console);
    write_file(output_file, get_out_str(alphabet, encoded_tree, encoded_str, bits_tree, bits_data));
}

void decoding(std::vector<std::string>  v_alphabet_tree_str,
              size_t                    input_size,
              size_t                    alpha_size,
              size_t                    tree_size,
              std::string               output_file,
              bool                      is_console,
              memory_vector&            nodes) {

    Node*           root        = build_alphabet_tree(v_alphabet_tree_str[0], v_alphabet_tree_str[1], nodes);
    char_code_map   chars_codes = {};
    std::string     decoded_str = huffman_decoding(v_alphabet_tree_str[2], root, chars_codes);

    print_statistics(input_size - alpha_size - tree_size - 6, decoded_str.length(), alpha_size + tree_size + 6, chars_codes, is_console);
    write_file(output_file, decoded_str);
}
