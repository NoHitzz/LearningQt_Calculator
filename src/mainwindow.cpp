// 
// mainwindow.cpp
// Emu_Calculator
// 
// Noah Hitz 2024
// 

#include <algorithm>
#include <charconv>
#include <cstdlib>
#include <sstream>
#include <QGridLayout>
#include <QLineEdit>
#include <QPushButton>
#include <QKeyEvent>
#include <stdexcept>
#include <string>

#include "QtCore/qlogging.h"
#include "QtCore/qnamespace.h"
#include "QtCore/qobject.h"
#include "QtCore/qstringlist.h"
#include "QtWidgets/qgridlayout.h"
#include "QtWidgets/qwidget.h"

#include "src/mainwindow.h"


namespace EmuCalc {
    MainWindow::MainWindow(QWidget* parent, Qt::WindowFlags flags) : QMainWindow(parent) {
        m_rootWidget = new QWidget;
        m_mainGridLayout = new QGridLayout;
        m_mainGridLayout->setSpacing(0);
        m_mainGridLayout->setContentsMargins(0,0,0,0);
        m_rootWidget->setLayout(m_mainGridLayout);

        uisetup();

        setWindowTitle("Calculator");
        setStyleSheet("QMainWindow { background: #505050; }");
        setFixedSize(m_bttnSizeX*m_gridColumns, m_displaySizeY+m_bttnSizeY*(m_gridRows-1)); 

        setCentralWidget(m_rootWidget);

        connect(&m_calcEngine, &CalcEngine::hasChanged, this, &MainWindow::updateInterface);
    }

    MainWindow::~MainWindow() {
        for(int i{}; i < m_numberOfButtons; i++) {
            delete m_buttons[i];
        }
    }


    void MainWindow::uisetup() {
        m_numberDisplay = new QLineEdit();
        m_numberDisplay->setStyleSheet(R"V(
        QLineEdit { color: #EBEBEB; background-color: #505050; padding: 0px,0px,0px,0px; border: none; color: #ffffff; }
    )V");
        m_numberDisplay->setReadOnly(true);
        m_numberDisplay->setFocus(Qt::FocusReason::NoFocusReason);
        m_numberDisplay->setFixedHeight(m_displaySizeY);
        m_numberDisplay->setAlignment(Qt::AlignRight | Qt::AlignBottom);
        m_numberDisplay->setFont(QFont("Helvetica", m_displaySizeY-10, QFont::Thin));
        m_numberDisplay->setTextMargins(2,10,2,0);
        m_numberDisplay->setContentsMargins(0,0,0,0);
        m_mainGridLayout->addWidget(m_numberDisplay, 0, 0, 1, m_gridColumns, Qt::AlignTop);

        initializeButtons();
        updateInterface();
    }


    void MainWindow::initializeButtons() {
        // setting up buttons
        for(size_t i{}; i < m_numberOfButtons; i++) {
            m_buttons[i] = new QPushButton;
            m_buttons[i]->setFont(QFont("Helvetica", 24));
            m_buttons[i]->setMinimumSize(m_bttnSizeX, m_bttnSizeY);

            // button styles
            if(i <= 10) {
                // light grey
                m_buttons[i]->setStyleSheet(R"V(
            QPushButton { color: #EBEBEB; background-color: #7B7B7B; border: 1px solid #505050; padding: 0px; }
            QPushButton::pressed { color: #EBEBEB; background-color: #B0B0B0; border: 1px solid #505050; padding: 0px } 
            )V");
            } else if (i > 10 && i <= 17) {
                // dark grey
                m_buttons[i]->setStyleSheet(R"V(
            QPushButton { color: #EBEBEB; background-color: #616161; border: 1px solid #505050; padding: 0px; }
            QPushButton::pressed { color: #EBEBEB; background-color: #7B7B7B; border: 1px solid #505050; padding: 0px } 
            )V");
            } else {
                // orange
                m_buttons[i]->setStyleSheet(R"V(
            QPushButton { color: #EBEBEB; background-color: #F2A23C; border: 1px solid #505050; padding: 0px; }
            QPushButton::pressed { color: #EBEBEB; background-color: #C1802E; border: 1px solid #505050; padding: 0px } 
            )V");
            }

            // adding buttons to gridlayout 
            if (i == 10) {
                // zero button and dot button
                m_buttons[0]->setMinimumSize(m_bttnSizeX*2, m_bttnSizeY);
                m_buttons[0]->setText("0");
                m_buttons[10]->setText(".");
                m_mainGridLayout->addWidget(m_buttons[0], m_gridRows-1, 0, 1, 2, Qt::AlignTop | Qt::AlignHCenter);
                m_mainGridLayout->addWidget(m_buttons[10], m_gridRows-1, 2, Qt::AlignTop | Qt::AlignHCenter);

            } else if(i > 0 && i < 10) {
                // number buttons (1-9)
                int row {};
                int column {};
                row = 4-ceil((static_cast<double>(i) / static_cast<double>(m_gridColumns-1))-1)+1;
                column = (i-1) % (m_gridColumns-1);
                m_mainGridLayout->addWidget(m_buttons[i], row, column, Qt::AlignTop | Qt::AlignHCenter);

                m_buttons[i]->setText(QString::number(i));

            } else if(i > 10 && i < 17) {
                // dark grey auxiliary buttons
                int row = floor((i-11)/(m_gridColumns-1))+1;
                int column = (i-11)%(m_gridColumns-1);
                m_mainGridLayout->addWidget(m_buttons[i], row, column, Qt::AlignTop | Qt::AlignHCenter);

            } else if(i >= 17) {
                // orange math operator buttons
                int row = i-17+1;
                int column = m_gridColumns-1;
                m_mainGridLayout->addWidget(m_buttons[i], row, column, Qt::AlignTop | Qt::AlignHCenter);
            }

            // button object names and signal slot connections
            m_buttons[i]->setObjectName(QString::number(i + m_bttnIdOffset));
            connect(m_buttons[i], SIGNAL(clicked()), this, SLOT(buttonInput()));
        }

        // adding text to buttons
        m_buttons[11]->setText("M");
        m_buttons[12]->setText("M-");
        m_buttons[13]->setText("M+");
        m_buttons[14]->setText("Mc");
        m_buttons[15]->setText("%");
        m_buttons[16]->setText("+/-");
        m_buttons[17]->setText("AC");
        m_buttons[18]->setText("/");
        m_buttons[19]->setText("x");
        m_buttons[20]->setText("-");
        m_buttons[21]->setText("+");
        m_buttons[22]->setText("=");
    }


    void MainWindow::keyPressEvent(QKeyEvent* event) {
        // qDebug() << "Keycode: " << event->key();
        // number keys (no modifiers allowed except numpad number input)
        if(event->modifiers() == Qt::NoModifier 
                || event->modifiers() == Qt::KeypadModifier) {
            switch(event->key()) {
                case Qt::Key_0:
                    m_buttons[Buttons::zero]->animateClick();
                    break;
                case Qt::Key_1:
                    m_buttons[Buttons::one]->animateClick();
                    break;
                case Qt::Key_2:
                    m_buttons[Buttons::two]->animateClick();
                    break;
                case Qt::Key_3:
                    m_buttons[Buttons::three]->animateClick();
                    break;
                case Qt::Key_4:
                    m_buttons[Buttons::four]->animateClick();
                    break;
                case Qt::Key_5:
                    m_buttons[Buttons::five]->animateClick();
                    break;
                case Qt::Key_6:
                    m_buttons[Buttons::six]->animateClick();
                    break;
                case Qt::Key_7:
                    m_buttons[Buttons::seven]->animateClick();
                    break;
                case Qt::Key_8:
                    m_buttons[Buttons::eight]->animateClick();
                    break;
                case Qt::Key_9:
                    m_buttons[Buttons::nine]->animateClick();
                    break;
                default:
                    break;
            }
        }

        // operator shortcut keys
        switch(event->key()) {
            case Qt::Key_Period:
                m_buttons[Buttons::dot]->animateClick();
                break;
            case Qt::Key_Percent:
                m_buttons[Buttons::percentage]->animateClick();
                break;
            case Qt::Key_C:
                m_buttons[Buttons::clear]->animateClick();
                break;
            case Qt::Key_Slash:
                m_buttons[Buttons::divide]->animateClick();
                break;
            case Qt::Key_Asterisk:
                m_buttons[Buttons::multiply]->animateClick();
                break;
            case Qt::Key_Minus:
                if(event->modifiers() == Qt::AltModifier) { 
                    m_buttons[Buttons::signswitch]->animateClick();
                } else {
                    m_buttons[Buttons::subtract]->animateClick();
                }
                break;
            case Qt::Key_Plus:
                m_buttons[Buttons::add]->animateClick();
                break;
            case Qt::Key_Equal:
                m_buttons[Buttons::equal]->animateClick();
                break;
            case Qt::Key_Enter:
                m_buttons[Buttons::equal]->animateClick();
                break;
            case Qt::Key_Return:
                m_buttons[Buttons::equal]->animateClick();
                break;
            default:
                event->ignore();
                break;
        }
    }

    void MainWindow::numberInput(const int& digit) {
        if(m_calcEngine.getCurrent() == "0") {
            m_calcEngine.setCurrentTo(std::to_string(digit));
            return;
        }
        m_calcEngine.appendDigit(digit); 
    }

    void MainWindow::processOperation(Buttons bttn) {
        switch(bttn) {
            case Buttons::dot:
                m_calcEngine.decimalPoint();
                break;
            case Buttons::memget:
                m_calcEngine.loadMem(); 
                break;
            case Buttons::memminus:
                m_calcEngine.memSubtract();
                break;
            case Buttons::memplus:
                m_calcEngine.memAdd();
                break;
            case Buttons::memclear:
                m_calcEngine.clearMem();
                break;
            case Buttons::percentage:
                m_calcEngine.percentage();
                break;
            case Buttons::signswitch:
                m_calcEngine.signSwitch();
                break;
            case Buttons::clear:
                m_calcEngine.clear();
                break;
            case Buttons::divide:
                m_calcEngine.m_currentOperation= m_calcEngine.divide;
                m_calcEngine.switchCurrent();
                break;
            case Buttons::multiply:
                m_calcEngine.m_currentOperation = m_calcEngine.multiply;
                m_calcEngine.switchCurrent();
                break;
            case Buttons::subtract:
                m_calcEngine.m_currentOperation = m_calcEngine.subtract;
                m_calcEngine.switchCurrent();
                break;
            case Buttons::add:
                m_calcEngine.m_currentOperation = m_calcEngine.add;
                m_calcEngine.switchCurrent();
                break;
            case Buttons::equal:
                m_calcEngine.equals();
                break;
            default:
                break;
        }
    }

    /* -- Slot Implementations -- */
    void MainWindow::buttonInput() {
        QPushButton* buttonSender = qobject_cast<QPushButton*>(sender());     // retrieve the button clicked
        QString buttonText = buttonSender->text();                            // retrieve the text from the button clicked
        int code = buttonSender->objectName().toInt() - m_bttnIdOffset;

        if(code >= 0 && code < 10)
            numberInput(code);
        else  
            processOperation(static_cast<Buttons>(code));
    }

    void MainWindow::updateInterface() {
        m_numberDisplay->setText(QString::fromStdString(m_calcEngine.getCurrent()));
    }

} /* EmuCalc namespace */
