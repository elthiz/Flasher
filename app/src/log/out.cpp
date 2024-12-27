#include "out.h"

#include <iostream>

void print(const std::string& message)
{
    std::cout << message << std::endl;
}

void printProgressBar(int currentIteration, const std::string& prefix, int totalIterations) {
    const int barWidth = 20;  // Длина прогресс-бара (в символах)

    // Вычисляем процент выполнения
    int percentage = (currentIteration * 100) / totalIterations;

    // Вычисляем количество символов, которые должны быть заполнены
    int filledWidth = (percentage * barWidth) / 100;

    // Выводим префикс
    std::cout << "\t\r" << prefix << ":\t [";  // Выводим префикс и начало прогресс-бара

    // Строим прогресс-бар
    for (int i = 0; i < barWidth; ++i) {
        if (i < filledWidth) {
            std::cout << "\033[32m#\033[0m";  // Заполняем бар
        } else {
            std::cout << ".";  // Пустое пространство
        }
    }

    // Выводим процент выполнения
    std::cout << "] \033[32m" << percentage << "%\033[0m";
    std::cout.flush(); // Немедленно выводим изменения на экран
}