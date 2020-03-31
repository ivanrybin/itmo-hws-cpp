/*
    Huffman coding.
    Ivan Rybin 2019.
*/

#pragma once

#include <sstream>
#include <cstring>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

struct Node {
    Node* left      = nullptr;
    Node* right     = nullptr;
    Node* prev      = nullptr;

    bool is_leaf    = false;
    bool visited    = false;

    unsigned char letter{};
};

struct CharData {
    std::string     chars;
    size_t          frequency;
};

using char_code_map = std::unordered_map<unsigned char, std::string>;
using char_freq_map = std::map          <unsigned char, uint32_t>;
using memory_vector = std::vector       <const Node*>;

void encoding(const char*            input_str,
                  size_t             input_size,
                  const std::string& output_file,
                  bool               is_console,
                  memory_vector&     nodes
                  );

void decoding(std::vector<std::string>          v_alphabet_tree_str,
                              size_t            input_size,
                              size_t            alpha_size,
                              size_t            tree_size,
                              std::string       output_file,
                              bool              is_console,
                              memory_vector&    nodes
                              );

Node* build_alphabet_tree(const std::string&    alphabet,
                          const std::string&    encoded_tree,
                          memory_vector&        nodes
                          );

Node* build_tree_with_map(const char_code_map&  encoded_chars,
                          memory_vector&        nodes
                          );

void dfs(Node*                  root,
         std::stringstream&     tree,
         std::string&           alphabet
         );

std::string encode_tree(Node*           root,
                        size_t&         bits_tree,
                        std::string&    alphabet
                        );

std::string encode_string(const std::string&        content,
                          size_t                    content_size,
                          char_code_map&            char_freq_map,
                          size_t&                   bits_data
                          );

std::string get_out_str(const std::string&      alphabet,
                        const std::string&      tree,
                        const std::string&      str,
                        size_t                  bits_tree,
                        size_t                  bits_data
                        );

std::vector<std::string> get_alphabet_tree_str(const char*  content,
                                               size_t       content_size,
                                               size_t&      alphabet_size,
                                               size_t&      tree_size
                                               );

std::vector<CharData> chars_frequencies(const char* content,
                                        size_t      size
                                        );


std::string   huffman_decoding(const std::string&   encoded_str,
                               const Node*          root,
                               char_code_map&       chars_codes
                               );

char_code_map huffman_encoding(const std::vector<CharData>& input_chars);


void print_statistics(size_t                    input_size,
                      size_t                    output_size,
                      size_t                    help_size,
                      const char_code_map&      chars_codes,
                      bool                      is_console
                     );

size_t get_file_size(const std::string& file_name);

const char* get_char_content(const std::string& file_name,
                             size_t             size
                             );


void write_file(const std::string&      file_name,
                const std::string&      output_str
               );

void free_memory(std::vector<const Node*>& nodes);
