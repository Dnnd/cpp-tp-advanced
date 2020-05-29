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
build/examples/echo-server -h localhost -p 8080
echo "abcd" | build/examples/echo-client -h localhost -p 8080
```

## Домашнее задание 4
```
cmake -B ./build
cmake --build build
```

Запуск (реализован пример эхо-сервера и http-сервера (всегда отдает 200 OK))
```
build/examples/async-echo-server [-h HOST_URL] [-p PORTNAME]
build/examples/http-example [-h HOST_URL] [-p PORTNAME]
```

Пример - нагрузочное тестирование по HTTP
```
build/examples/http-example -h localhost -p 8080
ab -n 100000 -c 1000 http://localhost:8080/
```

Пример - эхо с использованием `nc` в качестве клиента
```
build/examples/async-echo-server -h localhost -p 8080
echo "abcd" | nc localhost 8080
```

## Домашнее задание 5
Пример - родительский процесс вставляет в ShMap четные ключи, дочерний - нечетные. Оба процесса после вставки читают состояние ShMap, каждый процесс должен увидеть собственные вставки ключей и потенциально - ключи, вставленные соседним процессом.
```
build/examples/shmem-example
```

## Домашнее задание 6
```
cmake -B ./build
cmake —build build
build/examples/http-server-example  [-h HOST_URL] [-p PORTNAME] [-d | -i | -e | -w] [-l LOGFILE]
```

## Домашнее задание 7
```
cmake -B ./build
cmake —build build
build/examples/kvstore-generator  [-k PATH_TO_STORAGE] [-s STEP] [-s KEYS_NUMBER]
```
`PATH_TO_STORAGE` - путь до генерируемого файла с данными

`STEP` - шаг, с которым генерируются ключи. Например, при STEP = 100, будут генерироваться ключи 0, 100, 200...

`KEYS_NUMBER` - количество сгенерированных ключей. 

```
build/examples/kvstore-http  [-k PATH_TO_STORAGE] [-s KEYS_NUMBER] [-h HOST_URL] [-p PORTNAME] [-d | -i | -e | -w] [-l LOGFILE]
```

`PATH_TO_STORAGE` - путь до файла с данными

`KEYS_NUMBER` - количество ключей в файле с данными
