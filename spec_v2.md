# Literals

## Numerals

1
1.2
6.022e23

1u64
1u32
1u16
1u8

1i64
1i32
1i16
1i8

1f64
1f32
1f16
1f8

## Binary expressions

## Arithmetic

+
-
*
/

### Comparisons
>
<
>=
<=

### Equality

==
!=

### Boolean

&&
||
!

### Bit

&
|
^

## Unary operators

-
++expression
--expression
expression++
expression--

## Booleans

true
false

## Strings

"Hello"

## String interpolation

"Hello $hello!"

## Structs

{ foo: 1u32, bar: 0u32 }

## Arrays

[ 1u32, 0u32 ]

## Constant declaration

const foo: u32 = 0u32;
const foo: bool = true;

## Variable declaration

var foo: u32 = 0;

## Variable assignment

foo = 3
foo = 5

## Types

u64
u32
u16
u8
i64
i32
i16
i8
f64
f32
f16
f8
string
bool
void

struct
array
tuple

u8[]
string[]
(u32, u32)[]
rgb[]
vector[][]

type Bytes = u8[]

struct Position { x: u32, y: u32 }

enum Weekdays = "Monday" | "Tuesday" | "Wednesday" | "Thursday" | "Friday" | "Saturday" | "Sunday";

union Maybe<T> = Some(T) | None;

union Result<T, E> = Result(T) | Error(E);

## Struct access

bar = foo.bar

## If else

if (expression) {
    ...;
    ...;
} else if (expression) {
    ...;
    ...;
} else {
    ...;
    ...;
}

## Pattern matching

result = match(expression)
    | Some(x) => Some(x * 2)
    | None => None;

## Function expression

(arg1: u32, arg2: u32) => arg1 + arg2

(arg1: u32, arg2: u32): u32 =>  {
    return arg1 + arg2
}

(arg1: u32, arg2: u32) => {
    bar: str = 123
    return bar + arg1 + arg2
}

## Function Declaration

pub fn main(): void {

}

fn foo(): void {

}

## Call expression

const fn = (x: u8, y: u8): u8 => x + y;

const result = fn(x: 3, y: 7)
const result = fn(3, 7)

## Loops

for (var i = 0; i < 10; i++) {

}

while (expression) {
    continue;
    break;
}

for pattern of expression {
    // Do something
}

## Pipes

10 |> double($$) |> "Hello $$" |> println($$)

Becomes:

var $$;
$$ = 10;
$$ = double($$);
$$ = "Hello $$";
$$ = println($$);

## Namespaces

namespace numbers {
    fn max(x: u8, y: u8): u8 {
        if (x > y) {
            return x;
        } else {
            return y;
        }
    }
}

numbers.max(10, 12);

using numbers {
    max(10, 12);
};

## Promises

union Promise<T, E> = Awaitable((T) => void) | Result(T) | Error(E);

fn then<T1, T2, E>(p: Promise<T1, E>, map: T1 => T2): Promise<T2, E> {
    return match(p)
        | Error(e) => p
        | Result(r) => Result(map(r))
        | Awaitable(callback) => {

        }
}
