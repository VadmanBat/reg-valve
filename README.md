# RegValve

Desktop-приложение (Qt 6) для анализа передаточных функций и ручной настройки регулятора.  
Математика — внешняя библиотека **[numina](https://github.com/VadmanBat/numina)**; UI — Qt Designer (`.ui`).

## Зависимости

- CMake ≥ 3.28, C++23
- Qt 6 (Widgets, Charts) — по умолчанию MSYS2 UCRT64: `C:/msys64/ucrt64`
- numina по пути `C:/cpp/projects/static-libs/numina` (или `-DNUMINA_ROOT=...`)

## Сборка

```bash
# toolchain MSYS2 UCRT64 в PATH (g++, ninja)
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release ^
  -DCMAKE_PREFIX_PATH=C:/msys64/ucrt64 ^
  -DQt6_DIR=C:/msys64/ucrt64/lib/cmake/Qt6
cmake --build build
```

В CLion: kit с компилятором `C:/msys64/ucrt64/bin/g++.exe`, CMake option  
`-DCMAKE_PREFIX_PATH=C:/msys64/ucrt64` (или уже подхватится из `CMakeLists.txt`).

**PATH:** для `moc`/`uic` CMake ставит обёртки (`cmake/msys-qt-env.cmake`), чтобы не было  
`0xC0000135` (нет DLL). Дополнительно можно в CLion → CMake profile → Environment:  
`PATH=C:\msys64\ucrt64\bin;%PATH%`.

Рабочий каталог для запуска: рядом с exe должны быть (или пути из `data/`):

- `data/fonts/...`
- `data/styles/button-style.qss` (или `styles/button-style.qss`)

## Структура

```
ui/                 # .ui для Designer
code/app/           # MainWindow
code/tabs/          # Exp / Num / Reg
code/dialogs/       # диалоги
code/widgets/       # TranFuncForm, RegParameter, RegulationWidget, DoubleSlider
code/series/        # Series, ComplexSeries, SetSeries
code/charts/        # chart_utils
code/control/       # фабрика P…PID (только здесь)
code/util/          # tf_builder → API numina
```

Редактирование UI: открыть `ui/**/*.ui` в Qt Designer.

## Вкладки

1. **Идентификация** — файл h(t) или (t, u, y) → Simoyu / Дюамель+Simoyu; сравнение «Эксперимент» / «Модель»
2. **Анализ** — ввод W(p), переходная/КЧХ, показатели качества объекта
3. **Синтез** — ручная настройка регулятора, замкнутый контур

## License

См. `LICENSE`.
