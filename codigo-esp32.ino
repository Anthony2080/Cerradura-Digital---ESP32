/* ==========================================================
   PROYECTO: Cerradura Electrónica con ESP32
   DESCRIPCIÓN: Sistema de control de acceso con teclado matricial 4x3,
                pantalla LCD, LEDs, buzzer pasivo y servo.
   AUTOR: Antonio Pirotta
   FECHA: 25/6/26
   ========================================================== */

// ---------- LIBRERÍAS ----------
#include <Keypad.h>              // Para el teclado matricial
#include <LiquidCrystal_I2C.h>   // Para la pantalla LCD con I2C
#include <ESP32Servo.h>          // Para controlar el servo motor en ESP32

// ---------- DEFINICIÓN DE CONSTANTES ----------
#define CONTRASENA "74275"        // Contraseña correcta (74275 = shark)
#define TIEMPO_ABIERTA 3000      // Tiempo que la puerta queda abierta (3 segundos)

// ---------- PINES DEL TECLADO MATRICIAL 4x3 ----------
const byte FILAS = 4;
const byte COLUMNAS = 3;

// Distribución de teclas en la matriz 4x3
char teclas[FILAS][COLUMNAS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

// Pines GPIO para las filas (conectados al ESP32)
byte pinesFilas[FILAS] = {13, 12, 14, 27};
// Pines GPIO para las columnas (conectados al ESP32)
byte pinesColumnas[COLUMNAS] = {26, 25, 33};

// ---------- PINES DE COMPONENTES ----------
#define PIN_LED_ROJO   4
#define PIN_LED_VERDE  5
#define PIN_BUZZER     16   // Buzzer pasivo - necesita señal PWM
#define PIN_SERVO      23

// ---------- INICIALIZACIÓN DE OBJETOS ----------
Keypad keypad = Keypad(makeKeymap(teclas), pinesFilas, pinesColumnas, FILAS, COLUMNAS);
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Dirección I2C 0x27, 16 caracteres x 2 líneas
Servo servo;  // Objeto para el servo (ahora usa ESP32Servo)

// ---------- VARIABLES GLOBALES ----------
String passwordIngresada = "";   // Contraseña que ingresa el usuario
bool puertaAbierta = false;      // Estado de la puerta
unsigned long tiempoApertura = 0; // Momento en que se abrió la puerta

// ---------- FUNCIÓN SETUP (Configuración) ----------
void setup() {
  // Configurar pines de LEDs como salida
  pinMode(PIN_LED_ROJO, OUTPUT);
  pinMode(PIN_LED_VERDE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);   // El buzzer pasivo también usa OUTPUT
  
  // Inicializar pantalla LCD
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("CERRADURA ELECT");
  lcd.setCursor(0, 1);
  lcd.print("INGRESE CODIGO:");
  
  // Inicializar servo (posición 0° = puerta cerrada)
  servo.attach(PIN_SERVO);
  servo.write(0);
  
  // Apagar LEDs al inicio
  digitalWrite(PIN_LED_ROJO, LOW);
  digitalWrite(PIN_LED_VERDE, LOW);
  
  delay(1000);  // Pequeña pausa para estabilizar
}

// ---------- FUNCIÓN LOOP (Bucle Principal) ----------
void loop() {
  // 1. Verificar si hay que cerrar la puerta automáticamente
  if (puertaAbierta) {
    if (millis() - tiempoApertura >= TIEMPO_ABIERTA) {
      cerrarPuerta();
    }
    return;  // Si está abierta, no procesamos teclas
  }
  
  // 2. Leer tecla presionada
  char tecla = keypad.getKey();
  if (!tecla) {
    return;  // Si no hay tecla, salimos
  }
  
  // 3. Procesar la tecla
  switch (tecla) {
    // Caso: Números (0-9)
    case '0': case '1': case '2': case '3':
    case '4': case '5': case '6': case '7':
    case '8': case '9':
      if (passwordIngresada.length() < 8) {  // Máximo 8 dígitos
        passwordIngresada += tecla;
        actualizarPantalla();
        tonoBuzzer(1000, 50);  // Tono corto al presionar tecla
      }
      break;
    
    // Caso: Tecla "*" (borrar último dígito)
    case '*':
      if (passwordIngresada.length() > 0) {
        passwordIngresada.remove(passwordIngresada.length() - 1);
        actualizarPantalla();
        tonoBuzzer(800, 30);   // Tono diferente para retroceso
      }
      break;
    
    // Caso: Tecla "#" (validar contraseña)
    case '#':
      verificarPassword();
      break;
  }
}

// ---------- FUNCIONES PERSONALIZADAS ----------

/*
 * Función: actualizarPantalla()
 * Descripción: Muestra la contraseña con asteriscos por seguridad
 */
void actualizarPantalla() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("INGRESE CODIGO:");
  lcd.setCursor(0, 1);
  
  // Mostrar asteriscos en lugar de los números
  for (int i = 0; i < passwordIngresada.length(); i++) {
    lcd.print("*");
  }
}

/*
 * Función: verificarPassword()
 * Descripción: Compara la contraseña ingresada con la correcta
 */
void verificarPassword() {
  if (passwordIngresada == CONTRASENA) {
    abrirPuerta();        // Contraseña correcta
  } else {
    accesoDenegado();     // Contraseña incorrecta
  }
}

/*
 * Función: abrirPuerta()
 * Descripción: Acciones para abrir la puerta
 *              Incluye melodía de confirmación con el buzzer pasivo
 */
void abrirPuerta() {
  // LED verde
  digitalWrite(PIN_LED_VERDE, HIGH);
  digitalWrite(PIN_LED_ROJO, LOW);
  
  // Melodía de confirmación con buzzer pasivo (dos tonos distintos)
  tonoBuzzer(2000, 150);   // Frecuencia aguda 2000Hz
  delay(150);
  tonoBuzzer(2500, 150);   // Frecuencia más aguda 2500Hz
  
  // Servo a 90° (simula apertura)
  servo.write(90);
  
  // Mensaje en LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESO CONCEDIDO");
  lcd.setCursor(0, 1);
  lcd.print("BIENVENIDO!");
  
  // Activar timer para cierre automático
  puertaAbierta = true;
  tiempoApertura = millis();
  passwordIngresada = "";
}

/*
 * Función: cerrarPuerta()
 * Descripción: Acciones para cerrar la puerta
 */
void cerrarPuerta() {
  servo.write(0);                    // Cerrar servo
  digitalWrite(PIN_LED_VERDE, LOW);  // Apagar LED verde
  digitalWrite(PIN_LED_ROJO, LOW);   // Apagar LED rojo
  puertaAbierta = false;             // Cambiar estado
  
  // Volver a pantalla inicial
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CERRADURA ELECT");
  lcd.setCursor(0, 1);
  lcd.print("INGRESE CODIGO:");
}

/*
 * Función: accesoDenegado()
 * Descripción: Acciones cuando la contraseña es incorrecta
 *              Incluye tono de error con el buzzer pasivo (frecuencia grave)
 */
void accesoDenegado() {
  // LED rojo
  digitalWrite(PIN_LED_ROJO, HIGH);
  digitalWrite(PIN_LED_VERDE, LOW);
  
  // Tono de error con buzzer pasivo (3 pitidos graves y cortos)
  for (int i = 0; i < 3; i++) {
    tonoBuzzer(400, 200);   // Frecuencia grave 400Hz
    delay(200);
  }
  
  // Mensaje de error
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("ACCESO DENEGADO");
  lcd.setCursor(0, 1);
  lcd.print("INTENTE DE NUEVO");
  delay(1500);
  
  // Limpiar y volver a inicio
  passwordIngresada = "";
  digitalWrite(PIN_LED_ROJO, LOW);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("CERRADURA ELECT");
  lcd.setCursor(0, 1);
  lcd.print("INGRESE CODIGO:");
}

/*
 * Función: tonoBuzzer()
 * Parámetros:
 *   - frecuencia: Hz del tono (ej: 400Hz grave, 2000Hz agudo)
 *   - duracion: milisegundos que dura el tono
 * Descripción: Genera un tono en el buzzer pasivo usando la función tone()
 *              Al ser pasivo, necesita una señal PWM para generar diferentes frecuencias.
 *              Un buzzer activo no podría hacer esto ya que solo tiene un tono fijo.
 */
void tonoBuzzer(int frecuencia, int duracion) {
  tone(PIN_BUZZER, frecuencia);   // Genera la señal PWM a la frecuencia deseada
  delay(duracion);
  noTone(PIN_BUZZER);             // Apaga el buzzer
}