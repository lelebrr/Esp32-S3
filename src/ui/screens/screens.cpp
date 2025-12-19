/**
 * @file screens.cpp
 * @brief Inicialização de todas as telas
 */

#include "screens.h"
#include <debug_log.h>

void initAllScreens(lv_obj_t *parent) {
    // Novo menu carousel estilo Lele (uma função por tela)
    menuCarousel.create(parent);

    // Menu grid antigo (desabilitado - pode remover depois)
    // menuGrid.create(parent);

    statsScreen.create(parent);
    networksScreen.create(parent);
    handshakesScreen.create(parent);

    LOG_INFO("Todas as telas inicializadas (usando Carousel)");
}
