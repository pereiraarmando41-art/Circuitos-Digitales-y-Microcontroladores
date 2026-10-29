# 🎛️ Circuitos Digitales y Microcontroladores (E305) — ATmega328P

![Microchip Studio](https://img.shields.io/badge/IDE-Microchip%20Studio-blue)
![Proteus](https://img.shields.io/badge/Simulation-Proteus%208.12-orange)
![MCU](https://img.shields.io/badge/MCU-ATmega328P-green)
![Academic](https://img.shields.io/badge/UNLP-Facultad%20de%20Ingeniería-red)

Este repositorio contiene las soluciones para los 4 Trabajos Prácticos evaluables de la asignatura **Circuitos Digitales y Microcontroladores (E305)** del Departamento de Electrotecnia de la Facultad de Ingeniería (Universidad Nacional de La Plata).

Todos los desarrollos están programados *bare-metal* en C con el compilador **AVR-GCC** en **Microchip Studio** y simulados en **Proteus VSM (v8.12)**.

---

## 📂 Contenido del Repositorio

### 📍 [TP1: Sistema de Visualización Multitarea Cooperativo](./TP1_Visualizador_Multitarea)
- **Objetivo:** Desarrollo del firmware para un sistema de visualización con tres salidas independientes y dos entradas de usuario mediante un *super loop* no bloqueante (sin interrupciones ni timers).
- **Periféricos y Hardware:**
  - **8 LEDs analógicos** en el Puerto D (`PORTD`) con secuencias de desplazamiento y rebote (100ms) cambiables vía pulsador en `PC0`.
  - **8 LEDs Neopixel (WS2812B)** en `PB0` gestionando patrones intercalados y de barrido (150ms) cambiables vía pulsador en `PC1`.
  - Limitación de corriente ajustada a 10mA según tensión $V_{LED}$ por color.

### 📍 [TP2: Controlador de Horno Microondas Doméstico](./TP2_Controlador_Microondas)
- **Objetivo:** Firmware del controlador de un microondas estructurado como una Máquina de Estados Finitos (MEF) periódica gestionada mediante interrupción de Timer.
- **Periféricos y Hardware:**
  - **LCD 16x2** y **Teclado Matricial 4x4** (Teclas `0-9` para ingreso MM:SS, `A`=START, `B`=STOP/CLEAR, `C`=+30s, `D`=PUERTA).
  - **Actuadores LED:** Magnetrón (`PB5`), Luz interior (`PC4`) y Alarma de fin de proceso (`PC5`).

### 📍 [TP3: Monitor de Invernadero (Background/Foreground)](./TP3_Monitor_Invernadero)
- **Objetivo:** Implementación de un monitor ambiental con arquitectura *background/foreground* que verifica ventanas horarias de temperatura y humedad en rangos diurnos/nocturnos.
- **Periféricos y Hardware:**
  - RTC **DS3231** vía **I2C/TWI** (registros de hora/fecha con *pull-ups* de 4.7kΩ).
  - Sensor **DHT11** conectado a `PC0` (*pull-up* de 10kΩ).
  - Telemetría e interfaz de comandos mediante **USART0** a 9600bps (8N1) gestionada por interrupciones y temporización con Timer.

### 📍 [TP4: Control de Intensidad y Color de LED RGB (PWM + ADC)](./TP4_Control_LED_RGB)
- **Objetivo:** Control periódico de encendido, apagado y desvanecimiento (*fading*) de un LED RGB de Ánodo Común regulando el período según la luz ambiente medida.
- **Periféricos y Hardware:**
  - **LED RGB (RGBLED-CA):** Generación de 3 señales PWM (>30Hz, 8 bits) en `PB1` (Timer1 OC1A), `PB2` (Timer1 OC1B) y `PB5` (PWM por software).
  - **Sensor LDR:** Conectado al canal `ADC3` (`PC3`) en divisor de tensión con 100kΩ a GND para controlar el período $T$ entre 3s (máx. luz) y 6s (mín. luz).
  - **USART0:** Configuración del color resultante enviando la proporción RGB vía comandos de terminal (ej. `SET_COLOR=0,255,255\r\n`).

---

## 🛠️ Requisitos e Instalación

1. **Microchip Studio** (versión 7.0 o superior con AVR-GCC).
2. **Proteus VSM** (versión 8.12).

### 🚀 Instrucciones de Ejecución
1. Abrir el proyecto dentro de la carpeta `MicrochipStudio/` correspondiente al trabajo práctico deseado.
2. Compilar la solución (`F7` o *Build Solution*).
3. Abrir el archivo `.pdsprj` localizado en la carpeta `Proteus/` del mismo TP.
4. En Proteus, hacer doble clic en el microcontrolador **ATmega328P**, cargar el ejecutable `.hex` o `.elf` generado en la carpeta `Debug/` y presionar **Play**.

---

## 👥 Autores
- **Armando Antonio Pereira**
