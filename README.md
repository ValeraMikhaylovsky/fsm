# Автомат Конечных Состояний

`ecpp::fsm` это библиотека автомата конечных состояний на C++20 спроектированная для работы в однопоточной среде.

## Мотивация

Библиотека `ecpp::fsm` является сильно упрощённой версией библитеки [`::afsm`](https://github.com/zmij/afsm) 
и реализована таким образом, что бы максимально близко повторять синтаксис `::afsm`.
Библиотеку `ecpp::fsm` собирает любой компилятор поддерживающий С++20, в то время как `::afsm` не собирается компиляторами
Microsoft Visual Studio и MinGW.

## Возможности

* Табличное представление
  * Переходы задаются в таблице
  * Можно указывать действия при переходе между состояниями
  * Можно указывать условия перехода в следующее состояние
  * Поддержка предикатов условия перехода `not`, `or` и `and` и их комбинирование
  * Табличное представление внутренней таблицы действий по событиям внутри состояния
* Проверка типов на этапе компиляции
* Относительно быстрая компиляция
* Отсутствие внешних зависимостей

## Краткий обзор

Представление UML-диаграммы тривиального конечного автомата и исходного кода, с которым он сопоставлен.
![minimal](doc/simple_fsm.png)

```c++
#include <cassert>
#include <fsm/fsm_ecpp.h>

using namespace ecpp::fsm;

// Events
struct start {};
struct stop {};

// State machine definition
struct minimal_def {
//@{
/** @name States */
struct initial      : state<initial> {};
struct running      : state<running> {};
struct terminated   : state<terminated> {};
//@}

using initial_state = initial;
using transitions   = transition_table
  <
    /*  State       Event       Next        */
    tr< initial,    start,      running     >,
    tr< running,    stop,       terminated  >
  >;
};

// State machine object
using minimal = ecpp::fsm::state_machine<minimal_def>;

void use()
{
    minimal fsm;
    assert(fsm.is_in_state<minimal_def::initial>());
    assert(fsm.process_event(start{}) == event_result::done);
    assert(fsm.is_in_state<minimal_def::running>());
    assert(fsm.process_event(stop{})  == event_result::done);
    assert(fsm.is_in_state<minimal_def::terminated>());
}
```

## License

[MIT License](LICENSE)
