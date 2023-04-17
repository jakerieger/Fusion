# Examples

## Math

Fuse (the Fusion REPL) supports executing literal math expressions directly and will print the output automatically if one is detected.

``` shell
››› 2 + 2

Output ⇛ 4
(took: 0.01ms)
```

``` shell
››› 2 + 2 * 8

Output ⇛ 18
(took: 0.01ms)
```

``` shell
››› 2 + 2 % (1 + 1)

Output ⇛ 0
(took: 0.01ms)
```

## Variables

To create a variable in Fusion, you simply give it a name and assign it a value:

```shell
››› x = 2
(x ⇛ 2)

››› msg = 'hello!'
(msg ⇛ 'hello')
```

## I/O

To print a value to the console, you can use the `print` function:

```shell
››› print 'hello'
⇛ hello
```

To read a value from the console input, you can use the `input` function:

```shell
››› x = input
x ⇛ <some input>

››› print x
⇛ <some input>
```