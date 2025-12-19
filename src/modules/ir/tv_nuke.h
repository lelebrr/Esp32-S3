/**
 * @file tv_nuke.h
 * @brief Módulo TV Nuke - Desliga todas as TVs via IR
 * @author Lele Origin Team
 * @version 1.0
 * @date 2024-12-13
 *
 * Este módulo envia códigos de desligar para centenas de modelos de TV
 * usando diferentes protocolos IR:
 * - NEC (maioria das marcas)
 * - Samsung
 * - Sony (SIRC)
 * - RC5/RC6 (Philips)
 * - LG
 *
 * Marcas suportadas: Samsung, LG, Sony, Philips, Philco, AOC, TCL,
 * Semp, Toshiba, Hisense, Sharp, Sanyo, JVC, RCA, Vizio, Roku, Fire TV,
 * Magnavox, Insignia, Emerson, Sylvania, Westinghouse e genéricos.
 *
 * @warning Use apenas para fins educacionais e em ambientes autorizados.
 */

#ifndef __TV_NUKE_H__
#define __TV_NUKE_H__

#include <Arduino.h>

/**
 * @brief Inicializa o módulo TV Nuke
 * @note Configura o pino IR TX
 */
void tv_nuke_init();

/**
 * @brief Inicia o ataque TV Nuke
 * @return true se iniciou com sucesso
 */
bool tv_nuke_start();

/**
 * @brief Para o ataque TV Nuke
 */
void tv_nuke_stop();

/**
 * @brief Atualiza o ataque (deve ser chamado no loop)
 * @return Número de códigos enviados nesta iteração
 * @note Envia ~10 códigos/segundo para dar tempo do LED IR transmitir
 */
int tv_nuke_update();

/**
 * @brief Verifica se o ataque está ativo
 * @return true se está atacando
 */
bool tv_nuke_is_active();

/**
 * @brief Obtém total de códigos enviados
 * @return Contador de códigos
 */
uint32_t tv_nuke_get_codes_sent();

/**
 * @brief Obtém nome da marca atual sendo enviada
 * @return String com nome da marca (ex: "Samsung", "LG")
 */
const char* tv_nuke_get_current_brand();

#endif // __TV_NUKE_H__
