# StylesManager

Manager for https://github.com/twri/sdxl_prompt_styler so you can easily once opened modify each one.
Also allows you to import CSV (styles.csv) from Automatic1111 and then save into a json file formatted for it



## Building the Project

### Prerequisites
- CMake 3.31 or higher
- C++23 compatible compiler
- Qt 6.9.x

### Configuration

The project can be configured in several ways:

1. **Environment Variable**:
   Set the `QT_DIR` environment variable to your Qt installation path:
   ```bash
   # Windows
   set QT_DIR=C:\Qt\6.9.1\msvc2022_64
   
   # Linux/macOS
   export QT_DIR=/path/to/qt
   ```

2. **CMake Command Line**:
   ```bash
   cmake -DQT_DIR=/path/to/qt ..
   ```

3. **CMake GUI**:
    - Open CMake GUI
    - Set the QT_DIR variable to your Qt installation path
    - Configure and Generate

### Building
```bash
cmake -B build
cmake --build build
```