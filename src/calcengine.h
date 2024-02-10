// 
// calcengine.h
// Emu_Calculator
// 
// Noah Hitz 2024
// 

#ifndef CALCENGINE_H
#define CALCENGINE_H

#include <ios>
#include <string>

#include "QtCore/qdebug.h"
#include "QtCore/qobject.h"
#include "QtCore/qtmetamacros.h"


namespace EmuCalc {
    class CalcEngine : public QObject {
        Q_OBJECT
        public:
            CalcEngine() = default;
            ~CalcEngine(); 

            static double custom_stod(const std::string& input); 
            static std::string custom_numberToStr(const double& input);

            static std::string add(std::string& str1, std::string& str2);
            static std::string subtract(std::string& str1, std::string& str2);
            static std::string multiply(std::string& str1, std::string& str2);
            static std::string divide(std::string& str1, std::string& str2);

            void clear();
            void switchCurrent();
            void loadMem();
            void clearMem(); 
            void memAdd();
            void memSubtract();
            void signSwitch();
            void percentage();
            void decimalPoint();
            void equals(); 
            void appendDigit(const int& digit);

            const std::string& getCurrent() const { return *m_currentNumber; }
            bool getDecimalFlag() const { return *m_currentDecimalFlag; }

            void setCurrentTo(const std::string& number) { *m_currentNumber = number; hasChanged(); }
            void setDecimalFlag() { *m_currentDecimalFlag = true; }

            private:
            void append(const char c); 
            bool isDecimal(const std::string& number) const; 

        signals:
            void hasChanged();

        public:
            std::function<std::string(std::string&, std::string&)> m_currentOperation {};

        private:
            std::string m_primaryNumber = "0";
            std::string m_secondaryNumber = "0";
            std::string* m_currentNumber = &m_primaryNumber;
            
            std::string m_memNumber = "0";

            bool m_primaryDecimalFlag = false;
            bool m_secondaryDecimalFlag = false;
            bool* m_currentDecimalFlag = &m_primaryDecimalFlag;

            bool m_memDecimalFlag = false;
    };

} /* EmuCalc namespace */
#endif /* CALCENGINE_H */
