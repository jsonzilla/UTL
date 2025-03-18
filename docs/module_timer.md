[<img src ="images/icon_cpp_std_17.svg">](https://en.wikipedia.org/wiki/C%2B%2B#Standardization)
[<img src ="images/icon_license_mit.svg">](./LICENSE.md)
[<img src ="images/icon_header_only.svg">](https://en.wikipedia.org/wiki/Header-only)
[<img src ="images/icon_no_dependencies.svg">](https://github.com/DmitriBogdanov/UTL/tree/master/single_include)

# utl::timer **(deprecated)**

[<- to README.md](..)

[<- to implementation.hpp](https://github.com/DmitriBogdanov/UTL/blob/master/include/UTL/timer.hpp)

**utl::timer** module implements a very concise global-state timer.

Useful to quickly time code segments without all the boilerplate of [`<chrono>`](https://en.cppreference.com/w/cpp/chrono).

> [!Important]
>
> This module was **deprecated** in favor of `utl::time`. It will currently remain available for compatibility reasons but may be removed at any point of time in the future.

## Definitions

```cpp
// Time measurement
void start() noexcept;

double elapsed_ms()    noexcept;
double elapsed_sec()   noexcept;
double elapsed_min()   noexcept;
double elapsed_hours() noexcept;

std::string elapsed_string_ms();
std::string elapsed_string_sec();
std::string elapsed_string_min();
std::string elapsed_string_hours();

std::string elapsed_string_fullform();

// Local datetime
std::string datetime_string();
std::string datetime_string_id();
```

## Methods

### Time measurement

> ```cpp
> void start() noexcept;
> ```

Sets internal start timepoint for elapsed measurements.

> ```cpp
> double elapsed_ms()    noexcept;
> double elapsed_sec()   noexcept;
> double elapsed_min()   noexcept;
> double elapsed_hours() noexcept;
> ```

Returns elapsed time as `double`. Internally time is measured in nanoseconds.

> ```cpp
> std::string elapsed_string_ms();
> std::string elapsed_string_sec();
> std::string elapsed_string_min();
> std::string elapsed_string_hours();
> ```

Returns elapsed time as `std::string` with units.

> ```cpp
> std::string elapsed_string_fullform();
> ```

Returns elapsed time in format `%H hours %M min %S sec %MS ms`.

### Datetime

> ```cpp
> std::string datetime_string();
> std::string datetime_string_id();
> ```

Returns current local date and time in format `%y-%m-%d %H:%M:%S` or `%y-%m-%d-%H-%M-%S`.

**Note:** First format looks better, but contains characters illegal in filenames, which is why a second filename-compatible format is provided.

## Examples

### Measuring time

[ [Run this code](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:2,endLineNumber:14,positionColumn:2,positionLineNumber:14,selectionStartColumn:2,selectionStartLineNumber:14,startColumn:2,startLineNumber:14),source:'%23include+%3Chttps://raw.githubusercontent.com/DmitriBogdanov/UTL/master/single_include/UTL.hpp%3E%0A%0Aint+main()+%7B%0A++++using+namespace+utl%3B%0A%0A++++timer::start()%3B%0A%0A++++std::this_thread::sleep_for(std::chrono::milliseconds(3700))%3B%0A%0A++++std::cout%0A++++++++%3C%3C+%22Time+elapsed+during+sleep_for(3700+ms):%22+%3C%3C+!'%5Cn!'%0A++++++++%3C%3C+timer::elapsed_string_sec()+++++++++++++++%3C%3C+!'%5Cn!'%0A++++++++%3C%3C+timer::elapsed_string_fullform()++++++++++%3C%3C+!'%5Cn!'%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:71.71783148269105,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:clang1600,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B17+-O2',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+16.0.0+(Editor+%231)',t:'0')),header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+clang+16.0.0',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+clang+16.0.0+(Compiler+%231)',t:'0')),k:46.69421860597116,l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:28.282168517308946,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),version:4) ]

```cpp
using namespace utl;

timer::start();

std::this_thread::sleep_for(std::chrono::milliseconds(3700));

std::cout
    << "Time elapsed during sleep_for(3700 ms):" << '\n'
    << timer::elapsed_string_sec()               << '\n'
    << timer::elapsed_string_fullform()          << '\n';
```

Output:
```
Time elapsed during sleep_for(3700 ms):
3.711095 sec
0 hours 0 min 3 sec 712 ms
```

### Getting local datetime

[ [Run this code](https://godbolt.org/#g:!((g:!((g:!((h:codeEditor,i:(filename:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,selection:(endColumn:5,endLineNumber:5,positionColumn:5,positionLineNumber:5,selectionStartColumn:5,selectionStartLineNumber:5,startColumn:5,startLineNumber:5),source:'%23include+%3Chttps://raw.githubusercontent.com/DmitriBogdanov/UTL/master/single_include/UTL.hpp%3E%0A%0Aint+main()+%7B%0A++++using+namespace+utl%3B%0A++++%0A++++std::cout+%3C%3C+%22Current+time+is:+%22+%3C%3C+timer::datetime_string()+%3C%3C+!'%5Cn!'%3B%0A%7D%0A'),l:'5',n:'0',o:'C%2B%2B+source+%231',t:'0')),k:71.71783148269105,l:'4',n:'0',o:'',s:0,t:'0'),(g:!((g:!((h:compiler,i:(compiler:clang1600,filters:(b:'0',binary:'1',binaryObject:'1',commentOnly:'0',debugCalls:'1',demangle:'0',directives:'0',execute:'0',intel:'0',libraryCode:'0',trim:'1',verboseDemangling:'0'),flagsViewOpen:'1',fontScale:14,fontUsePx:'0',j:1,lang:c%2B%2B,libs:!(),options:'-std%3Dc%2B%2B17+-O2',overrides:!(),selection:(endColumn:1,endLineNumber:1,positionColumn:1,positionLineNumber:1,selectionStartColumn:1,selectionStartLineNumber:1,startColumn:1,startLineNumber:1),source:1),l:'5',n:'0',o:'+x86-64+clang+16.0.0+(Editor+%231)',t:'0')),header:(),l:'4',m:50,n:'0',o:'',s:0,t:'0'),(g:!((h:output,i:(compilerName:'x86-64+clang+16.0.0',editorid:1,fontScale:14,fontUsePx:'0',j:1,wrap:'1'),l:'5',n:'0',o:'Output+of+x86-64+clang+16.0.0+(Compiler+%231)',t:'0')),k:46.69421860597116,l:'4',m:50,n:'0',o:'',s:0,t:'0')),k:28.282168517308946,l:'3',n:'0',o:'',t:'0')),l:'2',n:'0',o:'',t:'0')),version:4) ]

```cpp
using namespace utl;

std::cout << "Current time is: " << timer::datetime_string() << '\n';
```

Output:
```
Current time is: 2023-12-05 02:11:34
```