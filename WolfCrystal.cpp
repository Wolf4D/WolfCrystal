/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 * WolfCrystals v1.0.0
 * Библиотека, предназначенная для обеспечения поддержки русского языка 
 * теми из HD44780-совместимых дисплеев, в память знакогенератора которых 
 * изначально была зашита катакана, а не кириллица.
 *
 * Это осуществляется путём замены русских букв подобными им
 * символами из английского алфавита и катаканы. Для семи русских букв
 * были нарисованы имитирующие их глифы. 
 * 
 * Библиотека совместима с Arduino IDE 1.0+
 * (C) 2014 Ivan Klenov, Wolf4D@list.ru
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 */

#include "WolfCrystal.h"

#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <LiquidCrystal.h>
#include <avr/pgmspace.h>

#include <string>


#if defined(ARDUINO) && ARDUINO >= 100
  #include "Arduino.h"
#else
  #include "WProgram.h"
#endif

///////////////////////////////////////////////////////////////////////////////
// Для большого количества строчных русских символов эквиваленты выглядели 
// гораздо хуже, чем для прописных (заглавных). 

// Потому по умолчанию все строчные русские буквы переводятся в прописные.
// Если требуется включить вывод русских строчных (а они местами ой как корявы),
// то раскомментируйте следующую строку:
//#define CASE_SENSITIVE
///////////////////////////////////////////////////////////////////////////////

// Глифы русских букв (использовано из них 7 штук, 1 глиф свободен):
byte be[8] = {
  0b11111,
  0b10000,
  0b10000,
  0b11110,
  0b10001,
  0b10001,
  0b11110,
  0b00000
};

byte ge[8] = {
  0b11111,
  0b10001,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b10000,
  0b00000
};


byte de[8] = {
  0b00111,
  0b01001,
  0b01001,
  0b01001,
  0b01001,
  0b01001,
  0b01111,
  0b10001
};

byte zje[8] = {
  0b10101,
  0b10101,
  0b01110,
  0b00100,
  0b01110,
  0b10101,
  0b10101,
  0b00000
};


byte pe[8] = {
  0b11111,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b10001,
  0b00000
};

byte ii[8] = {
  0b10001,
  0b10011,
  0b10101,
  0b10101,
  0b10101,
  0b11001,
  0b10001,
  0b00000
};

byte uu[8] = {
  0b10010,
  0b10101,
  0b10101,
  0b11101,
  0b10101,
  0b10101,
  0b10010,
  0b00000
};


byte ll[8] = {
  0b00111,
  0b01001,
  0b01001,
  0b01001,
  0b01001,
  0b01001,
  0b10001,
  0b00000
};


byte ce[8] = {
  0b10010,
  0b10010,
  0b10010,
  0b10010,
  0b10010,
  0b10010,
  0b11111,
  0b00001
};

///////////////////////////////////////////////////////////////////////////////
// Конструктор и добавление LCD
WolfCrystal::WolfCrystal(LiquidCrystal * LCD) 
{ AddLCD(LCD); };

void WolfCrystal::AddLCD(LiquidCrystal * LCD)
{
if (LCD!=0) 
{myLCD=LCD; CreateCapsGlyphs();}
};

///////////////////////////////////////////////////////////////////////////////
// Функция преобразования строки в вид, пригодный для отправки на дисплей.

// Оформляет строку единообразно, вне зависимости от типа вызова (с char* или
// со String, и передаёт её в ProcessChars.
String WolfCrystal::GS(char *input)
{
String answer=String(input);
return ProcessChars(answer);
};

String WolfCrystal::GS(String input)
{return ProcessChars(input);};

///////////////////////////////////////////////////////////////////////////////
// ProcessChars обрабатывает введённую строку.
// Мне не удалось придумать лёгкого способа замены символов проще и эффективнее,
// чем обычный case. Тем более, что некоторые символы заменяются на  
// два. Кроме того, Arduino имеет несколько изъянов:

String WolfCrystal::ProcessChars(String input)
{
/*
Для хранения русского языка Arduino использует двухбайтовую кодировку.
Функция toUpperCase не работает с русским в таком виде. Похоже, она вообще 
ни в каком виде с национальными языками не работает - впрочем, на сайте
разработчиков это (весьма неявно) сказано.
Пришлось прописывать таблицу соответствий без оптимизаций, вручную.

Плюс, двухбайтовость кодировки даёт о себе знать при разложении String
в массив с использованием стандартного toCharArray. String копируется в 
массив _побайтно_, несмотря на то, что символы 2байтовые. 
То есть на русскую букву в массив char-ов пишется два байта. 

НО!
При этом преобразование из двухбайтовой в однобайтовую кодировку совершается!
Корректно преобразованный символ пишется в первый выделенный под символ char,
а во второй char попадает мусор. По этой причине вылезают "левые" символы, 
находящиеся в заведомо неиспользуемых областях кодовых таблиц. 

Можно было бы отбросить каждый второй char, но тогда могут пострадать 
однобайтовые символы. Потому был применён примитивный фильтр, отсекающий
мусор.

Также иногда по странной причине съедаются части строки - потому
было использовано два ни на что толком не влияющих костыля.
*/

// Костыль 1:
input=input+' ';

// Костыль 2
if ((input.charAt(0)<=127) && (input.charAt(0)>=32)) input = ' ' + input;

// Буфер однобайтовых символов
char Buf[(input.length()+2)*2+1];
input.toCharArray(Buf, input.length());

// Тут будем хранить выходную строку
String answer="";

// Не самый "прямой" способ, но по соотношению трудоёмкость/эффект
// точно не худший (худший я уже опробовал >_<).
// Заменяем кириллицу на латиницу, катакану (по кодовой таблице) или глифы.
for(int i=1; i<input.length(); i=i+1)
switch(Buf[i])
{
case 'А': answer=answer+'A'; break;
case 'Б': answer=answer+char(uint8_t(1)); break;
case 'В': answer=answer+'B'; break;
case 'Г': answer=answer+char(uint8_t(2)); break;
case 'Д': answer=answer+char(uint8_t(3)); break;
case 'Е': answer=answer+'E'; break;
//case 'Ё': answer=answer+'E'+char(222); break;
case 'Ж': answer=answer+'E'+char(200); break;
case 'З': answer=answer+'3'; break;
case 'И': answer=answer+char(uint8_t(5)); break;
case 'Й': answer=answer+char(uint8_t(5))+char(96); break;
case 'К': answer=answer+'K'; break;
case 'Л': answer=answer+char(uint8_t(6)); break;
case 'М': answer=answer+'M'; break;
case 'Н': answer=answer+'H'; break;
case 'О': answer=answer+'O'; break;
case 'П': answer=answer+char(uint8_t(4)); break;
case 'Р': answer=answer+'P'; break;
case 'С': answer=answer+'C'; break;
case 'Т': answer=answer+'T'; break;
case 'У': answer=answer+'Y'; break;
case 'Ф': answer=answer+'E'+char(236); break;
case 'Х': answer=answer+'X'; break;
case 'Ц': answer=answer+char(uint8_t(7)); break;
case 'Ч': answer=answer+char(209); break;
case 'Ш': answer=answer+'W'; break;
case 'Щ': answer=answer+char(208); break;
case 'Ъ': answer=answer+char(162)+'b'; break;
case 'Ы': answer=answer+'b|'; break;
case 'Ь': answer=answer+'b'; break;
case 'Э': answer=answer+char(214); break;
case 'Ю': answer=answer+char(196)+'O'; break;
case 'Я': answer=answer+'9|'; break;
// Так как никакие toUpper не работают для кириллицы, то просто прописано
// две ветви выполнения для разных настроек библиотеки. Это позволяет уменьшить
// потребление SRAM.
#ifdef CASE_SENSITIVE
case 'а': answer=answer+'a'; break;
case 'б': answer=answer+'6'; break;
case 'в': answer=answer+char(226); break;
case 'г': answer=answer+'r'; break;
case 'д': answer=answer+char(229); break;
case 'е': answer=answer+'e'; break;
// case 'ё': answer=answer+'e'+char(222); break;
// странный баг компилятора! буквы ё и Ё не принимаются!
case 'ж': answer=answer+'*'; break;
case 'з': answer=answer+char(174); break;
case 'и': answer=answer+'u'; break;
case 'й': answer=answer+'u'+char(96); break;
case 'к': answer=answer+'k'; break;
case 'л': answer=answer+char(202); break;
case 'м': answer=answer+'m'; break;
case 'н': answer=answer+char(252); break;
case 'о': answer=answer+'o'; break;
case 'п': answer=answer+'n'; break;
case 'р': answer=answer+'p'; break;
case 'с': answer=answer+'c'; break;
case 'т': answer=answer+'t'; break;
case 'у': answer=answer+'y'; break;
case 'ф': answer=answer+'q'+'p'; break;
case 'х': answer=answer+'x'; break;
case 'ц': answer=answer+'u'+char(164); break;
case 'ч': answer=answer+char(249); break;
case 'ш': answer=answer+'w'; break;
case 'щ': answer=answer+char(175); break;
case 'ъ': answer=answer+char(162)+'b'; break;
case 'ы': answer=answer+'b'+'|'; break;
case 'ь': answer=answer+'b'; break;
case 'э': answer=answer+char(166); break;
case 'ю': answer=answer+char(170)+'o'; break;
case 'я': answer=answer+'g'; break;
#else
case 'а': answer=answer+'A'; break;
case 'б': answer=answer+char(uint8_t(1)); break;
case 'в': answer=answer+'B'; break;
case 'г': answer=answer+char(uint8_t(2)); break;
case 'д': answer=answer+char(uint8_t(3)); break;
case 'е': answer=answer+'E'; break;
//case 'Ё': answer=answer+'E'+char(222); break;
case 'ж': answer=answer+'E'+char(200); break;
case 'з': answer=answer+'3'; break;
case 'и': answer=answer+char(uint8_t(5)); break;
case 'й': answer=answer+char(uint8_t(5))+char(96); break;
case 'к': answer=answer+'K'; break;
case 'л': answer=answer+char(uint8_t(6)); break;
case 'м': answer=answer+'M'; break;
case 'н': answer=answer+'H'; break;
case 'о': answer=answer+'O'; break;
case 'п': answer=answer+char(uint8_t(4)); break;
case 'р': answer=answer+'P'; break;
case 'с': answer=answer+'C'; break;
case 'т': answer=answer+'T'; break;
case 'у': answer=answer+'Y'; break;
case 'ф': answer=answer+'E'+char(236); break;
case 'х': answer=answer+'X'; break;
case 'ц': answer=answer+char(uint8_t(7)); break;
case 'ч': answer=answer+char(209); break;
case 'ш': answer=answer+'W'; break;
case 'щ': answer=answer+char(208); break;
case 'ъ': answer=answer+char(162)+'b'; break;
case 'ы': answer=answer+'b|'; break;
case 'ь': answer=answer+'b'; break;
case 'э': answer=answer+char(214); break;
case 'ю': answer=answer+char(196)+'O'; break;
case 'я': answer=answer+'9|'; break;
#endif
// Фильтр ввода. Если символ не конвертирован, но не за пределами "значимых" по
// кодовой таблице, то пишем его тоже.
default: {if (((Buf[i]<=127) && (Buf[i]>=32)) || (Buf[i]>252)) answer=answer+Buf[i]; }break;
}
// Кусочек костыля.
answer=answer+' ';

return answer;
};

//#endif

// Записываем глифы в память контроллера
void WolfCrystal::CreateCapsGlyphs()
{

  myLCD->createChar(1, be);
  myLCD->createChar(2, ge);
  myLCD->createChar(3, de);  
  myLCD->createChar(4, pe); 
  myLCD->createChar(5, ii);  
  myLCD->createChar(6, ll); 
  myLCD->createChar(7, ce);
};
