#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "types.h"

int errorCount = 0;
int warningCount = 0;
std::string assemblyFilename;
std::string outputFilename;

std::vector<uint8_t> outputBinaryData;

//#define DEBUG_OUTPUT_LABEL_OFFSETS
//#define DEBUG_OUTPUT_VARIABLE_OFFSET

#include "registers.h"
#include "utill.h"
#include "assembler.h"

int main(int argc, char* argv[]) {
    if (argc < 2) { // Usage tip - Must have at least two argument
        std::cerr << "Usage: " << argv[0] << " <input.asm> [output.bin]\n";
        return 1;
    }
    
    // Set the output file name
    outputFilename = argv[2];
    
    // Open the file
    std::ifstream assemblySource(argv[1], std::ios::in);
    
    // Check if the file was successfully opened
    if (!assemblySource) {
        std::cerr << "Error: Could not open the file.\n";
        return 1;
    }
    
    assemblyFilename = argv[1];
    
    // Read the file into a buffer
    std::ostringstream buffer;
    buffer << assemblySource.rdbuf();
    std::string fileContents = buffer.str();
    assemblySource.close();
    
    std::vector<std::string> assemblyLines = String.Explode(fileContents, '\n');
    
    // Assemble the file
    std::cout << "Assembling " << assemblyFilename << "...";
    
    // Bake the assembly file
    int theCakeBaked = BakeTheCake(assemblyLines);
    
    // Check if the cake baked
    if (errorCount > 0 || warningCount > 0 || theCakeBaked != 0) {
        return -1;
    }
    
    
    // Build a hex file
    if (outputFilename.find(".hex") != std::string::npos) {
        std::string hexFileString = "uint8_t program[] = {\n";
        hexFileString += "    ";
        uint8_t counter=0;
        for (uint32_t i=0; i < outputBinaryData.size(); i++) {
            hexFileString += toHexString( outputBinaryData[i] ) + ", ";
            counter++;
            if (counter == 24) {
                counter=0;
                hexFileString += '\n';
                hexFileString += "    ";
            }
        }
        hexFileString += "\n};";
        
        // Write the file
        std::ofstream outputFileHex(outputFilename, std::ios::binary);
        if (!outputFileHex) {
            std::cerr << "Error opening output file" << std::endl << std::endl;
            return -1;
        }
        outputFileHex.write(reinterpret_cast<const char*>(hexFileString.data()), hexFileString.size());
        outputFileHex.close();
    }
    
    
    // Build the resulting binary file
    if (outputFilename.find(".bin") != std::string::npos) {
        std::ofstream outputFileBin(outputFilename, std::ios::binary);
        if (!outputFileBin) {
            std::cerr << "Error opening output file" << std::endl << std::endl;
            return -1;
        }
        
        outputFileBin.write(reinterpret_cast<const char*>(outputBinaryData.data()), outputBinaryData.size());
        outputFileBin.close();
    }
    
    return 0;
}
