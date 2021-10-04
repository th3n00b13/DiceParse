# DiceParse

## **What is this?**

It's a garbage

probably terrible code as we know

Anyway, this is an expression parser but only simple arithmetics and ..also rolling dice digitally.
The used algorithm is the Shunting-yard algorithm because my skill is not enough to make a higher one.

This thing was made because I just wanted to fully rewrite a bot that just rolls some dices.

## How to compile this

You can do is only run this program and input some formula and watch answers, if you don't modify source codes.
Compiling this requires C++20 and probably only it is

For reference, I use MSYS2 + gcc 10.3.0, and always compiling with `g++ -std=c++20 DiceParse.cpp -o DiceParse.exe` (or `g++ -fPIC -static -lstdc++ -lgcc -std=c++20 DiceParse.cpp -o DiceParse.exe` to make it able to run with Windows)

## How to use

When you executed the application, a console will be shown

And...

insert your formula **(without space because the program won't recognize well)**

enter

program shows answer

if you want to exit, just type exit and press enter, and everything is done

Be careful, this program **ONLY takes simple formulas: functions and some operators, even variables are unavailable**! (Only allows `+`(Add), `-`(Sub), `*`(Mul), `/`(Div), `(`, `)`(Brackets), and `D` )

Also, a(b+c) will be parsed be like: a*(b+c)

Functions:

```cpp
double Parse(std::basic_string<char16_t> Formula)
```

Calculate the formula and returns a value

On this program, Operator `xDy` (`D`) ~~(which is not even operator)~~ does the job: roll `1~y` dice `x` times and returns sum value
