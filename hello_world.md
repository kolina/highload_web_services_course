# Зависимости

Для запуска необходимо, чтобы были установлены:
* `make`
* `docker`
* `docker-compose`

# Сборка и запуск
* `make all`: собрать и запустить сервис с нуля
* `make restart`: пересобрать `so` файл с новым приложением и рестартовать `fastcgi`-демон

# Проверка, что сервис работает
```
ubuntu@ip-172-31-5-22: ~/highload_web_services_course > curl localhost:8080/hellofastcgi
Hello, stranger
ubuntu@ip-172-31-5-22: ~/highload_web_services_course > 
```
