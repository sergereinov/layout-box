# layout-box - simple & abstract layout manager

Простой абстрактный layout manager. _Прототип._

Layout manager это система автоматического размещения элементов внутри других элементов. Каждый из вложенных элементов получает свои координаты и размер таким образом, чтобы вместе с соседями наиболее полезно занять отведенное им пространство. А при изменении размеров окна менеджер следит за тем, чтобы интерфейс оставался удобным.

![TestMFCControls](/images/TestMFCControls-vh.jpg)
<details>
  <summary>Анимация</summary>
  
  ![TestMFCControls](/images/TestMFCControls.gif)
</details>

## Оглавление
- [О проекте](#о-проекте)
- [Сразу к делу - содержание](#содержание)
- [Рекомендации](#рекомендации)
- [Системные требования](#системные-требования)


## О проекте
Простой и абстрактный компоновщик.

**Простой**. Из проекта, по возможности, исключены все лишние сущности и функции, несвойственные основной задаче. Ключевая идея - оставить в проекте только компоновку соседних элементов, вложенных в общий контейнер. Даже координаты размещения в контейнере каждый элемент получает в самый последний момент. Потому что для взаимной компоновки имеет значение только размер элемента и то, сколько места он займет в общем пространстве.  

Если на стержень длиной 10 сантиметров надеть 12 сантиметровых пружинок, то  итоговый размер пружинок не будет зависеть от места на стержне, в котором они окажутся. Размер пружинок зависит только от внутренних свойств каждой их них - от жесткости и от границ максимального сжатия и растяжения (минимального и максимального размеров).
Точные координаты потребуются только тогда, когда появится задача последовательного линейного размещения в координатах. То есть, при таком подходе, даже физический смысл координатных осей и их количество не имеют значения до самого последнего момента - наделения физическим смыслом всей системы в целом.  

**Абстрактный**, таким образом, потому что сам менеджер не привязан ни к какой-то конкретной координатной системе, ни к платформе, ни к какому-либо GUI-фреймворку.

Весь layout-box опирается всего на один метод
```c++
void Box::ArrangeItems(std::vector<BoxItem *> &items, … , int dimensionIndex);
```
Реализуемый внутри алгоритм получает на вход, строго говоря, список допустимых длин и общую длину отрезка по заданной координате. Для каждого элемента вычисляется такая длина, чтобы она была в границах его минимума и максимума, а сумма всех длин была точно равна общей длине контейнера `Box` по заданной координате.

Не сложно пересобрать и расширить layout-box до количества измерений больше двух. И, в той же парадигме, управлять распределением длин внутри, например, трехмерного куба в четырехмерном пространстве. Для этого надо переопределить `LAYOUT_POINT_DIMENSIONS_COUNT` (см. layout-types.h) и добавить класс-компоновщик, умеющий использовать дополнительные измерения и способный хранить внутри себя вложенные элементы по рабочим измерениям. Например:
```c++
class D3CubeBox : public Box { /* … */ };
```

Несмотря на общую идею абстрактности, кое-где “срезаются углы” в сторону более простой конкретики, чтобы сохранить понимание кода на бытовом уровне. В духе “все контролы размещаются внутри 2D-окна, согласно своим минимально и максимально допустимым размерам”.

![TestDrawDlg](/images/TestDrawDlg-h.jpg)
<details>
  <summary>Анимация</summary>
  
  ![TestDrawDlg](/images/TestDrawDlg.gif)
</details>
<br/>

Менеджер layout-box предлагает несколько готовых классов-компоновщиков:
- `HBox` - горизонтальное размещение элементов
- `VBox` - вертикальное размещение элементов
- `GridBox` - размещение элементов в виде двумерной таблицы
- `StackedBox` - все элементы размещаются поверх друг друга в общем пространстве

Как и в других менеджерах компоновки, классы-компоновщики могут быть добавлены в качестве элемента в другие классы-компоновщики.

Все пользовательские элементы должны наследоваться от класса `BoxItem` и реализовать хотя бы один виртуальный метод:
```c++
virtual void InvalidateAt(const Point &offset);
```
Через этот метод компоновщик сообщает элементу координаты, в которых он должен себя разместить. Размер элемента на данном этапе уже задан компоновщиком и доступен потомку `BoxItem` как `this->m_area`.

Для управления размерами всего поля компоновки и размерами каждого элемента есть только три Size-параметра: точный размер, минимальный размер, максимальный размер. (У элементов и у некоторых контейнеров есть еще один многомерный параметр "вес" для управления пропорциями занимаемого пространства.)  

В идею менеджера layout-box заложено всего два строгих правила:
- **Точный размер можно задать только самому внешнему компоновщику.** Все вложенные элементы и другие вложенные компоновщики получат свои размеры автоматически через алгоритм соотнесения размеров по внутренним свойствам элементов (вес/пропорция, минимум и максимум). В любом случае, размеры всех вложенных элементов **будут пересчитаны** при вызове `UpdateGeometry()` самого внешнего класса-компоновщика.
- **Параметры минимальный размер и максимальный размер могут быть заданы только для размещаемых пользовательских элементов.** Все минимальные и максимальные размеры компоновщиков (если они содержат элементы, то есть не пустые), вплоть до самого внешнего, **будут пересчитаны** при вызове `UpdateMinMax()` самого внешнего компоновщика.

Для лучшего понимания работы layout-box следует всегда помнить два этих правила.
1. Точный размер всегда задается сверху вниз. Например, только для главного контейнера по размерам окна.
2. Минимум и максимум может быть только у размещаемого элемента. Например, у Label, Text, Button и т.п.


## Содержание


- **layout-box** - основной исходный код layout-box
- **mfc** - демонстрационный враппер MFC CDialog с подключением dialog view для управления MFC-контролами с простейшим viewmodel-биндингом.
- **test-layout** - демо MSVS-проект и все примеры.
- **tinyxml** - подключенный в качестве модуля сторонний git-проект tinyxml2 https://github.com/leethomason/tinyxml2


## Рекомендации

Существует много менеджеров компоновки с разнообразным функционалом и на разных платформах.
Если нет каких-то особых причин подключать именно этот прототип и если есть возможность использовать типовые фреймворки / встроенные технологии компоновки (например, HTML, QML/QLayout, Tkinter, wxFormBuilder, C# Visual Designer и т.д.), тогда лучше не изобретать велосипед и воспользоваться какой-то стандартной технологией.

Этот проект - прототип. На данный момент он существует как концепция.
И хотя layout-box вполне работоспособен, приемлемо быстр и даже может показаться удобным - в нем остается еще много ограничений. Кое-где сильно "срезаются углы" ради упрощения картины в целом.

## Системные требования

Исходный код проверялся под Visual Studio 2008. Используется C++98/2003.  
Зависимости:
- для layout-box: только std-контейнеры такие как `std::set`, `std::vector`, `std::deque`
- для layout-box/view, library и xml-reader: `std::map`, `std::string`, tinyxml2
- для mfc, ViewDialog: MFC, Windows

В явном виде механизм исключений не используется. Исключения могут быть сгенерированы std-контейнерами или другими сторонними библиотеками.

Лицензия на авторский код MIT. Сторонний код и стандартные библиотеки существуют под своими лицензиями.

Проект дополняется.
  

