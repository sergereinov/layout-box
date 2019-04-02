# Table of content

- [Inheritance map](#inheritance-map)
- [Composition map](#composition-map-essential-things)

# Inheritance map
<details>
  <summary>Click to show</summary>
  
### layout-object.h
```c++
namespace layout {

  class LinkedObject

}
```

### layout-types.h/cpp

```c++
namespace layout {

  class Point
    class Size : public Point
    
  class Span
  class Margins
  
  class Area
    class Item : public Area
    
}
```

### layout-box.h/cpp

```c++
#include "layout-object.h"
#include "layout-types.h"

namespace layout {

  class BoxItem : public Item, public LinkedObject //base class for all layout items

    class Box : public BoxItem //abstract base class for all containers except StackedBox
      class HBox : public Box
        class VBox : public HBox
        
      class GridBox : public Box
    
  class StackedBox : public BoxItem

}
```
</details>

# Composition map (essential things)

### layout-types.h/cpp
<details>
  <summary>Click to show</summary>

```c++

#define LAYOUT_POINT_DIMENSIONS_COUNT 2

namespace layout {

  class Point
  {
  public:
    static const int DimensionsCount = LAYOUT_POINT_DIMENSIONS_COUNT; // axes count definition
    
    static const int X_Dim = 0;   // x-axis index
    static const int Y_Dim = 1;   // y-axis index
#if LAYOUT_POINT_DIMENSIONS_COUNT > 2
    static const int Z_Dim = 2;   // z-axis index (if defined)
    //and so on..
#endif

    static const double Maximum;  // = 1e302 (something big enough)

    double D[DimensionsCount];    // storage for x-, y-, z-value (and so on)
    
    //... helpers
  };


  class Size : public Point
  {
    //... all that inherited from Point + helpers
  };


  class Span
  {
  public:
    size_t S[Point::DimensionsCount];
    
    //... other
  };


  class Margins
  {
  public:
    Point LeftTop;      //margins near zero-point (near x0, y0, z0 etc.)
    Point RightBottom;  //margins near size-point (near sx, sy, sz etc.)
    
    //... helpers
  };
  
  
  class Area
  {
    //... methods & helpers
    
  protected:
    Size m_area;        //size of the area
    Size m_weight;      //relative weight factor for this area
    Span m_span;        //value of coverage of this area across (columns, rows, etc.) of the parent container
    
    Margins m_margins;  //area margins helper (not used by layout mechanics)

    Size m_minArea;     //minimum allowed area size
    Size m_maxArea;     //maximum allowed area size
  };
  
  
  class Item : public Area
  {
  public:

#ifdef _DEBUG
    int ItemDebugID; //items debug stuff (helps to select/debug the particular item)
    Item() : ItemDebugID(0) {}
#endif
  };
  
}
```
</details>


### layout-box.h/cpp
<details>
  <summary>Click to show</summary>

```c++
#include "layout-object.h"
#include "layout-types.h"

namespace layout {

  class BoxItem : public Item, public LinkedObject
  {
    //... virtual methods with default behaviour
  };
  
  
  class Box : public BoxItem
  {
    // ArrangeItems (main method of layouting everything around)
    // + virtual interfaces for ArrangeItems
  };


  class HBox : public Box
  {
    //... virtual methods from BoxItem class
    
  protected:    
    std::vector<BoxItem *> m_Items; //array of pointers to nested items
    
    int m_mainDimension;            //index of main axis - x for horizontal box container
    int m_secondDimension;          //index of secondary axis - y for horizontal box container
    
    //... other
  };
  
  
  class VBox : public HBox
  {
    // swapped main and secondary axes indices
    //... all other inherits from HBox class
  };
 
 
  class GridBox : public Box
  {
  public:
    static const int MaxRowColumnsCount = 100; //speculative constraints for UI
    
    //... methods and other stuff
    
  protected:
    std::vector<BoxItem *> m_Items; //linear container for all nested items (length = rows * columns)
    size_t m_rowsCount;             //items rows count
    size_t m_columnsCount;          //items columns count
    
    std::vector<BoxItem *> m_Ruler; //diagonal ruler:
                                    //  'x' is column-side ruler & 'y' is row-side ruler
                                    //  ruler.size = max(x,y)
                                    //actually this ruler used for layouting entire grid
                                    //next all items takes their sizes from corresponding ruler's x- or y-value
    
    int m_rowDimension;             //index of row axis (y by default)
    int m_columnDimension;          //index of column axis (x by default)
  
    //... other
  };
  
  
  class StackedBox : public BoxItem  
  {
    // the most simplified container that places all elements on top of others in a common space
    
  protected:
    std::vector<BoxItem *> m_Items; //array of pointers to nested items
  };
  
}

```
</details>

