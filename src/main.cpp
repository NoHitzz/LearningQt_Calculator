// 
// main.cpp
// Emu_Calculator
// 
// Noah Hitz 2024
// 

/** 
 *  Calculator. A calculator implementation using the Qt library.
 *  Copyright (C) 2024 Noah Hitz
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 **/


#include <QApplication>
#include "mainwindow.h"

int main(int argc, char** argv) {
    QApplication calculator(argc, argv); 

    EmuCalc::MainWindow mainwin;
    mainwin.show();

    return calculator.exec();
}
