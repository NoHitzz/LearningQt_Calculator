// 
// calcengine.cpp
// Emu_Calculator
// 
// Noah Hitz 2024
// 

#include <cstddef>
#include <sstream>

#include "QtCore/qlogging.h"

#include "calcengine.h"


namespace EmuCalc {
    CalcEngine::~CalcEngine() {}

    void CalcEngine::clear() {
        m_primaryNumber = "0";
        m_secondaryNumber = "0";
        m_currentNumber = &m_primaryNumber;
        m_primaryDecimalFlag = false;
        m_secondaryDecimalFlag = false;
        m_currentDecimalFlag = &m_primaryDecimalFlag;
        hasChanged();
    } 

    void CalcEngine::loadMem() { 
        *m_currentNumber = m_memNumber;
        *m_currentDecimalFlag = m_memDecimalFlag;
        hasChanged();
    }

    void CalcEngine::clearMem() { 
        m_memNumber = "0"; 
        m_memDecimalFlag = false;
    }

    void CalcEngine::memAdd() {
        m_memNumber = add(m_memNumber, *m_currentNumber);
        m_memDecimalFlag = isDecimal(m_memNumber); 
        *m_currentNumber = "0";
        *m_currentDecimalFlag = false;
        hasChanged();
    }

    void CalcEngine::memSubtract() {
        m_memNumber = subtract(m_memNumber, *m_currentNumber);
        m_memDecimalFlag = isDecimal(m_memNumber); 
        *m_currentNumber = "0";
        *m_currentDecimalFlag = false;
        hasChanged();
    }

    void CalcEngine::signSwitch() {
        double n{};
        try {
            n = custom_stod(*m_currentNumber);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
       
        if (n == 0)
            return;
        
        *m_currentNumber = custom_numberToStr(n * -1);
        hasChanged();
    }

    void CalcEngine::percentage() {
        if (m_currentNumber != &m_secondaryNumber) 
            return;

        double a{};
        double b{};
        try {
            a = custom_stod(m_primaryNumber);
            b = custom_stod(m_secondaryNumber);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
        m_secondaryNumber = custom_numberToStr((b/100) * a);
        m_secondaryDecimalFlag = isDecimal(m_secondaryNumber);
        hasChanged();
    }

    void CalcEngine::decimalPoint() { 
        if (*m_currentDecimalFlag)
            return;
        append('.'); 
        setDecimalFlag(); 
        hasChanged();
    }

    void CalcEngine::equals() {
        if(m_currentOperation == nullptr || m_currentNumber != &m_secondaryNumber)
            return;
        m_currentNumber = &m_primaryNumber;
        m_primaryNumber = m_currentOperation(m_primaryNumber, m_secondaryNumber);
        m_secondaryNumber = "0";
        m_secondaryDecimalFlag = false;
        m_primaryDecimalFlag = isDecimal(m_primaryNumber);
        m_currentDecimalFlag = &m_primaryDecimalFlag;
        m_currentOperation = nullptr;
        hasChanged();
    }

    void CalcEngine::switchCurrent() {
        m_currentNumber = &m_secondaryNumber;
        m_currentDecimalFlag = &m_secondaryDecimalFlag;
        hasChanged();
    }

    void CalcEngine::appendDigit(const int& digit) { 
        append(digit + '0'); 
        hasChanged();
    }

    void CalcEngine::append(const char c) { 
        *m_currentNumber += c;
    }

    bool CalcEngine::isDecimal(const std::string& number) const {
        bool result = {};
        result = std::find(number.begin(), number.end(), '.') != number.end();
        return result;
    }


    /* --- Static Math Operator Implementations --- */
    std::string CalcEngine::add(std::string& str1, std::string& str2) {
        double a {};
        double b {};
        try {
            a = custom_stod(str1);
            b = custom_stod(str2);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }

        return custom_numberToStr(a+b); 
    }

    std::string CalcEngine::subtract(std::string &str1, std::string &str2) {
        double a{};
        double b{};
        try {
            a = custom_stod(str1);
            b = custom_stod(str2);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
        return custom_numberToStr(a-b);
    }

    std::string CalcEngine::multiply(std::string &str1, std::string &str2) {
        double a{};
        double b{};
        try {
            a = custom_stod(str1);
            b = custom_stod(str2);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
        return custom_numberToStr(a*b);
    }

    std::string CalcEngine::divide(std::string &str1, std::string &str2) {
        double a{};
        double b{};
        try {
            a = custom_stod(str1);
            b = custom_stod(str2);
        } catch(std::exception& e) {
            qDebug() << "Exception: " << e.what();
        }
        if(b == 0) {
            return "nan";
        }
        return custom_numberToStr(a/b);
    }


    /* --- Utility Functions --- */
    // Qt messes with the C locale settings and this breaks some C++ functions,
    // these utility functions are used to circumvent that 
    double CalcEngine::custom_stod(const std::string& input) {
        double result {};
        std::istringstream istream(input);
        istream.imbue(std::locale("C"));
        istream >> result;

        if (istream.fail() || istream.bad() || !istream.eof()) 
            throw std::invalid_argument("failed to convert string to double");

        return result;
    }

    std::string CalcEngine::custom_numberToStr(const double& input) {
        std::ostringstream sstream;
        sstream.imbue(std::locale("C"));
        sstream << input;
        return sstream.str();
    }

} /* EmuCalc namespace */
