/*
Название проекта: [Кнопки и таймеры]
Дата: [02.03.2026]
Автор: [RAV]
Описание: [Практика оформления и применения кнопок, вызов и реализация работы таймеров]
*/

// --- Подключение библиотек ---
#include <EncButton.h>
#include <GTimer.h>


// --- Объявление констант ---
// const тип_данных ИМЯ_КОНСТАНТЫ = значение;

const int Pusk_PIN = 5;    // Пин кнопки Пуск
const int Flap_PIN = 6;    // Пин заслонки
const int Reset_Pin = 7;   // Пин кнопки сброс
const int Motor_Pin = 12;  // Пин Мотора
const int Alarm_Pin = 11;  // Пин Аварии

// --- Объявление глобальных переменных ---
uint16_t MOTOR_TIME_SEC = 10;
uint16_t PAUSE_TIME_SEC = 5;
uint16_t Flap_TIME_MS = 3000UL;

//----Объявление переменных в коде----
//----переменные мигания----
uint16_t Thermal_alarm = 500;  // Период мигания при Thermal_alarm (мс)
uint16_t Flap_alarm = 1000;    // Период мигания при Flap_alarm (мс)
uint8_t step = 0;              // Номер шага
bool motorTimerDone = false;   // Флаг: таймер мотора сработал


//-----Флаги----
bool flag_Motor = 0;
bool flag_alarm = false;  // Флаг для запуска мигания

// ---- Прототипы функций (если используете свои функции) ----
// void myFunction();
void Motor_pusk();
void Pause_pusk();
void alarmBlink();
// --- Создание объектов кнопок ---
Button Pusk(Pusk_PIN);  // Создаем объект кнопки Пуск
Button Flap(Flap_PIN);  // Создаем объект Заслонки

// ---- Создание объектов таймеров ----
GTimerCb<millis> timer_Motor(MOTOR_TIME_SEC * 1000UL, Motor_pusk, GTMode::Timeout);  // Таймер работы мотора
GTimerCb<millis> timer_Pause(PAUSE_TIME_SEC * 1000UL, Pause_pusk, GTMode::Timeout);  // Таймер промежутка Паузы
GTimerCb<millis> timer_Alarm(Thermal_alarm, alarmBlink, GTMode::Interval);           // Таймер для мигания

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
  timer_Alarm.stop();
}

// === ФУНКЦИЯ loop() ===
// Бесконечно повторяется, пока плата включена
void loop() {  // Основная логика программы:
  //---- Обязательный опрос ----
  Pusk.tick();         // Опрос кнопки Пуск
  Flap.tick();         // Опрос заслонки
  timer_Motor.tick();  // Проверка таймера Мотора
  timer_Alarm.tick();  // Проверка таймера мигания

  switch (step) {
    case 0:
      if (Pusk.click()) {
        Serial.println("Кнопка Пуск нажата! Запускаем мотор на 5 секунд...");
        digitalWrite(Motor_Pin, 1);  // Включаем мотор
        timer_Motor.start();         // Запускаем таймер
        step = 1;
      }
      break;
    case 1:
      if (motorTimerDone) {
        step = 10;
        motorTimerDone = false;
        flag_alarm = true;
      }

      
      break;
    case 2:
      // Логика шага 2
      break;
    case 3:
      // Логика шага 3
      break;
    case 4:
      // Логика шага 4
      break;
    case 10:  // Шаг аварии
              // Логика мигания в зависимости от флагов и состояний
      if (flag_alarm) {
        if (Thermal_alarm) {
          timer_Alarm.setTime(Thermal_alarm);  // Устанавливаем период мигания для Thermal_alarm
        } else {
          timer_Alarm.setTime(Flap_alarm);  // Устанавливаем период мигания для Flap_alarm
        }
        if (!timer_Alarm.running()) {  // Если таймер не запущен — запускаем
          timer_Alarm.start();
        }
      } else {
        timer_Alarm.stop();          // Останавливаем мигание, если флаг сброшен
        digitalWrite(Alarm_Pin, 0);  // Гасим пин
      }
      break;
  }

  //----Обработка кнопки Пуск----

  //----Обработка кнопки Reset----
  if (Flap.hold()) {
    Serial.println("Заслонка сработала");
    flag_alarm = true;  // Включаем сигнализацию при удержании заслонки
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
  motorTimerDone = true;         // Устанавливаем флаг завершения таймера
};

void Pause_pusk() {
  Serial.println("Таймер Пауза сработал!");
};
void alarmBlink() {
  digitalWrite(Alarm_Pin, !digitalRead(Alarm_Pin));  // Инвертируем состояние пина
};