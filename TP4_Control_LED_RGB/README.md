# 📍 TP4: Control de Intensidad y Color de LED RGB (PWM + ADC)

## 📌 Descripción del Proyecto
Sistema de control de intensidad y color para un LED RGB de Ánodo Común. El LED se enciende y apaga periódicamente con un efecto de desvanecimiento (*fading*) a un ritmo controlado por la luz ambiente detectada mediante un sensor LDR, y con un color configurado por el usuario a través de comandos de la interfaz serie.

---

## 🛠️ Especificaciones de Hardware y Conexiones (Proteus)
1. **LED RGB (RGBLED-CA Ánodo Común):** Conectado a los terminales `PB5` (Rojo), `PB2` (Verde) y `PB1` (Azul) mediante resistencias de limitación de $220\Omega$.
2. **Sensor de Luz LDR:** Conectado al canal `ADC3` (`PC3`) formando un divisor de tensión con una resistencia de $100k\Omega$ conectada a GND.
3. **Interfaz Serie USART0:** Terminal virtual conectada a `USART0` a **9600 bps (8N1)**.

---

## ⚙️ Generación de Señales y Lógica del Sistema
- **PWM de 8 bits (>30Hz):**  
  - Canal Verde (`PB2`): Generado con Timer1 (**OC1B**).
  - Canal Azul (`PB1`): Generado con Timer1 (**OC1A**).
  - Canal Rojo (`PB5`): Generado mediante **PWM por software**.
- **Selección de Color:** Se especifica la proporción RGB desde la terminal enviando el comando:  
  `SET_COLOR=R,G,B\r\n` (por ejemplo: `SET_COLOR=0,255,255\r\n`).
- **Control de Período vía ADC:**
  - Muestreo del canal `ADC3` (sensor LDR) para regular el período $T$ del parpadeo entre **3 segundos** (máxima iluminación) y **6 segundos** (mínima luz).

---

## 📄 Consigna Oficial
El enunciado completo y las especificaciones de evaluación de la cátedra se encuentran en [docs/tp4-2026_entregable.pdf](./docs/tp4-2026_entregable.pdf).

---

## 📂 Estructura del TP
```text
TP4_Control_LED_RGB/
├── docs/
│   └── tp04-2026_completo.pdf       # Consigna oficial de la cátedra y guía conceptual
├── MicrochipStudio/                # Proyecto y código fuente en C (AVR-GCC)
├── Proteus/                        # Esquema de simulación (.pdsprj v8.12)
└── README.md                       # Documentación principal del TP