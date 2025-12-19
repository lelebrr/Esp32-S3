void speak_from_progmem(const char* progmem_str) { 
    char buffer[256]; 
    strncpy_P(buffer, progmem_str, sizeof(buffer) - 1); 
    buffer[sizeof(buffer) - 1] = '\0'; 
    think_and_speak(String(buffer)); 
} 
