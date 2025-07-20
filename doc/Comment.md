
## 1. **文件级别注释**
文件的整体描述，包括作者、日期和功能概述。

```cpp
/**
 * @file my_class.h
 * @brief This file contains the declaration of the MyClass class.
 * 
 * Detailed description of what the file contains and its purpose.
 * 
 * @author Your Name
 * @date 2024-06-17
 */
```

---

## 2. **类级别注释**
描述类的用途。

```cpp
/**
 * @class MyClass
 * @brief This class provides an example of Doxygen documentation.
 *
 * Detailed description of the class, its behavior, and purpose.
 */
class MyClass
{
public:
    /**
     * @brief Default constructor for MyClass.
     */
    MyClass();

    /**
     * @brief Constructor with parameters.
     * 
     * @param value An integer value to initialize the object.
     */
    MyClass(int value);

    /**
     * @brief Destroys the MyClass object.
     */
    ~MyClass();

    /**
     * @brief Sets a value.
     * 
     * @param value An integer to set.
     * @return void
     */
    void setValue(int value);

    /**
     * @brief Gets the stored value.
     * 
     * @return The current stored integer value.
     */
    int getValue() const;

private:
    int m_value; ///< The stored integer value.
};
```

---

## 3. **函数级别注释**
详细描述函数的输入、输出和功能。

```cpp
/**
 * @brief Adds two integers and returns their sum.
 * 
 * This function takes two integers as inputs, adds them, and returns the result.
 * 
 * @param a The first integer.
 * @param b The second integer.
 * @return The sum of a and b.
 */
int add(int a, int b)
{
    return a + b;
}
```

---

## 4. **变量级别注释**
对类或全局变量进行注释。

```cpp
int globalVar; ///< A global variable used for demonstration.

class MyClass
{
private:
    int m_count; ///< Keeps track of the count of operations.
};
```

---

## 5. **枚举和枚举值注释**
注释枚举类型和其中的值。

```cpp
/**
 * @enum Status
 * @brief Represents the status of an operation.
 */
enum Status
{
    SUCCESS = 0, ///< The operation was successful.
    FAILURE = 1, ///< The operation failed.
    PENDING = 2  ///< The operation is pending.
};
```

---

## 6. **宏注释**
注释宏定义或预处理器常量。

```cpp
/**
 * @def PI
 * @brief The value of PI (π), approximated to 3.14159.
 */
#define PI 3.14159
```

---

## 7. **模板类或函数注释**
注释模板类或函数。

```cpp
/**
 * @class TemplateClass
 * @brief A templated class example.
 *
 * @tparam T The type parameter for the class.
 */
template <typename T>
class TemplateClass
{
public:
    /**
     * @brief Prints the value.
     * @param value The value of type T to print.
     */
    void printValue(const T& value);
};
```

---

## 8. **示例代码**
在文档中插入代码示例，便于读者理解用法。

```cpp
/**
 * @brief Demonstrates the use of add function.
 * 
 * Example usage:
 * @code
 * int result = add(2, 3);
 * std::cout << "Result: " << result; // Outputs 5
 * @endcode
 */
```

---

## 9. **TODO 标记**
添加未完成任务的标记。

```cpp
/**
 * @todo Add error handling for edge cases.
 */
void someFunction();
```

---

## 10. **分组标签**
使用分组组织文档结构。

```cpp
/** 
 * @defgroup MathFunctions Mathematical Functions 
 * @brief Group of mathematical utility functions.
 * @{
 */

/**
 * @brief Multiplies two integers.
 * @param a First integer.
 * @param b Second integer.
 * @return The product of a and b.
 */
int multiply(int a, int b);

/** @} */ // End of group MathFunctions
```

---

## 常用 Doxygen 标签汇总

| 标签        | 说明                                  |
|-------------|---------------------------------------|
| `@file`     | 文件的简要描述                        |
| `@brief`    | 简要描述函数、类、变量等              |
| `@param`    | 描述函数参数                          |
| `@return`   | 描述函数的返回值                      |
| `@author`   | 作者                                  |
| `@date`     | 日期                                  |
| `@tparam`   | 模板参数                              |
| `@todo`     | 未完成任务                            |
| `@see`      | 引用其他文档或函数                    |
| `@code` ... `@endcode` | 插入代码示例              |
| `@note`     | 提示或补充说明                        |
| `@warning`  | 警告信息                              |
| `@defgroup` | 定义一个分组                          |
