#define  rAL   0x00
#define  rAH   0x01
#define  rBL   0x02
#define  rBH   0x03
#define  rCL   0x04
#define  rCH   0x05
#define  rDL   0x06
#define  rDH   0x07

#define  rAX   0x00
#define  rBX   0x02
#define  rCX   0x04
#define  rDX   0x06


// Function to get register byte code
uint8_t get_register_code(const std::string& reg) {
    if (reg == "AL") return rAL;
    if (reg == "AH") return rAH;
    if (reg == "BL") return rBL;
    if (reg == "BH") return rBH;
    if (reg == "CL") return rCL;
    if (reg == "CH") return rCH;
    if (reg == "DL") return rDL;
    if (reg == "DH") return rDH;
    return 0xFF; // Invalid register
}

// Function to get register byte code
uint16_t get_register_code16(const std::string& reg) {
    if (reg == "AX") return rAX;
    if (reg == "BX") return rBX;
    if (reg == "CX") return rCX;
    if (reg == "DX") return rDX;
    return 0xFF; // Invalid register
}

