#ifndef __BINARYIO_H__
#define __BINARYIO_H__

#include <string>
#include <vector>

class BinaryIO {
public:
    /* Writes to a binary file named ++filename++.
     * ++serial_tree++ should be a serialized Huffman tree, and
     * ++bitstring++ should be a string of 0s and 1s 
     * comprising the encoded text.
     * 
     * Possible exception: Will throw a runtime_error if given
     * ++bitstring++ is larger than 2^32 bits.
     */
    void writeFile(std::string filename,
                   std::string serial_tree, std::string bitstring);
    
    /* Reads from zapped file named ++filename++.
     * returns a 2-element string vector.
     * Element 0 contains the serialized Huffman tree, and
     * element 1 contains the encoded text.
     */
    std::vector<std::string> readFile(std::string filename);
private:
    template<typename streamtype>
    void open_or_die(streamtype &stream, std::string filename);    
};

#endif
