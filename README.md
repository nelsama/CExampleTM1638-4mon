# TM1638 Demo - Monitor 6502

Este proyecto es una **demostración** del uso del módulo **TM1638** (display de 8 dígitos + LEDs + botones) con el Monitor 6502 en la Tang Nano 9K.

## Características

- ✅ Programa en C usando cc65
- ✅ Usa **ROM API** para UART y delays
- ✅ Muestra texto desplazándose en display de 8 dígitos
- ✅ Mensaje "Hola mundo desde TM1638" en scroll infinito
- ✅ Usa librería TM1638 optimizada para 6502
- ✅ Control de brillo ajustable
- ✅ Código compacto y eficiente

## Estructura del Proyecto

```
Tm1638Demo/
├── src/
│   ├── main.c          # Código fuente principal
│   └── startup.s       # Código de inicio del runtime C
├── config/
│   └── programa.cfg    # Configuración del linker
├── build/              # Archivos objeto (generados)
├── output/             # Binario final (generado)
├── include/
│   └── romapi.h        # Definiciones de ROM API
├── makefile            # Script de compilación
└── README.md           # Este archivo
```

## Hardware Requerido

- **Tang Nano 9K** con Monitor 6502 v2.2.0+
- **Módulo TM1638** (8 dígitos, 8 LEDs, 8 botones)
- Conexiones:
  - STB (Strobe): Puerto configurable
  - CLK (Clock): Puerto configurable  
  - DIO (Data): Puerto configurable

## Software Requerido

- **CC65** instalado en `D:\cc65` (o ajustar ruta en makefile)
- **Monitor 6502 v2.2.0+** con ROM API en $BF00
- **[tm1638-6502-cc65](https://github.com/nelsama/tm1638-6502-cc65)** (incluida en `../../libs/tm1638-6502-cc65`) - Librería optimizada para controlar el módulo TM1638 en sistemas 6502 con cc65

## Compilación

```bash
# Compilar el programa
make

# Limpiar archivos generados
make clean

# Ver tamaño del binario
make info

# Ver mapa de memoria
make map
```

## Uso

### Vía SD Card
1. Compilar con `make`
2. Copiar `output/demo-tm1638.bin` a la SD Card como `TM1638_C`
3. En el monitor:
   ```
   SD                      ; Inicializar SD
   LOAD TM1638_C           ; Cargar programa (default: $0800)
   R                       ; Ejecutar
   ```

### Vía XMODEM
```
XRECV                   ; Recibir via XMODEM (default: $0800)
R                       ; Ejecutar
```

## Funcionalidad

El programa realiza lo siguiente:

1. **Inicialización**: Configura el módulo TM1638 con brillo medio (nivel 4 de 8)
2. **Mensaje de bienvenida**: Muestra "HELLO   " durante 1 segundo
3. **Scroll infinito**: Desplaza el texto "Hola mundo desde TM1638" continuamente hacia la izquierda con un delay de 100ms entre desplazamientos
4. **Información por UART**: Muestra un banner de inicio en la consola serie

Para salir, presionar `CTRL+C` o resetear el monitor.

## Mapa de Memoria

| Rango | Uso |
|-------|-----|
| `$0002-$001F` | Zero Page del Monitor (NO USAR) |
| `$0020-$007F` | Zero Page disponible para programas |
| `$0100-$01FF` | Stack del 6502 (compartido) |
| `$0200-$07FF` | BSS del Monitor (NO USAR) |
| `$0800-$3DFF` | RAM para programas (código, datos, BSS) |
| `$3E00-$3FFF` | Stack de CC65 (512 bytes) |
| `$C000-$C0FF` | Puertos de I/O |
| `$BF00-$BF2F` | ROM API (Jump Table) |

## ROM API Utilizada

```c
void rom_uart_putc(char c);         /* Enviar carácter por UART */
void rom_delay_ms(uint16_t ms);     /* Delay en milisegundos */
```

## Funciones de la Librería TM1638

```c
void tm1638_init(void);                    /* Inicializar módulo */
void tm1638_set_brightness(uint8_t level); /* Brillo (0-7) */
void tm1638_show_text(const char *text);   /* Mostrar texto (máx 8 chars) */
```

## Personalización

Para modificar el comportamiento:

- **Cambiar el mensaje**: Editar la variable `mensaje` en [main.c](src/main.c)
- **Ajustar velocidad**: Modificar el parámetro de delay en `scroll_text()` (línea 111)
- **Cambiar brillo**: Ajustar el valor en `tm1638_set_brightness()` (0=mín, 7=máx)

## Notas Técnicas

- El texto debe incluir espacios al final para crear una separación visual en el scroll
- Si el texto tiene 8 caracteres o menos, se muestra estático sin scroll
- El scroll es cíclico e infinito
- Los LEDs del hardware se apagan al inicio del programa


## ROM API Utilizada

Este programa usa la **ROM API** del monitor, lo que permite:
- ✅ Código más compacto (~777 bytes vs ~1224 bytes con implementación propia)
- ✅ Sin conflictos con el estado del monitor
- ✅ No necesita incluir librerías UART/Timer

### Funciones usadas:

| Dirección | Función | Uso en el programa |
|-----------|---------|-------------------|
| `$BF18` | `uart_putc(char)` | Enviar caracteres al terminal |
| `$BF2D` | `get_micros()` | Leer contador de microsegundos |
| `$BF30` | `delay_us(us)` | Delays en microsegundos |
| `$BF33` | `delay_ms(ms)` | Delays en milisegundos |

Para usar otras funciones de la ROM API, consultar [`include/romapi.h`](../../include/romapi.h) en la raíz del proyecto.

## Crear un Nuevo Programa

1. Copiar esta carpeta completa
2. Renombrar el proyecto
3. Editar `src/main.c` con tu código
4. Ajustar `PROGRAM_NAME` en el makefile si deseas otro nombre
5. Compilar con `make`

## Ejemplo: Controlar LEDs desde C

```c
#include <stdint.h>

#define LEDS (*(volatile uint8_t *)0xC001)

void mi_funcion(void) {
    /* Encender todos los LEDs */
    LEDS = 0x00;    /* Lógica negativa */
    
    /* Apagar todos */
    LEDS = 0xFF;
    
    /* Patrón personalizado (bits 0,2,4 encendidos) */
    LEDS = ~0x15;   /* 010101 invertido */
}
```

## Ejemplo: Llamar ROM API desde C

```c
/* Incluir el header de ROM API */
#include "../../../include/romapi.h"

/* Usar en tu código */
void mi_funcion(void) {
    rom_uart_putc('H');        /* Enviar caracter */
    rom_delay_ms(100);         /* Delay de 100ms */
    
    /* Otras funciones disponibles */
    rom_uart_puts("Hello\n");  /* Enviar string */
    uint32_t t = rom_get_micros();  /* Leer microsegundos */
    rom_delay_us(500);         /* Delay de 500 microsegundos */
}
```

**Nota**: El header `romapi.h` define todas las funciones ROM disponibles con sus prototipos correctos. Ver [`include/romapi.h`](../../include/romapi.h) para la lista completa.

## Diferencias con la Versión en Ensamblador

| Característica | Ensamblador | C |
|----------------|-------------|---|
| Tamaño | ~300 bytes | ~2KB |
| Velocidad | Más rápido | Aceptable |
| Facilidad | Más complejo | Más fácil |
| Mantenimiento | Difícil | Fácil |
| Funciones C | No | Sí (strings, math, etc.) |

**Recomendación**: Usar C para programas complejos, ensamblador para código crítico o muy pequeño.

## Notas Importantes

- El código **inicia en $0800** (segmento STARTUP)
- Usar **Zero Page $20-$7F** para variables (no $02-$1F)
- Los LEDs usan **lógica negativa** (0=encendido)
- El stack de CC65 está en **$3E00-$3FFF** (512 bytes)
- `main()` debe retornar para volver al monitor

## Resolución de Problemas

**Error al compilar**: Verificar que CC65_HOME apunte a la instalación de cc65

**Programa no carga**: Verificar que la dirección sea $0800 en el comando LOAD

**LEDs no funcionan**: Recordar que usan lógica negativa (0=encendido)

**Crashes al ejecutar**: Verificar que no se use Zero Page $02-$1F (reservada por monitor)

## Ver También

- [examples/leds/](../leds/) - Versión en ensamblador (más compacta)
- [examples/sidplayer/](../sidplayer/) - Ejemplo avanzado en C usando ROM API
- [include/romapi.h](../../include/romapi.h) - Documentación completa de ROM API
- [README.md principal](../../README.md) - Documentación del monitor

## 💖 Apóyame

Si disfrutas de este proyecto, considera apoyarme:

[![Support me on Ko-fi](https://img.shields.io/badge/Ko--fi-Apóyame-FF5E5B?logo=kofi&logoColor=white&style=for-the-badge)](https://ko-fi.com/nelsonfigueroa2k)

## Licencia

Este proyecto está licenciado bajo la **GNU General Public License v3.0**.
