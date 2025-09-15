#include <Mouse.h>

const int pedalPin = 2;  // Pin donde conectas el final de carrera

// Variables para debounce
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 30; // ms para filtrar rebote
bool stableState = HIGH; // Estado estable anterior
bool lastState = HIGH;   // Lectura anterior

// Variables para lógica de clics
unsigned long pressStartTime = 0;
unsigned long lastClickTime = 0;
int clickCounter = 0;

const unsigned long clickWindow = 400;   // tiempo máx. entre clics (ms)
const unsigned long holdThreshold = 800; // ms para arrastre
bool dragging = false;

void setup() {
  pinMode(pedalPin, INPUT_PULLUP); 
  Mouse.begin();
  //Serial.begin(9600); // Descomenta si quieres debug por Serial
}

void loop() {
  bool reading = digitalRead(pedalPin);
  unsigned long now = millis();

  // --- Debounce ---
  if (reading != lastState) {
    lastDebounceTime = now;
  }

  if ((now - lastDebounceTime) > debounceDelay) {
    // Si el estado cambió de verdad (luego de debounce)
    if (reading != stableState) {
      stableState = reading;

      // --- Inicio de pulsación ---
      if (stableState == LOW) {
        pressStartTime = now;
      }

      // --- Liberación de pulsación ---
      if (stableState == HIGH) {
        if (dragging) {
          Mouse.release(MOUSE_LEFT); // terminar arrastre
          dragging = false;
        } else {
          clickCounter++;
          lastClickTime = now;
        }
      }
    }
  }

  // --- Arrastre (si se mantiene presionado) ---
  if (stableState == LOW && !dragging && (now - pressStartTime > holdThreshold)) {
    Mouse.press(MOUSE_LEFT);
    dragging = true;
    clickCounter = 0; // cancelar secuencia de clics
  }

  // --- Procesar clics cortos ---
  if (clickCounter > 0 && (now - lastClickTime > clickWindow)) {
    if (clickCounter == 1) {
      Mouse.click(MOUSE_LEFT);        // clic izquierdo
    } else if (clickCounter == 2) {
      Mouse.click(MOUSE_LEFT);        // doble clic
      delay(50);
      Mouse.click(MOUSE_LEFT);
    } else if (clickCounter == 3) {
      Mouse.click(MOUSE_RIGHT);       // clic derecho
    }
    clickCounter = 0;
  }

  lastState = reading;
}
