# cpp-tp-advanced

## Домашнее задание 1
Запуск:

``` 
cmake -B ./build

build/examples/process-echo build/examples/echo
```

## Домашнее задание 2

Сборка

```
cmake -B ./build
cmake --build build
```
Использование
```
build/examples/logger-example [--debug | --info | --error | --warn] [--logfile <path_to_logfile]
```

## Домашнее задание 3
```
cmake -B ./build
cmake --build build
```

Использование
```
build/examples/echo-client [-h HOST_URL] [-p PORTNAME]
build/examples/echo-server [-h HOST_URL] [-p PORTNAME]
```

Пример
```
build/examples/echo-server --hostname localhost --port 8080
echo "abcd" | build/examples/echo-client --hostname localhost --port 8080
```
