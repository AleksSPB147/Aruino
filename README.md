/*
  Название проекта: [Кнопки и таймеры]
  Дата: [02.03.2026]
  Автор: [RAV]
  Описание: [Практика оформления и применения кнопок, вызов и реализация работы таймеров]
*/

// ----Подключение библиотек----
#include <EncButton.h>
#include <GTimer.h>


// ----Объявление констант----
// const тип_данных ИМЯ_КОНСТАНТЫ = значение;

const int Pusk_PIN = 5;    // Пин кнопки Пуск
const int Flap_PIN = 6;    // Пин заслонки
const int Reset_Pin = 7;   // Пин кнопки сброс
const int Motor_Pin = 12;  // Пин Мотора
const int Alarm_Pin = 11;  // Пин Аварии

// ----Объявление глобальных переменных----
uint16_t MOTOR_TIME_SEC = 10;
uint16_t PAUSE_TIME_SEC = 5;
uint16_t Flap_TIME_MS = 3000UL;

//----Объявление переменных в коде----





//-----Флаги----
bool flag_Motor = 0;


// ----Прототипы функций (если используете свои функции)----
// void myFunction();
void Motor_pusk();
void Pause_pusk();

// ----Создание объектов кнопок----
Button Pusk(Pusk_PIN);  // Создаем объект кнопки Пуск
Button Flap(Flap_PIN);  // Создаем объект Заслонки

// ----Создание объектов таймеров----
GTimerCb<millis> timer_Motor(MOTOR_TIME_SEC * 1000UL, Motor_pusk, GTMode::Timeout);  // Таймер работы мотора
GTimerCb<millis> timer_Pause(PAUSE_TIME_SEC * 1000UL, Pause_pusk, GTMode::Timeout);  // Таймер промежутка Паузы


// === ФУНКЦИЯ setup() ===
// Выполняется один раз при включении/перезагрузке платы
void setup() {
  Serial.begin(9600);
  pinMode(Motor_Pin, OUTPUT);
  pinMode(Alarm_Pin, OUTPUT);
  Flap.setHoldTimeout(Flap_TIME_MS);  // удержание через 3 секунду
  digitalWrite(Motor_Pin, 0);         // Изначально мотор выключен
  digitalWrite(Alarm_Pin, 0);         // Изначально мотор выключен

  // Явно останавливаем таймеры при старте
  timer_Motor.stop();  // Переводим таймер Мотора в стоп
  timer_Pause.stop();  // переводим таймер Паузы в стоп
}

// === ФУНКЦИЯ loop() ===
// Бесконечно повторяется, пока плата включена
void loop() {  // Основная логика программы:

  //----Обязательный опрос кнопок----
  Pusk.tick();  // Опрос кнопки Пуск
  Flap.tick();  // Опрос заслонки

  //----Проверка таймера (обязательно для работы таймера)----
  timer_Motor.tick();  // В каждом цикле loop() проверяем таймер
  timer_Pause.tick();  // В каждом цикле loop() проверяем таймер

  //----Обработка кнопки Пуск----
  if (Pusk.click()) {
    Serial.println("Кнопка Пуск нажата! Запускаем мотор на 10 секунд...");
    digitalWrite(Motor_Pin, 1);  // Включаем мотор
    timer_Motor.start();         // Запускаем таймер
  }
  //----Обработка Заслонка----
  if (Flap.hold()) {
    Serial.println("Заслонка сработала");
  }
}
// --- Определение пользовательских функций (если есть) ---
/*
void myFunction() {
  // Код вашей функции
}
*/
void Motor_pusk() {
  Serial.println("Таймер Мотор: мотор ВЫКЛЮЧЕН через 10 секунд!");
  digitalWrite(Motor_Pin, LOW);  // Выключаем мотор
};

void Pause_pusk() {
  Serial.println("Таймер Пауза сработал!");
};











// //----Обработка кнопки Reset----
// if (Reset.hold()) {
//   if (!timer_Pause.running()) {
//     Serial.println("Начато удержание кнопки Reset");
//     timer_Pause.start();
//   } else {
//     if (timer_Pause.running()) {
//       timer_Pause.stop();
//       Serial.println("Удержание прервано. Сброс отменён.");
//     }
//   }
// }
