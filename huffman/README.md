Huffman coding


filesystem library:

        #include <experimental/filesystem>
        
compilation flag

        g++ -o huffman main.cpp huffman.cpp -lstdc++fs


huffmans flags:

        -v -- (optional) show alphabet - codes - frequencies
        -d -- decoding
        -c -- encoding

encoding:

        ./huffman -v -c source encoded.bin      
        
        321 -- file size before encoding
        123 -- encoded size
        42  -- huffman tree size  => 123 + 42 == total size of encoded file
    
decoding:

        ./huffman -v -d encoded.bin decoded
        
        123 -- encoded file size
        321 -- decoded file size
        42  -- huffman tree size  => 123 + 42 == total size of encoded file
    
