# 📍 TP1: Sistema de Visualización Multitarea Cooperativo

## 📌 Descripción del Proyecto
Desarrollo del firmware para un sistema de visualización que gestiona tres salidas independientes (paralelo y serie) y dos entradas de usuario. El software aplica una arquitectura de **multitarea cooperativa** (*super loop* no bloqueante) donde ninguna tarea bloquea la ejecución de las demás. 

> **Nota:** En este ejercicio no está permitido el uso de interriupciones ni de periféricos de temporización (timers).

---

## 🛠️ Especificaciones de Hardware y Conexiones (Proteus)
1. **LEDs Puerto D:** 8 diodos LED conectados al puerto D (`PORTD`) en modo analógico con resistencias serie calculadas para limitar la corriente a 10mA según la caída $V_{LED}$.
2. **Pulsadores:** Dos botones conectados a los terminales `PC0` y `PC1`.
3. **LEDs Neopixel (WS2812B):** Tira de 8 LEDs Neopixel conectada a la salida `PB0`.

---

## ⚙️ Comportamiento del Firmware (AVR-GCC)

### 1. Control del Puerto D (100ms de retardo no bloqueante)
- **Secuencia a:** Enciende un LED a la vez desde el LSB hacia el MSB de forma repetitiva.
- **Secuencia b:** Enciende un LED a la vez desde el MSB y rebota en los extremos.
- **Control:** El programa inicia en la secuencia (a) y conmuta a la (b) (y viceversa) cada vez que se presiona el pulsador en `PC0`.

### 2. Control de LEDs Neopixel (150ms de retardo no bloqueante)
- **Secuencia c:** Enciende LEDs pares en rojo y luego LEDs impares en azul alternadamente.
- **Secuencia d:** Enciende en verde un LED a la vez de derecha a izquierda repetitivamente.
- **Control:** El programa conmuta entre las secuencias (c) y (d) cada vez que se presiona el pulsador en `PC1`.

---

## 📐 Memoria de Cálculo
- **Cálculo de Resistencias Limitadoras:** La deducción matemática para la limitación de corriente a 10mA según la caída de tensión $V_{LED}$ de cada color está detallada en el documento [Cálculo de resistencia serie.pdf](./docs/Cálculo de resistencia serie.pdf).

---

## 📄 Consigna Oficial
El enunciado completo y las especificaciones de evaluación de la cátedra se encuentran en [docs/tp1-2026_entregable.pdf](./docs/tp1-2026_entregable.pdf).

---

## 📂 Estructura del TP
```text
TP1_Visualizador_Multitarea/
├── docs/
│   ├── tp1-2026_entregable.pdf         # Consigna oficial de la cátedra
│   └── calculo_resistencias.pdf        # Memoria de cálculo de resistencias para LEDs
├── MicrochipStudio/                    # Proyecto y código fuente en C (AVR-GCC)
├── Proteus/                            # Esquema de simulación (.pdsprj v8.12)
└── README.md                           # Documentación principal del TP
