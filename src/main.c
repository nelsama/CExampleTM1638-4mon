/**
 * ============================================================================
 * TM1638 HELLO WORLD Demo - Monitor 6502
 * ============================================================================
 * Muestra "HELLO WORLD" rotatorio en display TM1638 de 8 dígitos
 * 
 * Características:
 *   - Usa ROM API para timer y UART
 *   - Texto desplazándose hacia la izquierda
 *   - Display de 8 dígitos
 * 
 * Uso:
 *   LOAD TM1638_C 0800
 *   R 0800
 * 
 * ROM API utilizada:
 *   - rom_uart_putc()   - Enviar caracteres
 *   - rom_delay_ms()    - Delays en milisegundos
 * ============================================================================
 */

#include <stdint.h>
#include "romapi.h"
#include "../../libs/tm1638-6502-cc65/include/tm1638.h"

/* ============================================================================
 * HARDWARE
 * ============================================================================ */
#define LEDS            (*(volatile uint8_t *)0xC001)   /* LEDs (lógica negativa) */

/* Función para enviar strings usando ROM API */
void uart_print(const char *s) {
    while (*s) rom_uart_putc(*s++);
}

/* ============================================================================
 * FUNCIÓN DE SCROLL DE TEXTO
 * ============================================================================ */

/**
 * Muestra texto desplazándose en el display de 8 dígitos
 * @param text: Texto a mostrar (con espacios de padding al final)
 * @param delay_ms: Tiempo entre desplazamientos en milisegundos
 */
void scroll_text(const char *text, uint16_t delay_ms) {
    uint8_t len = 0;
    uint8_t pos = 0;
    char display_buf[9];  /* 8 caracteres + null terminator */
    uint8_t i;
    
    /* Calcular longitud del texto */
    while (text[len] != '\0') len++;
    
    /* Si el texto es menor o igual a 8, mostrarlo directamente */
    if (len <= 8) {
        tm1638_show_text(text);
        return;
    }
    
    /* Scroll infinito */
    while (1) {
        /* Copiar 8 caracteres desde la posición actual */
        for (i = 0; i < 8; i++) {
            display_buf[i] = text[(pos + i) % len];
        }
        display_buf[8] = '\0';
        
        /* Mostrar en el display */
        tm1638_show_text(display_buf);
        
        /* Esperar usando ROM API */
        rom_delay_ms(delay_ms);
        
        /* Avanzar posición */
        pos++;
        if (pos >= len) pos = 0;
        
        if(rom_uart_rx_ready()) {
            char c = rom_uart_getc();
            if(c == 'q' || c == 'Q') {
                tm1638_clear_display();
                uart_print("Scroll detenido por usuario.\r\n");
                break;
            }
        }
    }
}


/* ============================================================================
 * MAIN
 * ============================================================================ */

int main(void) {


    /* Texto para hacer scroll - agregar espacios para separación visual */
    const char *mensaje = "Hola mundo desde TM1638    ";

    /* Banner por UART */
    uart_print("\r\n");
    uart_print("===================================\r\n");
    uart_print("  TM1638 HELLO WORLD - Monitor 6502\r\n");
    uart_print("===================================\r\n");
    uart_print("Mostrando texto rotatorio...\r\n\r\n");
    uart_print("Presione 'q' para salir.\r\n\r\n");
    
    /* Apagar LEDs del hardware (si existen) */
    LEDS = 0xFF;

    /* Inicializar TM1638 */
    tm1638_init();
    tm1638_set_brightness(4);  /* Brillo medio (0-7) */
    
    /* Mensaje de bienvenida temporal */
    tm1638_show_text("HELLO   ");
    rom_delay_ms(1000);
    
    /* Iniciar scroll infinito (300ms entre desplazamientos) */
    scroll_text(mensaje, 100);
    
    return 0;
}
