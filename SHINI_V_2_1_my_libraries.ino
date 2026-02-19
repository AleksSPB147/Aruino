//Библиотеки:
#include <EncButton.h>  // Объявляем библиотеку обработки кнопок
//#include <TimerMs.h>    // Обявляем библиотеку таймера
//#include <Timers.h>       // Моя библиотека таймеры TON, TOF, TP
#include "TP.h"
#include "TON.h"
#include "SEL.h"
#include <FR_TRIG.h>      // Моя библиотека F_TRIG и R_TRIG
#include <Trigger_R_S.h>  // Моя библиотека RS_Trigger и SR_Trigger

/*********************************************************************************************************/
//Константы для пинов и параметров:
#define WORK_TIME_MOTOR 2   // Время работы в минутах
#define RELAX_TIME_MOTOR 2  //Время паузы в минутах
#define WORK_TIME_FLAP 1    //Время работы заслонки
#define MOTOR_PIN 12        //Пин мотора
#define ALARM_PIN 11        //Пин Аварии

// Создание экземпляров классов


/***********************************************************************************************************/
// Переменные
unsigned long motorDelayMs = 5000;  // 3 секунды (можно менять динамически)
float delaySec = 2;
int8_t page = 0;           // Переменная switch
int8_t val_motor = 0;      // Подсчет количества срабатываний мотра
int8_t val_ust_motor = 2;  // Допустимое количество срабатываний без материала
int8_t val_flap = 0;       // Подсчет количества срабатываний заслонки
//*******флаги надписей*******
bool flag_motor = 0;  // Флаг состояния мотора
bool flag_pusk = 0;   // Флаг состояния
bool flag_alarm = 0;  // Флаг состояния

bool p_Pusk;  //
int step = 0;
bool motorStartPrinted = 0;
bool iResetErr;
bool iThermal_relay;
bool resetInput;
bool firstBoot = true;       // Флаг первой загрузки*
unsigned long initTime = 0;  // Переменная для таймера задержки загрузки

/**********************************************************************************************************/
// Инициализируем объекты:
//****КНОПКИ******
EncButton<EB_TICK, 4> PUSK_PIN(INPUT_PULLUP);       // Кнопка Пуск
EncButton<EB_TICK, 2> STOP_PIN(INPUT_PULLUP);       // Кнопка Стоп
EncButton<EB_TICK, 5> FLAP_PIN(INPUT_PULLUP);       // Заслонка
EncButton<EB_TICK, 6> ResetErr(INPUT_PULLUP);       // Кнопка сброс ошибки / Пуск после ошибки
EncButton<EB_TICK, 7> Thermal_relay(INPUT_PULLUP);  // Тепловое реле

//*****ТАЙМЕРЫ********
TP TimMotor(4000);

/****************************************************void setup()******************************************************/

void setup() {
  Serial.begin(9600);
  pinMode(MOTOR_PIN, OUTPUT);  // Выход мотора
  pinMode(ALARM_PIN_PIN, OUTPUT);  // Выход Аварии
}

/****************************************************void loop()****************************************************/
void loop() {
  TimMotor.run(true);
  PUSK_PIN.tick();       // Проверяем состояние кнопки Пуск.
  STOP_PIN.tick();       // Проверяем состояние кнопки Стоп
  FLAP_PIN.tick();       // Проверяемсостояние заслонки
  Thermal_relay.tick();  // Проверяем состояние Теплового реле.
  ResetErr.tick();       // Проверяем состояние кнопки Стоп
  //TimMotor.run(PUSK_PIN.click());  // Вызывать КАЖДЫЙ цикл, даже если кнопка не нажата
  //Этот код реализует одноразовый отложенный переход на шаг 0 при первом запуске программы//
  if (firstBoot) {                              // Если firstBoot == true (первый запуск после включения/перезагрузки) — выполняем блок.Если firstBoot == false — пропускаем весь блок (никаких действий)
    initTime = initTime ? initTime : millis();  //
    if (millis() - initTime >= 1500) step = 0, firstBoot = false;
    return;
  }
  //*****************************************************************************************//

  step = CODESYS_SEL(Thermal_relay.press(), step, 10);



  switch (step) {
    case 0:  //Стартовое меню
      Serial.println("Шаг 0 Стартовое меню ");
      step = CODESYS_SEL(!Thermal_relay.hold(), step, 1);
      flag_pusk = true;
      break;


    case 1:  // Стоп. Ждем команду на запуск
      //Serial.println("Шаг 1 Ждем команду Пуск ");
      if (flag_pusk) {  // Чтобы не повторялось
        Serial.println("Шаг 1 Ждем команду Пуск ");
        flag_pusk = false;
      }
      if (PUSK_PIN.press()) {  // Нажатие кнопки что кратковременно что длительное приводит к срабатыванию
        Serial.println("Шаг 1  Пуск нажали ");
        TimMotor.reset();    // Сброс таймера перед запуском
        TimMotor.run(true);  // Запускаем таймер именно в этом цикле
        flag_motor = true;   // Устанавливаем флаг для вывода сообщения
        step = 2;            // Переход в шаг 2
      }
      break;


    case 2:  // Работает мотор
      //("Шаг 2 Ждем подтверждение запуска");

      if (flag_motor && TimMotor.Q()) {  // Чтобы не повторялось
        Serial.println("Мотор работает");
        flag_motor = false;  
      }
      // Если таймер выключился, переходим обратно в ожидание
      if (TimMotor.Q_Ftrig()) {
        Serial.println("Мотор выключился");
        step = 3;
      }

      digitalWrite(MOTOR_PIN, TimMotor.Q());
      break;
    case 3:  // Допоплнительная логика
      Serial.println("Ждем дальнейшей логики");
      step = 0;
      flag_alarm = true;  // Флаг состояния

      break;


    case 4:  // Допоплнительная логика
      Serial.println("Мотор выключился");
      //step = 1;
      break;

    case 10:  // Шаг аварии
      if (flag_alarm) {
        Serial.println("Шаг Авария");
        flag_alarm = false;  // Флаг состояния
      }
      digitalWrite(MOTOR_PIN, LOW);  // Гарантированно отключаем мотор
      //step = CODESYS_SEL(ResetErr.isClick(), step, 0);
      step = ResetErr.press() ? 0 : step;
      break;
  }

  //p_Pusk = (step > 0) && (step <2);
  //S/erial.println(p_Pusk);
  //Serial.println(step);
}

/*  TimMotor.run(PUSK_PIN.hold());
  digitalWrite(MOTOR_PIN, TimMotor.Q());
  if (TimMotor.Q()) {
    Serial.println("Мотор работает");
  }

  if (TimMotor.Q_Ftrig()) {
    Serial.println("Мотор выключился");
  }
  //tStep = CODESYS_SEL(!iRun, tStep, 1);
  //Serial.println(TimMotor.ET());
  //Serial.println("Включен");
}
//Serial.println("Включен");
*/


/* if (firstBoot) {  // Проверяем, установлен ли флаг firstBoot == true, если firstBoot == false: весь блок пропускается.
    initTime = initTime ? initTime : millis();
    if (millis() - initTime >= 1500) step = 0, firstBoot = false;
    return;
  }

  step = (!Thermal_relay.hold() ? 10 : step);  // Если тепловое реле выключено сразу идем в шаг 10  блокировки
// Если тепловое реле выключено идем в шаг 10. Если включено то остаемся в этом шаге
  

  switch (step) {
    case 0:                                      //Проверяем блокировку
      step = (Thermal_relay.hold() ? 1 : step);  // Если нет блокировок идем в первый шаг
      break;

    case 1:  // Стоп. Ждем команду Пуск
      step = (PUSK_PIN.press() ? 2 : step);
      //tmr_w_motor.run(true);
      //myF_Trig(tmr_w_motor.Q)

      Serial.println("Блокировки нет. Ждем Пуск ");
      //step = ((iThermal_relay && Thermal_relay.press()) ? 2 : step);

      //Serial.println("Блокировки нет, ждем пуска мотора ");
      break;

    case 2:
      Serial.println("В шаге 2 ");
      digitalWrite(MOTOR_PIN, tmr_w_motor.Q);
      
      
      if (!tmr_w_motor.Q) {
                
                Serial.println("Таймер завершился. Переход в шаг 3.");
                step = 3;
      }
      //tmr_w_motor.run(false);
      break;
    case 3:
    
      Serial.println("В шаге 3 ");
    
      //Serial.println("Работаем ");
      //digitalWrite(MOTOR_PIN, tmr_w_motor.Q);
      //step = (tmr_w_motor.Q ? 3 : step);

      break;

    case 10:
      Serial.println("В 10 шаге. Блокировка. Сработало тепловое реле.");
      step = (ResetErr.click() ? 0 : step);
      //if (ResetErr.click()) {  // Надежное однократное срабатывание
      //step = 0;
      //Serial.println("Переход в step=0 по кнопке Reset");
      //}
      //Serial.println("В 10 шаге");
      //step = (ResetErr.click() ? 0 : step);
      //Serial.println("В 10 шаге ");
      break;
  }
}


myTP.run(PUSK_PIN.click());
  //Serial.println(PUSK_PIN.hold());  // Выводит 1 при удержании, 0 иначе
  digitalWrite(MOTOR_PIN, myTP.Q);
  if (R_detector.Run(myTP.Q)) {
    Serial.println("Передний фронт (R_TRIG)!");
  }
  if (F_detector.Run(myTP.Q)) {
    Serial.println("Задний фронт (R_TRIG)!");
    
  }

//myRS.run(PUSK_PIN.click(),STOP_PIN.click());  // Обновляем таймер
//if (myRS.Q()) {
//Serial.println("Сработал RS‑триггер");
//}
//digitalWrite(MOTOR_PIN, mySR.Q);
// Дополнительно: сообщение при истечении PT
//if (myTP.ET >= 3000 && PUSK_PIN.click()) {
//  Serial.println("Прошло 5 секунд!");
//}
//if (PUSK_PIN.hold()){
//Serial.println("Кнопка нажата");
// }

//TimerMs tmr_w_motor(5000, 0, 1);  // Таймер где просто задаем в милисикундах время
//TimerMs tmr_r_motor(5000, 0, 1);  // Таймер паузы работы привода
//TimerMs tmr_w_flap(3000, 0, 1);   // Таймер проверки срабатывания заслонки

//TON myTON();
//TON myTON(2000);  // Создаём экземпляр таймера
//TOF myTOF(3000);                              // Задержка 3000 мс (3 секунды)
//TP myTP((unsigned long)(delaySec * 1000));                                // PT = 3000 мс (3 секунды)

//F_TRIG F_detector;
//R_TRIG R_detector;
//SR_Trigger mySR;
//RS_Trigger myRS;  // Создаём RS-триггер*/
/*
  Serial.print("myTP.Q = ");
  Serial.println(myTP.Q ? "HIGH" : "LOW");
  Serial.print("myTP.ET = ");
  Serial.print(myTP.ET);
  Serial.println(" ms");




  
 */













//digitalWrite(13, TOF_Motor.q);  // Включаем светодиод (5 В)
//TON_Motor.in = PUSK_PIN.hold();

// Обновляем состояние таймера
//TON_Motor.update();

// Включаем светодиод, если таймер сработал (q == true)

//digitalWrite(MOTOR_PIN, TON_Motor.q);




//TOF_Motor.in = PUSK_PIN.isPress();
//TOF_Motor.update();
//digitalWrite(13, TOF_Motor.q);  // Включаем светодиод (5 В)
//if (TOF_Motor.q) {
//Serial.println("Таймер сработал ");
//digitalWrite(13, HIGH);  // Включаем светодиод (5 В)
//}



//if (ResetErr.click()) {  // Надежное однократное срабатывание
//step = 0;
//Serial.println("Переход в step=0 по кнопке Reset");
