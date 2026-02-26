#include <GTimer.h>
#include <EncButton.h>

// Константы — номера пинов
const int LED_PIN = 12;    // Пин светодиода
const int BUTTON_PIN = 2;  // Пин кнопки

// Объекты
GTimer<millis> myTimer;      // Таймер для замера времени удержания (в текущей логике не используется)
Button FLAP_PIN(BUTTON_PIN);     // Создаём объект кнопки для работы с пином 2

// Переменные состояния
bool timerActivated = false;    // Флаг активации таймера: false — не активирован, true — активирован
unsigned long holdTime = 3000;  // Время удержания в мс (3 секунды)

void setup() {
  Serial.begin(9600);  // Инициализация последовательного порта (скорость 9600 бод)
  pinMode(LED_PIN, OUTPUT);    // Настраиваем пин светодиода как выход
  digitalWrite(LED_PIN, LOW);  // Гарантированно выключаем светодиод при старте

  // Настройки кнопки
  FLAP_PIN.setHoldTimeout(holdTime); // Устанавливаем таймаут удержания: 3 с
}

void loop() {
  FLAP_PIN.tick();  // Опрашиваем состояние кнопки — библиотека обрабатывает дребезг и определяет события

  // Если кнопка удерживается 3 секунды и таймер ещё не активирован
  if (FLAP_PIN.hold() && !timerActivated) {
    // Включаем светодиод и отмечаем активацию
    timerActivated = true;
    digitalWrite(LED_PIN, HIGH);
    Serial.println("> УСПЕХ: Светодиод ВКЛЮЧЁН после 3 с удержания!");
  }

  // Если кнопка отпущена — сбрасываем флаг активации (чтобы можно было повторить)
  if (!FLAP_PIN.press()) {
    timerActivated = false;
  }

  
}