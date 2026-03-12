# Proyecto: Manejo de Interrupciones con Botón en ESP32

## Descripción
Este proyecto implementa un sistema de conteo de pulsaciones de un botón utilizando interrupciones en un microcontrolador compatible con PlatformIO (como ESP32). Cada vez que se presiona el botón, se incrementa un contador y se muestra por el puerto serie. Después de 1 minuto, la interrupción se desactiva automáticamente.

## Código Original vs Modificaciones

### Código Original
El código base presentaba los siguientes problemas:
- Las variables compartidas entre la ISR y el loop principal no estaban declaradas como `volatile`
- El formato de impresión usaba `printf` que puede no ser compatible en todos los entornos Arduino
- No había mensaje de inicio para confirmar que el programa estaba funcionando

### Modificaciones Realizadas

| Aspecto | Código Original | Mi Código Modificado |
|---------|-----------------|----------------------|
| **Declaración de variables** | `uint32_t numberKeyPresses;`<br>`bool pressed;` | `volatile uint32_t numberKeyPresses;`<br>`volatile bool pressed;` |
| **Formato de impresión** | `Serial.printf("Button 1...%u times\n", button1.numberKeyPresses);` | `Serial.print("Button 1...");`<br>`Serial.print(button1.numberKeyPresses);`<br>`Serial.println(" times");` |
| **Mensaje inicial** | No incluido | `Serial.println("Inicio de procesador");` |
| **Inclusión de librerías** | No explícita | `#include <Arduino.h>` |

### Correcciones Explicadas

1. **Uso de `volatile`**: En el código original, las variables `numberKeyPresses` y `pressed` no estaban declaradas como `volatile`. Esto es crítico porque:
   - La ISR (Interrupt Service Routine) modifica estas variables
   - El compilador podría optimizar el acceso a estas variables si no se declaran como `volatile`
   - Sin `volatile`, el compilador podría almacenar estas variables en registros y no detectar los cambios realizados por la interrupción

2. **Formato de impresión**: Reemplacé `Serial.printf()` por múltiples llamadas a `Serial.print()` porque:
   - `printf()` no está disponible en todas las placas Arduino/ESP
   - Es más portable usar los métodos estándar de `Serial`

3. **Mensaje de inicio**: Agregué un mensaje de confirmación en el `setup()` para verificar que el programa se está ejecutando correctamente

4. **Librería Arduino**: Incluí explícitamente `<Arduino.h>` para mejor compatibilidad con PlatformIO

## Código Final y Explicación Línea por Línea

```cpp
#include <Arduino.h>
```
Incluye la librería principal de Arduino que proporciona las funciones básicas como `pinMode()`, `Serial.begin()`, etc.

```cpp
struct Button { 
  const uint8_t PIN; 
  volatile uint32_t numberKeyPresses; 
  volatile bool pressed; 
}; 
```
Define una estructura llamada `Button` que contiene:
- `PIN`: Número del pin (constante, no puede modificarse)
- `numberKeyPresses`: Contador de pulsaciones (volatile por ser modificado en ISR)
- `pressed`: Bandera que indica si se ha presionado el botón (volatile por ser modificado en ISR)

```cpp
Button button1 = {18, 0, false}; 
```
Crea una instancia de `Button` llamada `button1` con:
- Pin 18
- Contador inicializado a 0
- Bandera pressed inicializada a false

```cpp
void IRAM_ATTR isr() { 
  button1.numberKeyPresses += 1; 
  button1.pressed = true; 
} 
```
**ISR (Interrupt Service Routine)**:
- `IRAM_ATTR`: Coloca la función en RAM para ejecución más rápida
- Se ejecuta automáticamente cuando ocurre la interrupción
- Incrementa el contador de pulsaciones
- Establece la bandera `pressed` a true
- **Nota**: Debe ser lo más breve posible

```cpp
void setup() { 
  Serial.begin(115200); 
```
Inicia la comunicación serie a 115200 baudios

```cpp
  pinMode(button1.PIN, INPUT_PULLUP); 
```
Configura el pin del botón como entrada con resistencia pull-up interna

```cpp
  attachInterrupt(button1.PIN, isr, FALLING); 
```
Asocia la interrupción:
- Pin: button1.PIN (18)
- Función: isr (la ISR definida)
- Modo: FALLING (se activa cuando el pin pasa de HIGH a LOW)

```cpp
  Serial.println("Inicio de procesador");
} 
```
Mensaje de confirmación de inicio

```cpp
void loop() { 
  if (button1.pressed) { 
```
Verifica si se ha presionado el botón (si la bandera es true)

```cpp
      Serial.print("Button 1 has been pressed ");
      Serial.print(button1.numberKeyPresses);
      Serial.println(" times");
```
Muestra el mensaje con el número de pulsaciones

```cpp
      button1.pressed = false; 
```
Resetea la bandera para la siguiente pulsación

```cpp
  } 

  //Detach Interrupt after 1 Minute 
  static uint32_t lastMillis = 0; 
```
Variable estática que mantiene su valor entre llamadas al loop. Guarda el último momento en que se verificó el tiempo

```cpp
  if (millis() - lastMillis > 60000) { 
```
Comprueba si ha pasado 1 minuto (60000 ms) desde la última verificación

```cpp
    lastMillis = millis(); 
    detachInterrupt(button1.PIN); 
     Serial.println("Interrupt Detached!"); 
  } 
}
```
- Actualiza el tiempo de referencia
- Desactiva la interrupción del botón
- Muestra mensaje indicando que la interrupción se ha desactivado

## Componentes Necesarios
- Microcontrolador compatible con PlatformIO (ESP32 recomendado)
- 1 pulsador
- 1 resistencia pull-up (si no se usa INPUT_PULLUP interno)
- Cables de conexión

## Conexiones
- Botón conectado al pin GPIO18 del ESP32
- El otro terminal del botón a GND

## Funcionamiento
1. Al iniciar, muestra "Inicio de procesador" por el puerto serie
2. Cada pulsación del botón incrementa el contador
3. El programa muestra el número total de pulsaciones después de cada pulsación
4. Después de 60 segundos, la interrupción se desactiva automáticamente
5. Muestra "Interrupt Detached!" cuando se desactiva la interrupción

## Notas Importantes
- Las variables compartidas con la ISR deben ser `volatile`
- La ISR debe ser lo más corta posible (solo incrementa contador y cambia bandera)
- El pin usado (18) puede cambiarse según disponibilidad
- La interrupción se dispara en flanco de bajada (FALLING).

