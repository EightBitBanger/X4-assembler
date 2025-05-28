#define MAX_PROGRAM_SIZE 256
#include <map>

// Opcodes
#define  NOP_OPCODE    0x90
#define  MOVB_OPCODE   0x89
#define  MOVR_OPCODE   0x88
#define  MOVA_OPCODE   0x83
#define  MOVMW_OPCODE  0x82
#define  MOVMR_OPCODE  0x81

#define  ADD_OPCODE    0x00
#define  SUB_OPCODE    0x80
#define  MUL_OPCODE    0xF6
#define  DIV_OPCODE    0xF4
#define  INC_OPCODE    0xFD
#define  DEC_OPCODE    0xFC
#define  CMP_OPCODE    0x38
#define  CMPR_OPCODE   0x39
#define  JMP_OPCODE    0xFE
#define  JE_OPCODE     0x74
#define  JNE_OPCODE    0xF3
#define  JG_OPCODE     0x75
#define  JL_OPCODE     0xF1
#define  CALL_OPCODE   0x9A
#define  RET_OPCODE    0xCB
#define  PUSH_OPCODE   0xF0
#define  POP_OPCODE    0x0F
#define  INT_OPCODE    0xCD
#define  STI_OPCODE    0xFB
#define  CLI_OPCODE    0xFA

// Error messages
const std::string errorUnknownLabel = "Unknown label ";

struct Label {
    std::string name;
    uint32_t byteOffset;
};

union Pointer {
    uint32_t address;
    uint8_t  byte_t[4];
};

std::vector<Label> variableIndex;
std::vector<Label> labelIndex;

int BakeTheCake(std::vector<std::string>& assemblyLines) {
    
    uint32_t byteOffset = 0;
    
    // Gather all the labels and their associated offsets within the program
    uint8_t textFound = 0;
    for (unsigned int  ln=0; ln < assemblyLines.size(); ln++) {
        
        if (assemblyLines[ln].find("section") != std::string::npos) {
            
            if (assemblyLines[ln].find(".text") != std::string::npos) {
                textFound = 1;
                break;
            }
            
            if (assemblyLines[ln].find(".data") != std::string::npos) 
                continue;
            
        }
        
        // Add variable to the index
        if (assemblyLines[ln].find("=") != std::string::npos) {
            assemblyLines[ln] = StringRemoveAllWhitespace(assemblyLines[ln]);
            std::vector<std::string> varLine = String.Explode(assemblyLines[ln], '=');
            
            Label varLabel;
            varLabel.name = varLine[0];
            
            // Check hex value
            if (assemblyLines[ln].find("0x") != std::string::npos) {
                varLabel.byteOffset = hex_to_byte( varLine[1] );
            } else {
                // Integer
                varLabel.byteOffset = String.ToInt( varLine[1] );
            }
            
            
            uint16_t index = variableIndex.size();
            std::cout << std::endl << Int.ToString( index );
            
            String.Uppercase(varLabel.name);
            variableIndex.push_back(varLabel);
            
        }
    }
    
    // Check no entry point
    if (textFound == 0) {
        ThrowError(assemblyLines.size(), "'Section .text' not found"); return -1;
    }
    
    // Gather all the labels and their associated offsets within the program
    for (unsigned int  ln=0; ln < assemblyLines.size(); ln++) {
        
        std::string& line = assemblyLines[ln];
        
        if (line == "") 
            continue;
        
        // Check label
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            line[pos] = '\0';
            
            // Check label duplicates
            for (uint8_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                
                if (label == line) {
                    ThrowError(ln, "Duplicate label " + line); return -1;
                }
            }
            
            
            // Add a new label
            Label label;
            // Case independent label
            label.name = line;
            String.Uppercase(label.name);
            
            label.byteOffset = byteOffset;
            labelIndex.push_back(label);
            
            continue;
        }
        
        // Count the byte offset for the next label
        // This is based off each instruction until the next label
        
        line = StringRemoveLeadingWhitespace(line);
        
        if (line.compare(0, 3, "ADD")  == 0) {byteOffset += 3; continue;}
        if (line.compare(0, 3, "SUB")  == 0) {byteOffset += 3; continue;}
        if (line.compare(0, 3, "MUL")  == 0) {byteOffset += 4; continue;}
        if (line.compare(0, 3, "DIV")  == 0) {byteOffset += 4; continue;}
        
        if (line.compare(0, 3, "RET")  == 0) {byteOffset += 1; continue;}
        if (line.compare(0, 3, "CLI")  == 0) {byteOffset += 1; continue;}
        if (line.compare(0, 3, "STI")  == 0) {byteOffset += 1; continue;}
        if (line.compare(0, 3, "NOP")  == 0) {byteOffset += 1; continue;}
        
        if (line.compare(0, 3, "INC")  == 0) {byteOffset += 2; continue;}
        if (line.compare(0, 3, "DEC")  == 0) {byteOffset += 2; continue;}
        
        if (line.compare(0, 3, "POP")  == 0) {byteOffset += 2; continue;}
        if (line.compare(0, 4, "PUSH") == 0) {byteOffset += 2; continue;}
        if (line.compare(0, 3, "INT")  == 0) {byteOffset += 2; continue;}
        
        if (line.compare(0, 3, "CMP")  == 0) {byteOffset += 3; continue;}
        
        // Check MOV sub type
        if (line.compare(0, 3, "MOV")  == 0) {
            if (line.find("[") != std::string::npos && 
                line.find("]") != std::string::npos) {
                byteOffset += 6;
            } else {
                if (line.find("$") != std::string::npos) {
                    // Address move
                    byteOffset += 6;
                } else {
                    // Byte/register move
                    byteOffset += 3;
                }
            }
            continue;
        }
        
        if (line.compare(0, 3, "JMP")  == 0) {byteOffset += 5; continue;}
        if (line.compare(0, 2, "JE")   == 0) {byteOffset += 5; continue;}
        if (line.compare(0, 3, "JNE")  == 0) {byteOffset += 5; continue;}
        if (line.compare(0, 2, "JG")   == 0) {byteOffset += 5; continue;}
        if (line.compare(0, 2, "JL")   == 0) {byteOffset += 5; continue;}
        if (line.compare(0, 4, "CALL") == 0) {byteOffset += 5; continue;}
        
        if (line.compare(0, 2, "DB")   == 0) {
            std::vector<std::string> lineExplode = String.Explode(line, 0x27);
            std::string byteArray = lineExplode[1];
            if (lineExplode.size() > 2) {
                std::string bytePost = lineExplode[2];
                if (bytePost.find('0') != std::string::npos) 
                    byteOffset++;
            }
            byteOffset += byteArray.length();
            continue;
        }
        
    }
    
#ifdef DEBUG_OUTPUT_LABEL_OFFSETS
    // TEST - Display the labels and their offsets
    std::cout << std::endl << std::endl;
    for (unsigned int  i=0; i < labelIndex.size(); i++) 
        std::cout << labelIndex[i].name << Int.ToString( labelIndex[i].byteOffset ) << std::endl;
#endif
    
#ifdef DEBUG_OUTPUT_VARIABLE_OFFSET
    // TEST - Display the variable name and offsets
    for (unsigned int  i=0; i < variableIndex.size(); i++) 
        std::cout << std::endl << variableIndex[i].name << Int.ToString( variableIndex[i].byteOffset );
#endif
    
    // Assemble the program using the label offsets as address references
    outputBinaryData.resize( 1024 * 32 );
    
    uint32_t programSize = 0;
    
    for (unsigned int ln=0; ln < assemblyLines.size(); ln++) {
        
        std::string line = assemblyLines[ln];
        
        std::vector<std::string> explodedLine = String.Explode(line, ' ');
        
        if (line.compare(0, 3, "ADD") == 0) {outputBinaryData[programSize] = ADD_OPCODE; programSize += 3; continue;}
        if (line.compare(0, 3, "SUB") == 0) {outputBinaryData[programSize] = SUB_OPCODE; programSize += 3; continue;}
        if (line.compare(0, 3, "MUL") == 0) {outputBinaryData[programSize] = MUL_OPCODE; programSize += 4; continue;}
        if (line.compare(0, 3, "DIV") == 0) {outputBinaryData[programSize] = DIV_OPCODE; programSize += 4; continue;}
        
        if (line.compare(0, 3, "RET") == 0) {outputBinaryData[programSize] = RET_OPCODE; programSize += 1; continue;}
        if (line.compare(0, 3, "CLI") == 0) {outputBinaryData[programSize] = CLI_OPCODE; programSize += 1; continue;}
        if (line.compare(0, 3, "STI") == 0) {outputBinaryData[programSize] = STI_OPCODE; programSize += 1; continue;}
        if (line.compare(0, 3, "NOP") == 0) {outputBinaryData[programSize] = NOP_OPCODE; programSize += 1; continue;}
        
        if (line.compare(0, 3, "INC") == 0) {outputBinaryData[programSize] = INC_OPCODE; programSize += 2; continue;}
        if (line.compare(0, 3, "DEC") == 0) {outputBinaryData[programSize] = DEC_OPCODE; programSize += 2; continue;}
        
        // DB
        //
        if (line.compare(0, 2, "DB") == 0) {
            
            std::vector<std::string> lineExplode = String.Explode(line, 0x27);
            std::string byteArray = lineExplode[1];
            
            size_t length = byteArray.length();
            for (uint8_t i=0; i < length; i++) 
                outputBinaryData[programSize + i] = byteArray[i];
            programSize += length;
            
            if (lineExplode.size() > 2) {
                std::string bytePost = lineExplode[2];
                if (bytePost.find('0') != std::string::npos) {
                    outputBinaryData[programSize + length] = 0x00;
                    programSize++;
                }
            }
            continue;
        }
        
        // MOVB / MOVR - Move a byte or a register
        //
        if (line.compare(0, 3, "MOV") == 0) {
            
            std::string paramA = explodedLine[1];
            std::string paramB = explodedLine[2];
            
            // Remove comma
            paramA.resize(paramA.size() - 1);
            
            // Check memory move operation
            if (line.find("[") != std::string::npos && 
                line.find("]") != std::string::npos) {
                
                //MOVMW_OPCODE
                //MOVMR_OPCODE
                
                std::cout << std::endl << paramA;
                std::cout << std::endl << paramB;
                
                /*
                String.Uppercase(paramB);
                paramB.erase(0, 1);
                paramB.erase(paramB.size()-1, 1);
                
                // Find the label address
                union Pointer memoryAddress = {0};
                uint8_t found = 0;
                for (uint16_t i=0; i < variableIndex.size(); i++) {
                    std::string label = variableIndex[i].name;
                    label.resize(paramB.length());
                    
                    if (paramB.compare(label) != 0) 
                        continue;
                    memoryAddress.address = variableIndex[i].byteOffset;
                    std::cout << std::endl << paramB;
                    found = 1;
                    break;
                }
                if (found == 0) ThrowError(ln, "Unknown variable " + paramB);
                for (uint8_t i=0; i < 4; i++) 
                    outputBinaryData[programSize + 2 + i] = memoryAddress.byte_t[i];
                */
                
                programSize += 6;
                continue;
            }
            
            
            // Check 8-bit register
            uint8_t regTypeA = get_register_code(paramA);
            if (regTypeA != 0xff) {
                outputBinaryData[programSize] = MOVB_OPCODE;
                outputBinaryData[programSize+1] = regTypeA;
                
                // Check second register
                uint8_t regTypeB = get_register_code(paramB);
                if (regTypeB == 0xff) {
                    if (paramB.find("0x") == std::string::npos) {ThrowError(ln, "Unknown parameter"); return -1;}
                    outputBinaryData[programSize+2] = hex_to_byte(paramB);
                } else {
                    outputBinaryData[programSize+2] = regTypeB;  // Should be a register
                    outputBinaryData[programSize] = MOVR_OPCODE; // Correct the opcode
                }
                
                programSize += 3;
                continue;
            }
            
            // Check full 16-bit register
            regTypeA = get_register_code16(paramA);
            if (regTypeA != 0xff) {
                outputBinaryData[programSize] = MOVA_OPCODE;
                outputBinaryData[programSize+1] = regTypeA;
                paramB.erase(0, 1); // Remove the $ symbol
                String.Uppercase(paramB);
                
                // Find the label address
                union Pointer jumpAddress = {0};
                uint8_t found = 0;
                for (uint16_t i=0; i < labelIndex.size(); i++) {
                    std::string label = labelIndex[i].name;
                    label.resize(paramB.length());
                    String.Uppercase(label);
                    if (paramB.compare(label) != 0) 
                        continue;
                    jumpAddress.address = labelIndex[i].byteOffset;
                    found = 1;
                    break;
                }
                if (found == 0) ThrowError(ln, "Unknown label " + paramB);
                for (uint8_t i=0; i < 4; i++) 
                    outputBinaryData[programSize + 2 + i] = jumpAddress.byte_t[i];
                
                programSize += 6;
                continue;
            }
            
            ThrowError(ln, "Unknown register");
            return -1;
        }
        
        // INT
        //
        if (line.compare(0, 3, "INT") == 0) {
            outputBinaryData[programSize] = INT_OPCODE;
            std::string paramA = explodedLine[1];
            // Remove comma
            if (paramA.find("0x") == std::string::npos) {ThrowError(ln, "Unknown parameter"); return -1;}
            // Parameter is a byte
            outputBinaryData[programSize+1] = hex_to_byte(paramA);
            programSize += 2;
            continue;
        }
        
        // PUSH
        //
        if (line.compare(0, 4, "PUSH") == 0) {
            outputBinaryData[programSize] = PUSH_OPCODE;
            std::string paramA = explodedLine[1];
            uint8_t regTypeA = get_register_code(paramA);
            if (regTypeA == 0xff) {ThrowError(ln, "Unknown register"); return -1;}
            outputBinaryData[programSize+1] = regTypeA;
            programSize += 2;
            continue;
        }
        
        // POP
        //
        if (line.compare(0, 3, "POP") == 0) {
            outputBinaryData[programSize] = POP_OPCODE;
            std::string paramA = explodedLine[1];
            uint8_t regTypeA = get_register_code(paramA);
            if (regTypeA == 0xff) {ThrowError(ln, "Unknown register"); return -1;}
            outputBinaryData[programSize+1] = regTypeA;
            programSize += 2;
            continue;
        }
        
        // CMP / CMPR
        //
        if (line.compare(0, 3, "CMP") == 0) {
            outputBinaryData[programSize] = CMP_OPCODE;
            std::string paramA = explodedLine[1];
            std::string paramB = explodedLine[2];
            
            // Remove comma
            paramA.resize(paramA.size() - 1);
            
            // Check first register
            uint8_t regTypeA = get_register_code(paramA);
            if (regTypeA == 0xff) {ThrowError(ln, "Unknown register"); return -1;}
            outputBinaryData[programSize+1] = regTypeA;
            
            // Check second register
            uint8_t regTypeB = get_register_code(paramB);
            if (regTypeB == 0xff) {
                if (paramB.find("0x") == std::string::npos) {ThrowError(ln, "Unknown parameter"); return -1;}
                outputBinaryData[programSize+2] = hex_to_byte(paramB);
            } else {
                outputBinaryData[programSize+2] = regTypeB;  // Should be a register
                outputBinaryData[programSize] = CMPR_OPCODE; // Correct the opcode
            }
            programSize += 3;
            continue;
        }
        
        // JMP
        //
        if (line.compare(0, 3, "JMP") == 0) {
            outputBinaryData[programSize] = JMP_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        // JE
        //
        if (line.compare(0, 2, "JE") == 0) {
            outputBinaryData[programSize] = JE_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        // JNE
        //
        if (line.compare(0, 3, "JNE") == 0) {
            outputBinaryData[programSize] = JNE_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        // JG
        //
        if (line.compare(0, 2, "JG") == 0) {
            outputBinaryData[programSize] = JG_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        // JL
        //
        if (line.compare(0, 2, "JL") == 0) {
            outputBinaryData[programSize] = JL_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        // CALL
        //
        if (line.compare(0, 4, "CALL") == 0) {
            outputBinaryData[programSize] = CALL_OPCODE;
            std::string paramA = explodedLine[1];
            // Find the case label and apply the address
            union Pointer jumpAddress = {0};
            uint8_t found = 0;
            for (uint16_t i=0; i < labelIndex.size(); i++) {
                std::string label = labelIndex[i].name;
                label.resize(paramA.length());
                String.Uppercase(label);
                if (paramA.compare(label) != 0) 
                    continue;
                jumpAddress.address = labelIndex[i].byteOffset;
                found = 1;
                break;
            }
            if (found == 0) ThrowError(ln, "Unknown label " + paramA);
            for (uint8_t i=0; i < 4; i++) 
                outputBinaryData[programSize + 1 + i] = jumpAddress.byte_t[i];
            programSize += 5;
            continue;
        }
        
        
    }
    
    // Set the size of the binary program file
    outputBinaryData.resize( programSize );
    
    //ThrowWarning(10, errorUnknownLabel + "BEGIN");
    
    return 0;
}


