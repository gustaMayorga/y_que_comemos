#include "inputs.h"

// ------------------------------ ProximityCounter ---------------------------

void ProximityCounter::begin(uint8_t trigPin, uint8_t echoPin) {
  trig_ = trigPin; echo_ = echoPin;
  pinMode(trig_, OUTPUT);
  pinMode(echo_, INPUT);
  digitalWrite(trig_, LOW);
  st_ = ARMED;
}

uint16_t ProximityCounter::measureCm() {
  digitalWrite(trig_, LOW);  delayMicroseconds(3);
  digitalWrite(trig_, HIGH); delayMicroseconds(10);
  digitalWrite(trig_, LOW);
  // timeout ~ (PROX_MAX_CM * 2) / 0.0343 us  -> margen x1.3
  const unsigned long timeoutUs = (unsigned long)((PROX_MAX_CM * 2 / 0.0343f) * 1.3f);
  const unsigned long dur = pulseIn(echo_, HIGH, timeoutUs);
  if (dur == 0) return PROX_MAX_CM;                 // sin eco = nada cerca
  const uint16_t cm = (uint16_t)(dur * 0.01715f);   // dur/2 * 0.0343
  return cm > PROX_MAX_CM ? PROX_MAX_CM : cm;
}

bool ProximityCounter::poll() {
  const uint32_t now = millis();

  if (st_ == LOCKOUT) {
    if (now - tLock_ < PROX_LOCKOUT_MS) return false;
    st_ = WAIT_CLEAR;
  }
  if (now - tPoll_ < PROX_POLL_MS) return false;
  tPoll_ = now;

  lastCm_ = measureCm();

  switch (st_) {
    case ARMED:
      if (lastCm_ <= PROX_TRIGGER_CM) { st_ = HOLDING; tHold_ = now; }
      break;

    case HOLDING:
      if (lastCm_ > PROX_TRIGGER_CM) {          // se retiro antes de tiempo
        st_ = ARMED;
      } else if (now - tHold_ >= PROX_HOLD_MS) { // gesto valido
        st_ = LOCKOUT; tLock_ = now;
        return true;
      }
      break;

    case WAIT_CLEAR:
      if (lastCm_ >= PROX_REARM_CM) st_ = ARMED;
      break;

    default: break;
  }
  return false;
}

// ---------------------------------- Button ---------------------------------

void Button::begin(uint8_t pin, bool activeHigh, uint16_t longMs) {
  pin_ = pin; activeHigh_ = activeHigh; longMs_ = longMs;
  pinMode(pin_, activeHigh ? INPUT : INPUT_PULLUP);
}

void Button::poll() {
  const uint32_t now = millis();
  const bool raw = activeHigh_ ? (digitalRead(pin_) == HIGH) : (digitalRead(pin_) == LOW);

  if (raw != lastRaw_) { lastRaw_ = raw; tChange_ = now; return; }
  if (now - tChange_ < 30) return;                 // antirebote 30 ms

  if (raw && !stable_) {                            // flanco de bajada->presionado
    stable_ = true; tDown_ = now; longFired_ = false;
  } else if (!raw && stable_) {                     // soltado
    stable_ = false;
    if (!longFired_) shortEvt_ = true;
  } else if (raw && stable_ && !longFired_ && (now - tDown_ >= longMs_)) {
    longFired_ = true; longEvt_ = true;             // dispara sin esperar el release
  }
}

uint32_t Button::heldMs() const { return stable_ ? (millis() - tDown_) : 0; }

bool Button::pressedShort() { const bool e = shortEvt_; shortEvt_ = false; return e; }
bool Button::pressedLong()  { const bool e = longEvt_;  longEvt_  = false; return e; }
