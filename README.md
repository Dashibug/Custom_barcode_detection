# Custom 1D Barcode Detection (Hough Transform)

Курсовой проект по Computer Vision: инженерный метод детектирования штрих кодов c использованием преобразования Хафа.
В репозитории также есть лабораторные (`prj.lab`), но основной проект — в папке `prj.cw/misis2024s`.
> Репозиторий содержит:
> - `prj.cw/misis2024s` — course project
> - `prj.lab` — лабораторные/учебные задания

<img width="553" height="383" alt="image" src="https://github.com/user-attachments/assets/3e4c7379-78d5-4647-904a-c70b79f1b731" />

## Данные

Используется набор изображений, снятых в разных условиях и под различными углами.  
Для каждого изображения подготовлена **карта бинарной сегментации** (ground truth), которая помогает локализовать область поиска.

## Алгоритм решения

### 1) Предобработка и карта границ
1. `BGR → Grayscale`
2. медианная фильтрация (снижение шума)
3. автоматический подбор порогов Canny на основе медианы интенсивностей и коэффициента `sigma`
4. `Canny` → edge map

Дополнительно формируется визуализация “оригинал + edge map”.

### 2) Детектирование линий Хафом
На маскированной edge map применяется `cv::HoughLinesP`. Параметры:
rho = 1 (пиксели)
theta = 1°
threshold = 50
minLineLength = 50
maxLineGap = 10
Все найденные линии рисуются на:
- исходном изображении
- детектированном изображении (маска/промежуточный результат)
### 3) Прямоугольник штрих-кода

Если линии найдены, собираются все концы линий и строится минимальный прямоугольник

### 4) Сравнение с “идеальным” прямоугольником

Из parsed_data.json загружается разметка (VIA), и на изображение наносится “идеальный” прямоугольник (белым).
Это позволяет визуально сравнить результат детектора с разметкой.

<img width="572" height="240" alt="image" src="https://github.com/user-attachments/assets/fbd56ecc-b2df-4da2-906a-2f1d2273a0a4" />


## Build
Реализовано два удобных способа сборки проекта: через IDE (Visual Studio / CLion) или через командную строку.

### Requirements
- CMake 3.15+
- C++ compiler with C++17 support
- OpenCV (dev package)

### Build (macOS / Linux)
```bash
git clone https://github.com/Dashibug/Custom_barcode_detection.git
cd Custom_barcode_detection

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
```
### Build (Windows)
```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```
После сборки бинарники будут в одной из папок:
- build/Release/
- или build/<target_name>.exe (зависит от генератора)

<img width="567" height="241" alt="image" src="https://github.com/user-attachments/assets/5c9ffe94-2a3f-409e-8425-e8eb0403ce2d" />



