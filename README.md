# Cerradura Electrónica con ESP32

Sistema de control de acceso electrónico con teclado matricial 4x3, pantalla LCD I2C, LEDs, buzzer pasivo y micro servo. Permite abrir y cerrar una cerradura simulada mediante contraseña numérica.

## Características
- 🔐 Contraseña configurable (por defecto: 74275)
- ⌨️ Teclado matricial 4x3 para ingreso de código
- 🖥️ Pantalla LCD 16x2 con interfaz I2C (solo 4 cables)
- 💡 Feedback visual con LEDs (verde = acceso correcto, rojo = error)
- 🔊 Feedback sonoro con buzzer pasivo (diferentes tonos según acción)
- 🚪 Simulación de cerradura con micro servo SG90
- ⏱️ Cierre automático de puerta después de 3 segundos
- 🔒 Asteriscos en pantalla por seguridad al ingresar contraseña

## Componentes utilizados
- ESP32
- Teclado matricial 4x3
- Pantalla LCD 16x2 con módulo I2C
- LEDs (Rojo y Verde)
- Resistencias 220Ω
- Buzzer pasivo (2 pines)
- Micro Servo SG90
- Protoboard y cables Dupont

## Cómo usarlo
1. Conectar los componentes según el diagrama
2. Cargar el código al ESP32
3. Ingresar la contraseña "74275" en el keypad
4. Presionar "#" para validar
5. Presionar "*" para borrar el último dígito
```
