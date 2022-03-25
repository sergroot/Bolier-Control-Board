// Модуль управления бойлером v. 0.3. Релиз от 07.01.2022. 23:00
#include <Time.h>  
#include <Wire.h>  
#include <DS1307RTC.h>

#define Close_HOT88_Neptun      2               // ПИН Закрыть ГВС88 от Модуля NEPTUN88. В текущей редакции не используется.                      
#define Open_HOT88_Neptun       3               // ПИН Открыть ГВС88 от Модуля NEPTUN88. В текущей редакции не используется.
#define Close_COLD88_Neptun     4               // ПИН Закрыть ХВС88 от Модуля NEPTUN88. В текущей редакции не используется.
#define Open_COLD88_Neptun      5               // ПИН Открыть ХВС88 от Модуля NEPTUN88. В текущей редакции не используется.
#define Close_HOT89_Neptun      10              // ПИН Закрыть ГВС89 от Модуля NEPTUN89. В текущей редакции не используется.                      
#define Open_HOT89_Neptun       11              // ПИН Открыть ГВС89 от Модуля NEPTUN89. В текущей редакции не используется.
#define Close_COLD89_Neptun     12              // ПИН Закрыть ХВС89 от Модуля NEPTUN89. В текущей редакции не используется.
#define Open_COLD89_Neptun      13              // ПИН Открыть ХВС89 от Модуля NEPTUN89. В текущей редакции не используется.
#define Close_HOT88             6               // ПИН Закрыть ГВС88. Сигнал к ключу от Arduino.
#define Open_HOT88              7               // ПИН Открыть ГВС88. Сигнал к ключу от Arduino.
#define Close_COLD88            8               // ПИН Закрыть ХВС88. Сигнал к ключу от Arduino.
#define Open_COLD88             9               // ПИН Открыть ХВС88. Сигнал к ключу от Arduino.
#define Close_HOT89             14              // ПИН Закрыть ГВС89. Сигнал к ключу от Arduino.
#define Open_HOT89              15              // ПИН Открыть ГВС89. Сигнал к ключу от Arduino.
#define Close_COLD89            16              // ПИН Закрыть ХВС89. Сигнал к ключу от Arduino.
#define Open_COLD89             17              // ПИН Открыть ХВС89. Сигнал к ключу от Arduino.
#define Close_Drainage_HOT      18              // ПИН Закрыть слив ГВС из бойлера (аварийный режим)
#define Open_Drainage_HOT       19              // ПИН Открыть слив ГВС из бойлера (аварийный режим)
#define Close_Drainage_COLD     40              // ПИН Закрыть слив ХВС из бойлера (аварийный режим)
#define Open_Drainage_COLD      41              // ПИН Открыть слив ХВС из бойлера (аварийный режим)
#define Close_Input_Water       22              // ПИН Закрыть подачу воды в бойлер
#define Open_Input_Water        23              // ПИН Закрыть подачу воды в бойлер
#define BUTTON_PIN              24              // ПИН Кнопки
#define LED_PIN                 25              // ПИН Светодиода и реле включения бойлера
#define Dry_Neptun88            26              // ПИН Сухой контакт от NEPTUN88 (сработала защита от протечки)
#define Dry_Neptun89            27              // ПИН Сухой контакт от NEPTUN89 (сработала защита от протечки)
#define Close_Neptun88          30              // ПИН Внешнее управление для NEPTUN88 (Закрыть краны). В текущей редакции не используется
#define Close_Neptun89          31              // ПИН Внешнее управление для NEPTUN89 (Закрыть краны). В текущей редакции не используется
boolean buttonWasUp = true;                     // true - Кнопка отпущена
boolean buttonIsUp;                             // Флаг состояния Кнопки
boolean ledEnabled = false;                     // Флаг состояния бойлера вкл/выкл
long time_btnClick;                             // Определяем время, прошедшее с момента нажатия кнопки. Следующее нажатие обрабатываем спустя только sec_btnClick секунд
long time_tapActive;                            // Определяем время, прошедшее с момента начала работы кранов. Спустя время sec_tapActive снимаем напряжение с кранов
long time_protect88;                            // Определяем время срабатывания защиты от протечки 88
long time_protect89;                            // определяем время срабатывания защиты от протечки 89
long sec_tapActive = 45000;                      // Определяем время работы кранов в милисекундах
long sec_btnClick = 5000;                       // Спустя это количестве милисекунд обрабатывает нажатие кнопки
boolean protect88 = false;                      // Флаг протечки в 88
boolean protect89 = false;                      // флаг протечки в 89

void setup()
{
Serial.begin(9600);                            // Открываем серийный порт для отладки
pinMode (Close_HOT88_Neptun, INPUT);
pinMode (Open_HOT88_Neptun, INPUT);
pinMode (Close_COLD88_Neptun, INPUT);
pinMode (Open_COLD88_Neptun, INPUT);
pinMode (Close_HOT88_Neptun, INPUT);
pinMode (Open_HOT88_Neptun, INPUT);
pinMode (Close_COLD88_Neptun, INPUT);
pinMode (Open_COLD88_Neptun, INPUT);
pinMode (Close_HOT88, OUTPUT);
pinMode (Open_HOT88, OUTPUT);
pinMode (Close_COLD88, OUTPUT);
pinMode (Open_COLD88, OUTPUT);
pinMode (Close_HOT89, OUTPUT);
pinMode (Open_HOT89, OUTPUT);
pinMode (Close_COLD89, OUTPUT);
pinMode (Open_COLD89, OUTPUT);
pinMode (Close_Drainage_HOT, OUTPUT);
pinMode (Open_Drainage_HOT, OUTPUT);
pinMode (Close_Drainage_COLD, OUTPUT);
pinMode (Open_Drainage_COLD, OUTPUT);
pinMode (Close_Input_Water, OUTPUT);
pinMode (Open_Input_Water, OUTPUT);
pinMode (BUTTON_PIN, INPUT_PULLUP);
pinMode (LED_PIN, OUTPUT);
pinMode (Dry_Neptun88, INPUT_PULLUP);
pinMode (Dry_Neptun89, INPUT_PULLUP);
pinMode (Close_Neptun88, OUTPUT);
pinMode (Close_Neptun89, OUTPUT);

time_btnClick = millis() - sec_btnClick;        // Делаем таймер включения меньше текущего времени на sec_btnClick милисекунд, чтобы включение бойлера было доступно сразу после включения
digitalWrite (LED_PIN, LOW);                    // Бойлер при подаче питания  - выключен.
// Все краны при включении обесточиваем
digitalWrite (Open_HOT88, LOW);                 // Канал "Открыть ГВС88" - инактивируем
digitalWrite (Close_HOT88, LOW);                // Канал "Закрыть ГВС88" - инактивируем
digitalWrite (Open_HOT89, LOW);                 // Канал "Открыть ГВС89" - инактивируем
digitalWrite (Close_HOT89, LOW);                // Канал "Закрыть ГВС89" - инактивируем
digitalWrite (Open_HOT88, LOW);                 // Канал "Открыть ГВС88" - инактивируем
digitalWrite (Close_HOT88, LOW);                // Канал "Закрыть ГВС88" - инактивируем
digitalWrite (Open_HOT89, LOW);                 // Канал "Открыть ГВС89" - инактивируем
digitalWrite (Close_HOT89, LOW);                // Канал "Закрыть ГВС89" - инактивируем
digitalWrite (Open_Drainage_HOT, LOW);          // Канал "Открыть слив ГВС из бойлера" - инактивируем
digitalWrite (Close_Drainage_HOT, LOW);         // Канал "Закрыть слив ГВС из бойлера" - инактивируем
digitalWrite (Open_Drainage_COLD, LOW);         // Канал "Открыть слив ХВС из бойлера" - инактивируем
digitalWrite (Close_Drainage_COLD, LOW);        // Канал "Закрыть слив ХВС из бойлера" - инактивируем
digitalWrite (Open_Input_Water, LOW);           // Канал "Открыть подачу воды в бойлер" - инактивируем    
digitalWrite (Close_Input_Water, LOW);          // Канал "Закрыть подачу воды в бойлер" - инактивируем 
}

void loop()
{  
// Блок обработки нажатого состояния кнопки
  buttonIsUp = digitalRead(BUTTON_PIN); // Считывание текущего состояния Кнопки
  if (buttonWasUp && !buttonIsUp) {             // ...если «Кнопка была отпущена и не отпущена сейчас»...
    delay (10);
    buttonIsUp = digitalRead(BUTTON_PIN);       // Опять читаем состояние Кнопки
    if (!buttonIsUp && millis() - time_btnClick > sec_btnClick && !protect88 && !protect89) {  // Если Кнопка отпущена и не срабатывала защита от протечки, то...
      ledEnabled = !ledEnabled;                 // Меняем состояние светодиода и...
      time_btnClick = millis();                 // Запоминаем время смены состояния светодиода
      time_tapActive = millis();                // Запоминаем время начала работы кранов
    }
  }
  buttonWasUp = buttonIsUp;                     // запоминаем последнее состояние Кнопки для новой итерации, чтобы не обрабатывать снова нажатое состояни

  // Если Светодиод горит и не срабатывала защита от протечки - включаем режим бойлера на время
  if (ledEnabled && millis() - time_tapActive <= sec_tapActive && !protect88 && !protect89){
    // Подаем воду в бойлер
    digitalWrite (Open_Input_Water, HIGH);    // Канал "Открыть подачу воды в бойлер" - активируем    
    digitalWrite (Close_Input_Water, LOW);    // Канал "Закрыть подачу воды в бойлер" - инактивируем 
    // Закрываем горячую воду 88
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, HIGH);         // Канал "Закрыть ГВС88" - активируем
    // Закрываем горячую воду 89    
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, HIGH);         // Канал "Закрыть ГВС89" - активируем
    // Открываем холодную воду 88
    digitalWrite (Close_COLD88, LOW);         // Канал "Закрыть ХВС88" - инактивируем
    digitalWrite (Open_COLD88, HIGH);         // Канал "Открыть ХВС88" - активируем
    // Открываем холодную воду 89
    digitalWrite (Close_COLD89, LOW);         // Канал "Закрыть ХВС89" - инактивируем
    digitalWrite (Open_COLD89, HIGH);         // Канал "Открыть ХВС89" - активируем
    // Закрываем слив горячей воды из бойлера
    digitalWrite (Open_Drainage_HOT, LOW);    // Канал "Открыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_HOT, HIGH);  // Канал "Закрыть слив ГВС из бойлера" - активируем
    // Закрываем слив холодной воды из бойлера
    digitalWrite (Open_Drainage_COLD, LOW);   // Канал "Открыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_COLD, HIGH); // Канал "Закрыть слив ХВС из бойлера" - активируем
    // Включаем реле бойлера
    digitalWrite(LED_PIN, ledEnabled);        // Включаем индикатор включения бойлера
    }
// Если Светодиод не горит и не срабатывала защита от протечки - выключаем режим бойлера
  if (!ledEnabled && millis() - time_tapActive <= sec_tapActive && !protect88 && !protect89){
    // Выключаем реле бойлера
    digitalWrite(LED_PIN, ledEnabled);        // Выключаем индикатор включения бойлера
    // Открываем горячую воду 88
    digitalWrite (Close_HOT88, LOW);          // Канал "Закрыть ГВС88" - инактивируем    
    digitalWrite (Open_HOT88, HIGH);          // Канал "Открыть ГВС88" - активируем
    // Открываем горячую воду 89
    digitalWrite (Close_HOT89, LOW);          // Канал "Закрыть ГВС89" - инактивируем
    digitalWrite (Open_HOT89, HIGH);          // Канал "Открыть ГВС89" - активируем
    // Открываем холодную воду 88
    digitalWrite (Close_COLD88, LOW);         // Канал "Закрыть ХВС88" - инактивируем    
    digitalWrite (Open_COLD88, HIGH);         // Канал "Открыть ХВС88" - активируем
    // Открываем холодную воду 89
    digitalWrite (Close_COLD89, LOW);         // Канал "Закрыть ХВС89" - инактивируем    
    digitalWrite (Open_COLD89, HIGH);         // Канал "Открыть ХВС89" - активируем
    // Закрываем слив горячей воды из бойлера
    digitalWrite (Open_Drainage_HOT, LOW);    // Канал "Открыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_HOT, HIGH);  // Канал "Закрыть слив ГВС из бойлера" - активируем
    // Закрываем слив холодной воы из бойлера
    digitalWrite (Open_Drainage_COLD, LOW);   // Канал "Открыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_COLD, HIGH); // Канал "Закрыть слив ХВС из бойлера" - активируем
    // Закрываем подачу воды в бойлер
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем    
    digitalWrite (Close_Input_Water, HIGH);   // Канал "Закрыть подачу воды в бойлер" - активируем
    }
    
 // Если краны работают дольше, чем sec_tapActive и не срабатывала защита от протечки, то снимаем напряжение с кранов
 if (millis() - time_tapActive > sec_tapActive && !protect88 && !protect89){
    // Снимаем напряжение со всех кранов
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, LOW);          // Канал "Закрыть ГВС88" - инактивируем
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, LOW);          // Канал "Закрыть ГВС89" - инактивируем
    digitalWrite (Open_COLD88, LOW);           // Канал "Открыть ХВС88" - инактивируем
    digitalWrite (Close_COLD88, LOW);          // Канал "Закрыть ХВС88" - инактивируем
    digitalWrite (Open_COLD89, LOW);           // Канал "Открыть ХВС89" - инактивируем
    digitalWrite (Close_COLD89, LOW);          // Канал "Закрыть ХВС89" - инактивируем
    digitalWrite (Open_Drainage_HOT, LOW);    // Канал "Открыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_HOT, LOW);   // Канал "Закрыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Open_Drainage_COLD, LOW);   // Канал "Открыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_COLD, LOW);  // Канал "Закрыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем    
    digitalWrite (Close_Input_Water, LOW);    // Канал "Закрыть подачу воды в бойлер" - инактивируем   
    }

 // Сработала защита от протечки в 88 и ранее она не была зафиксирована
 if (digitalRead (Dry_Neptun88) == LOW && !protect88) {
    time_protect88 = millis();                // Запоминаем время срабатывания защиты от протечки 88
    protect88 = true;                         // Устанавливаем Флаг протечки в 88
    digitalWrite(LED_PIN, LOW);               // Выключаем бойлер
    // Закрываем подачу воды в бойлер
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем   
    digitalWrite (Close_Input_Water, HIGH);   // Канал "Закрыть подачу воды в бойлер" - активируем
    // Закрываем горячую воду 88
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, HIGH);         // Канал "Закрыть ГВС88" - активируем
    // Закрываем горчую воду 89    
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, HIGH);         // Канал "Закрыть ГВС89" - активируем
    // Закрываем холодную воду 88
    digitalWrite (Open_COLD88, LOW);          // Канал "Открыть ХВС88" - инактивируем
    digitalWrite (Close_COLD88, HIGH);        // Канал "Закрыть ХВС88" - активируем    
    // Закрываем холодную воду 89
    digitalWrite (Open_COLD89, LOW);          // Канал "Открыть ХВС89" - инактивируем
    digitalWrite (Close_COLD89, HIGH);        // Канал "Закрыть ХВС89" - активируем
    // Сливаем горячую воду из бойлера
    digitalWrite (Close_Drainage_HOT, LOW);   // Канал "Закрыть слив ГВС из бойлера" - инактивируем    
    digitalWrite (Open_Drainage_HOT, HIGH);   // Канал "Открыть слив ГВС из бойлера" - активируем
    // Сливаем холодную воду из бойлера
    digitalWrite (Close_Drainage_COLD, LOW);  // Канал "Закрыть слив ХВС из бойлера" - инактивируем    
    digitalWrite (Open_Drainage_COLD, HIGH);  // Канал "Открыть слив ХВС из бойлера" - активируем
  }
 // Протечка 88 ликвидирована - флаг протечки 88 был взведен
 if (digitalRead (Dry_Neptun88) == HIGH && protect88 == true) {
    protect88 = false;                        // Снимаем флаг активной защиты от протечки 88. Бойлер можно включать.
    digitalWrite (LED_PIN, ledEnabled);       // Восстанавливаем состояние бойлера до фиксации протечки
    time_tapActive = millis();                // Краны при необходимости включатся на определенное время  
    }
 // Если краны работают дольше, чем sec_tapActive и срабатывала защита от протечки 88, то снимаем напряжение с кранов
 if (millis() - time_protect88 > sec_tapActive && protect88){
    // Обесточиваем все краны
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, LOW);          // Канал "Закрыть ГВС88" - инактивируем
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, LOW);          // Канал "Закрыть ГВС89" - инактивируем
    digitalWrite (Open_COLD88, LOW);          // Канал "Открыть ХВС88" - инактивируем
    digitalWrite (Close_COLD88, LOW);         // Канал "Закрыть ХВС88" - инактивируем
    digitalWrite (Open_COLD89, LOW);          // Канал "Открыть ХВС89" - инактивируем
    digitalWrite (Close_COLD89, LOW);         // Канал "Закрыть ХВС89" - инактивируем
    digitalWrite (Open_Drainage_HOT, LOW);    // Канал "Открыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_HOT, LOW);   // Канал "Закрыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Open_Drainage_COLD, LOW);   // Канал "Открыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_COLD, LOW);  // Канал "Закрыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем    
    digitalWrite (Close_Input_Water, LOW);    // Канал "Закрыть подачу воды в бойлер" - инактивируем   
    }
 // Сработала защита от протечки в 89 и ранее она не была зафиксирована
 if (digitalRead (Dry_Neptun89) == LOW && !protect89) {
    time_protect89 = millis();                // Запоминаем время срабатывания защиты от протечки 89
    protect89 = true;                         // Устанавливаем Флаг протечки в 89
    digitalWrite(LED_PIN, LOW);               // Выключаем бойлер
    // Закрываем подачу воды в бойлер
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем   
    digitalWrite (Close_Input_Water, HIGH);   // Канал "Закрыть подачу воды в бойлер" - активируем
    // Закрываем горячую воду 88
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, HIGH);         // Канал "Закрыть ГВС88" - активируем
    // Закрываем горчую воду 89    
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, HIGH);         // Канал "Закрыть ГВС89" - активируем
    // Закрываем холодную воду 88
    digitalWrite (Open_COLD88, LOW);          // Канал "Открыть ХВС88" - инактивируем
    digitalWrite (Close_COLD88, HIGH);        // Канал "Закрыть ХВС88" - активируем    
    // Закрываем холодную воду 89
    digitalWrite (Open_COLD89, LOW);          // Канал "Открыть ХВС89" - инактивируем
    digitalWrite (Close_COLD89, HIGH);        // Канал "Закрыть ХВС89" - активируем
    // Сливаем горячую воду из бойлера
    digitalWrite (Close_Drainage_HOT, LOW);   // Канал "Закрыть слив ГВС из бойлера" - инактивируем    
    digitalWrite (Open_Drainage_HOT, HIGH);   // Канал "Открыть слив ГВС из бойлера" - активируем
    // Сливаем холодную воду из бойлера
    digitalWrite (Close_Drainage_COLD, LOW);  // Канал "Закрыть слив ХВС из бойлера" - инактивируем    
    digitalWrite (Open_Drainage_COLD, HIGH);  // Канал "Открыть слив ХВС из бойлера" - активируем
  }
// Протечка 89 ликвидирована - флаг протечки 89 был взведен:
  if (digitalRead (Dry_Neptun89) == HIGH && protect89 == true) {
    protect89 = false;                        // Снимаем флаг активной защиты от протечки 89. Бойлер можно включать
    digitalWrite (LED_PIN, ledEnabled);       // Восстанавливаем состояние бойлера до фиксации протечки
    time_tapActive = millis();                // Краны при необходимости включатся на определенное время 
    } 
// Если краны работают дольше, чем sec_tapActive и срабатывала защита от протечки 89, то снимаем напряжение с кранов
 if (millis() - time_protect89 > sec_tapActive && protect89){
    // Обесточиваем все краны
    digitalWrite (Open_HOT88, LOW);           // Канал "Открыть ГВС88" - инактивируем
    digitalWrite (Close_HOT88, LOW);          // Канал "Закрыть ГВС88" - инактивируем
    digitalWrite (Open_HOT89, LOW);           // Канал "Открыть ГВС89" - инактивируем
    digitalWrite (Close_HOT89, LOW);          // Канал "Закрыть ГВС89" - инактивируем
    digitalWrite (Open_COLD88, LOW);          // Канал "Открыть ХВС88" - инактивируем
    digitalWrite (Close_COLD88, LOW);         // Канал "Закрыть ХВС88" - инактивируем
    digitalWrite (Open_COLD89, LOW);          // Канал "Открыть ХВС89" - инактивируем
    digitalWrite (Close_COLD89, LOW);         // Канал "Закрыть ХВС89" - инактивируем
    digitalWrite (Open_Drainage_HOT, LOW);    // Канал "Открыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_HOT, LOW);   // Канал "Закрыть слив ГВС из бойлера" - инактивируем
    digitalWrite (Open_Drainage_COLD, LOW);   // Канал "Открыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Close_Drainage_COLD, LOW);  // Канал "Закрыть слив ХВС из бойлера" - инактивируем
    digitalWrite (Open_Input_Water, LOW);     // Канал "Открыть подачу воды в бойлер" - инактивируем    
    digitalWrite (Close_Input_Water, LOW);    // Канал "Закрыть подачу воды в бойлер" - инактивируем   
    }    
}
