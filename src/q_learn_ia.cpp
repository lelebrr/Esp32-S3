#include "q_learn_ia.h"
#include <esp_system.h>
#include <esp_heap_caps.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/gpio.h>
#include <esp_random.h>
#include <SD.h>
#include <SPIFFS.h>
#include <globals.h>

// Configurações otimizadas da Q-table
#define Q_TABLE_SIZE (1024 * 2)          // 2048 estados (2KB)
#define Q_TABLE_ACTIONS 8                // 8 ações possíveis
#define Q_TABLE_FILE "/q_table.bin"      // Arquivo em SD
#define Q_LEARN_ALPHA 0.1f               // Taxa de aprendizado
#define Q_LEARN_GAMMA 0.9f               // Fator de desconto
#define Q_LEARN_EPSILON 0.1f             // Exploração inicial
#define Q_SAVE_INTERVAL_MS 30000         // Salva a cada 30s

// Estados da IA (baseados em contexto)
enum AIState {
    STATE_IDLE = 0,
    STATE_WIFI_SCAN = 1,
    STATE_BLE_SPOOF = 2,
    STATE_DEAUTH_ATTACK = 3,
    STATE_NFC_FAULT = 4,
    STATE_EVIL_TWIN = 5,
    STATE_PMKID_CAPTURE = 6,
    STATE_SLEEP_MODE = 7
};

// Ações disponíveis
enum AIAction {
    ACTION_DO_NOTHING = 0,
    ACTION_START_WIFI_SCAN = 1,
    ACTION_START_BLE_SPAM = 2,
    ACTION_START_DEAUTH = 3,
    ACTION_TRIGGER_NFC_FAULT = 4,
    ACTION_START_EVIL_TWIN = 5,
    ACTION_START_PMKID = 6,
    ACTION_ENTER_SLEEP = 7
};

// Q-table em PSRAM otimizada
static float* q_table = nullptr;
static uint32_t current_state = STATE_IDLE;
static bool ai_learning_active = false;

// Task de aprendizado
static TaskHandle_t q_learning_task = NULL;

// Timer para salvamento automático
static esp_timer_handle_t save_timer = NULL;

// Mutex para thread safety
static SemaphoreHandle_t ai_mutex = NULL;

// Estatísticas de aprendizado
static uint32_t learning_iterations = 0;
static uint32_t successful_actions = 0;
static float average_reward = 0.0f;

// Callback do timer de salvamento
static void save_timer_callback(void* arg) {
    save_q_table_to_sd();
}

// Função para calcular recompensa baseada no contexto
static float calculate_reward(AIState state, AIAction action) {
    float reward = 0.0f;

    switch (action) {
        case ACTION_DO_NOTHING:
            reward = -0.1f;  // Penaliza inatividade
            break;
        case ACTION_START_WIFI_SCAN:
            reward = 0.5f;   // Bom para descoberta
            break;
        case ACTION_START_BLE_SPAM:
            reward = 0.8f;   // Alto impacto
            break;
        case ACTION_START_DEAUTH:
            reward = 1.0f;   // Ataque direto
            break;
        case ACTION_TRIGGER_NFC_FAULT:
            reward = 0.9f;   // Ataque físico
            break;
        case ACTION_START_EVIL_TWIN:
            reward = 1.2f;   // Ataque avançado
            break;
        case ACTION_START_PMKID:
            reward = 0.7f;   // Captura credenciais
            break;
        case ACTION_ENTER_SLEEP:
            reward = -0.5f;  // Penaliza sono desnecessário
            break;
    }

    // Modificadores baseados no estado
    if (state == STATE_IDLE && action != ACTION_DO_NOTHING) {
        reward += 0.2f;  // Bônus por sair do idle
    }

    // Penaliza ações consecutivas iguais
    static AIAction last_action = ACTION_DO_NOTHING;
    if (action == last_action) {
        reward -= 0.3f;
    }
    last_action = action;

    return reward;
}

// Função para escolher ação usando epsilon-greedy
static AIAction choose_action(uint32_t state) {
    if (q_table == nullptr) return ACTION_DO_NOTHING;

    // Epsilon-greedy exploration
    float epsilon = Q_LEARN_EPSILON * (1.0f - (learning_iterations / 10000.0f));  // Decai com tempo
    if (esp_random() / (float)UINT32_MAX < epsilon) {
        return (AIAction)(esp_random() % Q_TABLE_ACTIONS);
    }

    // Escolhe ação com maior Q-value
    uint32_t state_offset = state * Q_TABLE_ACTIONS;
    AIAction best_action = ACTION_DO_NOTHING;
    float best_value = q_table[state_offset];

    for (int i = 1; i < Q_TABLE_ACTIONS; i++) {
        if (q_table[state_offset + i] > best_value) {
            best_value = q_table[state_offset + i];
            best_action = (AIAction)i;
        }
    }

    return best_action;
}

// Função para executar ação
static bool execute_action(AIAction action) {
    bool success = false;

    switch (action) {
        case ACTION_DO_NOTHING:
            // Não faz nada
            success = true;
            break;

        case ACTION_START_WIFI_SCAN:
            // Implementar scan WiFi
            log_i("[AI] Starting WiFi scan");
            success = true;  // Simulado
            break;

        case ACTION_START_BLE_SPAM:
            // Inicia BLE spam
            extern bool start_ble_spam_br();
            success = start_ble_spam_br();
            if (success) log_i("[AI] BLE spam started");
            break;

        case ACTION_START_DEAUTH:
            // Inicia deauth (precisa target)
            log_i("[AI] Deauth action selected (needs target)");
            success = true;  // Simulado
            break;

        case ACTION_TRIGGER_NFC_FAULT:
            // Gatilho fault injection
            extern void trigger_nfc_fault_manual();
            trigger_nfc_fault_manual();
            success = true;
            log_i("[AI] NFC fault triggered");
            break;

        case ACTION_START_EVIL_TWIN:
            // Evil Twin (simulado)
            log_i("[AI] Evil Twin action selected");
            success = true;
            break;

        case ACTION_START_PMKID:
            // PMKID capture (simulado)
            log_i("[AI] PMKID capture action selected");
            success = true;
            break;

        case ACTION_ENTER_SLEEP:
            // Entra em sleep mode
            extern void conditional_deep_sleep();
            conditional_deep_sleep();
            success = true;
            log_i("[AI] Entering sleep mode");
            break;
    }

    return success;
}

// Função para atualizar Q-table
static void update_q_table(uint32_t state, AIAction action, float reward, uint32_t next_state) {
    if (q_table == nullptr) return;

    uint32_t state_action_idx = state * Q_TABLE_ACTIONS + action;

    // Encontra melhor ação para próximo estado
    uint32_t next_state_offset = next_state * Q_TABLE_ACTIONS;
    float max_next_q = q_table[next_state_offset];
    for (int i = 1; i < Q_TABLE_ACTIONS; i++) {
        if (q_table[next_state_offset + i] > max_next_q) {
            max_next_q = q_table[next_state_offset + i];
        }
    }

    // Q-learning update
    float current_q = q_table[state_action_idx];
    float new_q = current_q + Q_LEARN_ALPHA * (reward + Q_LEARN_GAMMA * max_next_q - current_q);
    q_table[state_action_idx] = new_q;
}

// Função para determinar próximo estado baseado no contexto
static uint32_t determine_next_state() {
    // Lógica simples baseada em flags globais
    extern bool attack_running;
    extern bool ai_mode;
    extern bool stealth_mode;

    if (!ai_mode) return STATE_IDLE;
    if (attack_running) return STATE_DEAUTH_ATTACK;  // Exemplo
    if (stealth_mode) return STATE_SLEEP_MODE;

    // Estado aleatório para simulação
    return esp_random() % Q_TABLE_ACTIONS;
}

// Task principal de aprendizado por reforço
static void q_learning_worker(void* pvParameters) {
    log_i("[AI] Q-learning task started");

    while (ai_learning_active) {
        if (xSemaphoreTake(ai_mutex, pdMS_TO_TICKS(100)) == pdTRUE) {
            // Escolhe ação
            AIAction action = choose_action(current_state);

            // Executa ação
            bool action_success = execute_action(action);

            // Calcula recompensa
            float reward = calculate_reward((AIState)current_state, action);
            if (action_success) {
                reward += 0.5f;  // Bônus por sucesso
                successful_actions++;
            }

            // Atualiza estatísticas
            average_reward = (average_reward * learning_iterations + reward) / (learning_iterations + 1);
            learning_iterations++;

            // Determina próximo estado
            uint32_t next_state = determine_next_state();

            // Atualiza Q-table
            update_q_table(current_state, action, reward, next_state);

            // Transita para próximo estado
            current_state = next_state;

            xSemaphoreGive(ai_mutex);

            // Log periódico
            if (learning_iterations % 100 == 0) {
                log_i("[AI] Iterations: %lu, Success rate: %.2f%%, Avg reward: %.3f",
                      learning_iterations,
                      (successful_actions * 100.0f) / learning_iterations,
                      average_reward);
            }
        }

        // Delay entre decisões (5-10 segundos)
        vTaskDelay(pdMS_TO_TICKS(5000 + (esp_random() % 5000)));
    }

    vTaskDelete(NULL);
}

// Função para inicializar Q-table
bool init_q_table() {
    // Aloca em PSRAM
    q_table = (float*)ps_malloc(Q_TABLE_SIZE * Q_TABLE_ACTIONS * sizeof(float));
    if (q_table == nullptr) {
        log_e("[AI] Failed to allocate Q-table in PSRAM");
        return false;
    }

    // Inicializa com valores pequenos aleatórios
    for (uint32_t i = 0; i < Q_TABLE_SIZE * Q_TABLE_ACTIONS; i++) {
        q_table[i] = (esp_random() / (float)UINT32_MAX - 0.5f) * 0.1f;  // -0.05 a 0.05
    }

    log_i("[AI] Q-table initialized in PSRAM (%u bytes)", Q_TABLE_SIZE * Q_TABLE_ACTIONS * sizeof(float));
    return true;
}

// Função para salvar Q-table no SD
bool save_q_table_to_sd() {
    if (q_table == nullptr) return false;

    if (!SD.exists("/")) return false;  // SD não montado

    File file = SD.open(Q_TABLE_FILE, FILE_WRITE);
    if (!file) {
        log_e("[AI] Failed to open Q-table file for writing");
        return false;
    }

    size_t bytes_to_write = Q_TABLE_SIZE * Q_TABLE_ACTIONS * sizeof(float);
    size_t bytes_written = file.write((uint8_t*)q_table, bytes_to_write);

    file.close();

    if (bytes_written != bytes_to_write) {
        log_e("[AI] Failed to write complete Q-table (%u/%u bytes)", bytes_written, bytes_to_write);
        return false;
    }

    log_i("[AI] Q-table saved to SD (%u bytes)", bytes_written);
    return true;
}

// Função para carregar Q-table do SD
bool load_q_table_from_sd() {
    if (q_table == nullptr) return false;

    if (!SD.exists(Q_TABLE_FILE)) {
        log_i("[AI] Q-table file not found, using initialized values");
        return false;
    }

    File file = SD.open(Q_TABLE_FILE, FILE_READ);
    if (!file) {
        log_e("[AI] Failed to open Q-table file for reading");
        return false;
    }

    size_t bytes_to_read = Q_TABLE_SIZE * Q_TABLE_ACTIONS * sizeof(float);
    size_t bytes_read = file.read((uint8_t*)q_table, bytes_to_read);

    file.close();

    if (bytes_read != bytes_to_read) {
        log_e("[AI] Failed to read complete Q-table (%u/%u bytes)", bytes_read, bytes_to_read);
        return false;
    }

    log_i("[AI] Q-table loaded from SD (%u bytes)", bytes_read);
    return true;
}

// Função para iniciar aprendizado
bool start_q_learning() {
    if (ai_learning_active) return false;

    // Inicializa componentes
    if (ai_mutex == NULL) {
        ai_mutex = xSemaphoreCreateMutex();
        if (ai_mutex == NULL) return false;
    }

    if (!init_q_table()) return false;
    load_q_table_from_sd();

    // Cria timer de salvamento automático
    esp_timer_create_args_t timer_args = {
        .callback = save_timer_callback,
        .arg = NULL,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "q_save_timer"
    };

    if (esp_timer_create(&timer_args, &save_timer) != ESP_OK) {
        log_e("[AI] Failed to create save timer");
        return false;
    }

    esp_timer_start_periodic(save_timer, Q_SAVE_INTERVAL_MS * 1000);

    // Cria task de aprendizado
    ai_learning_active = true;
    BaseType_t result = xTaskCreatePinnedToCore(
        q_learning_worker,
        "Q_LEARNING",
        4096,
        NULL,
        3,  // Prioridade média
        &q_learning_task,
        1   // Core 1
    );

    if (result != pdPASS) {
        ai_learning_active = false;
        return false;
    }

    log_i("[AI] Q-learning started");
    return true;
}

// Função para parar aprendizado
void stop_q_learning() {
    if (!ai_learning_active) return;

    ai_learning_active = false;

    if (q_learning_task != NULL) {
        vTaskDelete(q_learning_task);
        q_learning_task = NULL;
    }

    if (save_timer != NULL) {
        esp_timer_stop(save_timer);
        esp_timer_delete(save_timer);
        save_timer = NULL;
    }

    // Salva final
    save_q_table_to_sd();

    log_i("[AI] Q-learning stopped");
}

// Funções de estatísticas
uint32_t get_learning_iterations() {
    return learning_iterations;
}

uint32_t get_successful_actions() {
    return successful_actions;
}

float get_average_reward() {
    return average_reward;
}

bool is_q_learning_active() {
    return ai_learning_active;
}

// Função para liberar memória
void cleanup_q_table() {
    stop_q_learning();

    if (q_table != nullptr) {
        free(q_table);
        q_table = nullptr;
    }

    if (ai_mutex != NULL) {
        vSemaphoreDelete(ai_mutex);
        ai_mutex = NULL;
    }
}
