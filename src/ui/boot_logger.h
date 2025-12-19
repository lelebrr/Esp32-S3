#pragma once

/**
 * @file boot_logger.h
 * @brief Sistema de log para tela de boot
 * 
 * Captura mensagens de log e as exibe na tela de boot
 * com estilo hacker (letras verdes em fundo preto)
 */

#include <Arduino.h>
#include <vector>

#define MAX_BOOT_LOGS 12      // Máximo de linhas visíveis
#define MAX_LOG_LENGTH 50     // Máximo de caracteres por linha

// Estrutura de um log
struct BootLogEntry {
    char text[MAX_LOG_LENGTH];
    uint32_t color;           // Cor hex (0x00FF41 = verde)
    bool isError;
};

/**
 * @brief Gerenciador de logs do boot
 */
class BootLogger {
public:
    BootLogger();
    
    /** Adiciona uma linha de log */
    void addLog(const char* text, uint32_t color = 0x00FF41, bool isError = false);
    
    /** Atalhos para tipos de log */
    void logOK(const char* module, const char* message);
    void logError(const char* module, const char* message);
    void logProgress(int current, int total, const char* message);
    
    /** Limpa todos os logs */
    void clear();
    
    /** Retorna os logs */
    const std::vector<BootLogEntry>& getLogs() const { return _logs; }
    
    /** Número total de logs */
    int getLogCount() const { return _logs.size(); }
    
    /** Atualiza progresso (0-100) */
    void setProgress(int progress) { _progress = progress; }
    int getProgress() const { return _progress; }
    
    /** Define etapa atual */
    void setStep(int current, int total);
    int getCurrentStep() const { return _currentStep; }
    int getTotalSteps() const { return _totalSteps; }
    
private:
    std::vector<BootLogEntry> _logs;
    int _progress;
    int _currentStep;
    int _totalSteps;
};

// Cores padrão
#define LOG_COLOR_GREEN     0x00FF41
#define LOG_COLOR_CYAN      0x00FFFF
#define LOG_COLOR_RED       0xFF0055
#define LOG_COLOR_YELLOW    0xFFCC00
#define LOG_COLOR_GRAY      0x666666
#define LOG_COLOR_WHITE     0xFFFFFF

// Instância global
extern BootLogger bootLogger;
