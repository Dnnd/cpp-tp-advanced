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
build/examples/echo-client [--hostname HOST_URL] [--port PORTNAME]
build/examples/echo-server [--hostname HOST_URL] [--port PORTNAME]
```

Пример
```
build/examples/echo-server --hostname localhost --port 8080
echo "abcd" | build/examples/echo-client --hostname localhost --port 8080
```
