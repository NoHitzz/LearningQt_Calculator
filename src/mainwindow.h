// 
// mainwindow.h
// Emu_Calculator
// 
// Noah Hitz 2024
// 

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <_types/_uint8_t.h>
#include <functional>

#include "QtCore/qobject.h"
#include "QtCore/qtmetamacros.h"

#include "src/calcengine.h"

class QGridLayout;
class QLineEdit;
class QPushButton;

namespace EmuCalc {
    class MainWindow : public QMainWindow {
        Q_OBJECT
        public:
            enum Buttons {
                zero = 0, one, two, three, four, five, six, seven, eight, nine,      
                dot = 10, memget, memminus, memplus, 
                memclear = 14, percentage, signswitch,
                clear = 17, divide, multiply, subtract, add, equal
            };

        public:
            explicit MainWindow(QWidget* parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
            ~MainWindow();

        private:
            void uisetup(); 
            void initializeButtons();

            void keyPressEvent(QKeyEvent *event) override;
            void numberInput(const int& digit);
            void processOperation(Buttons bttn);

        private slots:
            void buttonInput();
            void updateInterface();

        private:
            static constexpr int m_gridRows = 7;
            static constexpr int m_gridColumns = 4;
            static constexpr int m_displaySizeY = 72;
            static constexpr int m_numberOfButtons = 23;
            static constexpr int m_bttnSizeX = 85;
            static constexpr int m_bttnSizeY = 72;
            static constexpr int m_bttnIdOffset = 100;

            QWidget* m_rootWidget {};
            QGridLayout* m_mainGridLayout {};

            QLineEdit* m_numberDisplay {};
            QPushButton* m_buttons[m_numberOfButtons];

            CalcEngine m_calcEngine {};
    };

} /* EmuCalc namespace */
#endif /* MAINWINDOW_H */
