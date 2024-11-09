# Literals

## Numerals

```deet
1
1.2
6.022e23

0x123
0o123
0b101

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
```

## Arithmetic

```deet
+
-
*
/
```

## Comparisons

```deet
>
<
>=
<=
```

## Equality

```deet
==
!=
```

### Boolean

```deet
&&
||
!
```

### Bit

```deet
&
|
^
```

## Unary operators

```deet
-
++expression
--expression
expression++
expression--
```

## Booleans

```deet
true
false
```

## Strings

```deet
"Hello"
```

## String interpolation

```deet
"Hello ${hello}!"
```

## Structs declaration

```deet
struct Person {
    name: String;
    age: u32;
};
```

## Structs expressions

```deet
{ foo: 1u32, bar: 0u32 }
Person { name: "Chal", age: 35 }
```

## Arrays

```deet
[ 1u32, 0u32 ]
```

## Constant declaration

```deet
const foo: u32 = 0u32;
const foo: bool = true;
```

## Variable declaration

```deet
var foo: u32 = 0;
```

## Variable assignment

```deet
foo = 3;
foo = 5;
```

## Types

```deet
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

u8[]
string[]
rgb[]
vector[][]

type Bytes = u8[]

struct Position { x: u32, y: u32 }

union Weekdays = Monday | Tuesday | Wednesday | Thursday | Friday

union Maybe<T> = Some(T) | None

```

## Struct access

```deet
bar = foo.bar;
bar = foo.bar;
```

## If else

```deet
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
```

## Pattern matching

```deet
result = match(literal)| true => 1 | false => 0;


result = match(union_instance)
    | Some(t) => Some(cb(t))
    | None => None;
```

## Function expression

```deet
(arg1: u32, arg2: u32) => arg1 + arg2

(arg1: u32, arg2: u32): u32 =>  {
    return arg1 + arg2
}

(arg1: u32, arg2: u32) => {
    bar: str = 123
    return bar + arg1 + arg2
}
```

## Function Declaration

```deet
// Public to other modules
pub fn main() {

}

// Private to the module
fn other_function(): u8 {
    return 10;
}
```

## Algebraic effects

```deet

effect @malloc = (struct: NameOfStruct) => StructPointer;

fn make_person(): @malloc {
    const person = Person {
        name: "Chal",
        age: 35,
    };
}

fn main()  {
    const person = malloc(Person { name: "Chal", age: 35 });
    log(person);
    free(person);
}

effect @raise<T> = (error: String) => T;

fn nullthrows(x: Maybe<u8>) @raise<u8> u8 {
    if (x = Some(n)) {
        return n;
    } else {
        return @raise "Number is null";
    }
}

fn main(x: Maybe<u8>): u8 {
    return nullthrows(x)
        with @raise: (x: u8) => 0;
}


effect @yield<T> = (iterator: Iterator<T>) => T;

fn generate_numbers() @yield<u8> void {
    @yield 1;
    @yield 2;
    @yield 3;
}

fn to_array<T>(generator: @yield<T> void) {
    const result = [];

    generate_numbers()
        with @yield<T> (t: T) => {
            result.push(t);
            resume;
        }

}

fn main() {
    const numbers = to_array(generate_numbers());
}


effect @await = <T>(callback: (done: () => void) => void) => T;

fn async_flow() @await @log void {
    const bytes = @await fetch();
    const decoded = @await decode(bytes);
    @log decoded;
}

fn main() {
    async_flow()
        with @await = thenable => {
            thenable.then(result => {
                resume result;
            });
        }
}


```

## Call expression

fn = (x: u8, y: u8): u8 => x + y

<!-- fn = (x: u8, y: u8): u8 => x + y -->
<!-- fn = (x, y) => x + y -->

result = fn(x: 3, y: 7)
result = fn(3, 7)

## Loops

```deet
for (i = 0; i < 10; i++) {

}

while (expression) {
    continue;
    break;
}

for pattern of expression {
    // Do something
}
```

## Pipes

```deet
10
    |> double
    |> x => "Hello ${x}"
    |> println;

Becomes:

var temp;
temp = 10;
temp = double(temp);
temp = (x => "Hello ${x}")(temp);
temp = println(temp);
```

## Namespaces

```deet
namespace #numbers {
    fn max(x: u8, y: u8): u8 {
        if (x > y) {
            return x;
        } else {
            return y;
        }
    }
}

#numbers.max(10, 12);

using #numbers {
    max(10, 12);
};
```

## NO Destructuring

```deet
{ foo, ... } = struct
[_, foo, ...] = array
Some foo
Type { foo }
```
