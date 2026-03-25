#pragma once
#include "../globals.hpp"

void screenConfirmationUpdate(Screen& currentScreen,
                              bool& existstudent, bool& correctstudent,
                              int& verifyvote);
void screenConfirmationDraw(bool existstudent, bool correctstudent,
                            const std::string& studentName);
