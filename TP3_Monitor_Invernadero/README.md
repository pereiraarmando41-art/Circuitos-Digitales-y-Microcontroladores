# 📍 TP3: Monitor Ambientador de Invernadero (Background/Foreground)

## 📌 Descripción del Proyecto
Implementación de un monitor de temperatura y humedad para un invernadero que controla que las variables ambientales se mantengan en los rangos óptimos según la hora del día (diurno/nocturno). 

Utiliza una arquitectura del tipo **Background/Foreground** para la gestión de interrupciones del sistema.

---

## 🛠️ Especificaciones de Hardware y Conexiones (Proteus)
1. **Módulo RTC DS3231:** Conectado por interfaz **I2C (TWI)** con resistencias de *pull-up* de $4.7k\Omega$.
2. **Sensor DHT11:** Conectado al terminal `PC0` del ATmega328P con resistencia de *pull-up* de $10k\Omega$.
3. **Comunicación Serie (USART0):** Terminal virtual conectada al periférico `USART0` configurado a **9600 bps (8N1)** gestionada por interrupciones de recepción y transmisión.

---

## 📊 Ventanas Horarias y Funcionalidades del Firmware

| Ventana Horaria | Temp. Óptima | Humedad Óptima |
| :--- | :---: | :---: |
| **Día (07:00 - 18:59)** | $20^\circ C$ a $30^\circ C$ | $50\%$ a $70\%$ |
| **Noche (19:00 - 06:59)** | $15^\circ C$ a $22^\circ C$ | $60\%$ a $80\%$ |

### Funciones de Telemetría y Comandos USART
- **Reporte periódico:** Cada $T$ segundos transmite una cadena formateada:  
  `[HH:MM:SS] T: XX°C | H: XX% | Estado: NORMAL/ALERTA`
- **Comandos de entrada por terminal:**
  - `SET TIME=HH:MM:SS`: Actualiza la hora del RTC.
  - `SET TM=SS`: Ajusta la tasa de muestreo $T$ entre 2 y 60 segundos.
- **Alertas de emergencia:** Si una variable está fuera de rango, transmite un mensaje de alerta cada dos tramas de telemetría.

---

## 📄 Consigna Oficial
El enunciado completo y las especificaciones de evaluación de la cátedra se encuentran en [docs/tp3-2026_entregable.pdf](./docs/tp3-2026_entregable.pdf).

---

## 📂 Estructura del TP
```text
TP3_Monitor_Invernadero/
├── docs/
│   └── tp03-2026-entregable.pdf    # Consigna oficial de la cátedra
├── MicrochipStudio/                # Proyecto y código fuente en C (AVR-GCC)
├── Proteus/                        # Esquema de simulación (.pdsprj v8.12)
└── README.md                       # Documentación principal del TP