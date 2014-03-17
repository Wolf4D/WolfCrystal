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

#ifndef WOLFCRYSTALS_H
#define WOLFCRYSTALS_H

#include <inttypes.h>
#include <LiquidCrystal.h>

class WolfCrystal
{
public:

// Конструктор и добавление LCD
WolfCrystal(LiquidCrystal * LCD=0);
void AddLCD(LiquidCrystal * LCD);

// Функции(я) преобразования строки в вид, пригодный для отправки на дисплей.
String GS(char *input);
String GS(String input);

private:
// ProcessChars обрабатывает введённую строку.
String ProcessChars(String input);

// Записываем глифы в память контроллера.
void CreateCapsGlyphs();

// Ссылка на объект LiquidCrystal
LiquidCrystal * myLCD;

};

#endif // WOLFCRYSTALS_H