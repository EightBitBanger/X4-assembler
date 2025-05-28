
// Convert hex string to byte
uint8_t hex_to_byte(std::string& hex) {
    if (hex[0] == '0' && 
        hex[1] == 'x') {
        hex.erase(0, 2);
    }
    return static_cast<uint8_t>(std::stoul(hex, nullptr, 16));
}

uint32_t hex_to_address(std::string& hex) {
    if (hex[0] == '0' && 
        hex[1] == 'x') {
        hex.erase(0, 2);
    }
    return static_cast<uint32_t>(std::stoul(hex, nullptr, 16));
}


void ThrowError(int errorLine, std::string errorMessage) {
    errorCount++;
    std::cout << std::endl << std::endl;
    std::cout << assemblyFilename << "(" << (errorLine + 1) << "): Error: " << errorMessage;
    return;
}

void ThrowWarning(int errorLine, std::string errorMessage) {
    warningCount++;
    std::cout << std::endl << std::endl;
    std::cout << assemblyFilename << "(" << (errorLine + 1) << "): Warning: " << errorMessage;
    return;
}




