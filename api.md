# API сервиса почты

Предложенное API предоставляет доступ к обычным функциям e-mail.

Для API был выбран REST-подход, потому что он основан на работе с ресурсами и связей между ресурсами с помощью ссылок, что хорошо подходит для сценариев работы с почтой, к тому же почти все операции в предоставляемом API хорошо ложатся на CRUD модель.

Опишем ресурсы, к которым API предоставляет доступ.
В качестве формата передачи используется application/hal+json, который описан здесь http://stateless.co/hal_specification.html.

## Сообщения

### Получить информацию о некотором письме

* **URL**

  /{userId}/messages/{messageId}

* **Метод:**

  `GET`
  
* **Ответ:**

   ```json
    {
        "_links": {
            "self": {"href": "/12345/messages/1234567"},
            "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
            "ma:thread": {"href": "/12345/threads/123456"}
        },
        "body": string,
        "to": [
            string
        ]
        "cc": [
            string
        ],
        "from": string,
        "theme": string
    }
    ```

### Получить информацию о всех письмах

* **URL**

  /{userId}/messages

* **Метод:**

  `GET`
  
* **Ответ:**

   ```json
    {
        "_links": {
            "self": {"href": "/12345/messages"},
            "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
            "ma:create": {"href": "/12345/messages"},
            "ma:find": {"href": "/12345/messages/{messageId}", "templated": true}
        },
        "_embedded": {
            "ma:message_item": [{
                "_links": {
                    "self": {"href": "/12345/messages/1234567"},
                    "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
                    "ma:thread": {"href": "/12345/threads/123456"}
                }
                "body": string,
                "to": [
                    string
                ]
                "cc": [
                    string
                ],
                "from": string,
                "theme": string
            }]
        }
    }
    ```

### Отправить письмо

* **URL**

  /{userId}/messages/{?threadId}
  
  Параметр `threadId` является опциональным. Если его указать, то письмо будет добавлено к нужному треду, иначе будет создан новый тред. В результате созданное сообщение будет отправлено пользователям, указанным в параметрах `to` и `cc`.

* **Метод:**

  `POST`
  
* **Тело запроса:**


   ```json
    {
        "body": string,
        "to": [
            string
        ]
        "cc": [
            string
        ],
        "theme": string
    }
    ```

## Треды

Треды являются последовательностью сообщений, представляющими беседу. Пользователь может управлять тредами опосредованно с помощью указания нужного идентификатора треда при создании письма.

### Получить информацию о некотором треде

* **URL**

  /{userId}/messages/{threadId}

* **Метод:**

  `GET`
  
* **Ответ:**

   ```json
    {
        "_links": {
            "self": {"href": "/12345/threads/123456"},
            "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
        },
        "_embedded": {
            "ma:message_item": [{
                "_links": {
                    "self": {"href": "/12345/messages/1234567"},
                    "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
                    "ma:thread": {"href": "/12345/threads/123456"}
                }
                "body": string,
                "to": [
                    string
                ]
                "cc": [
                    string
                ],
                "from": string,
                "theme": string
            }]
        }
    }
    ```

### Получить информацию о всех тредах

* **URL**

  /{userId}/threads

* **Метод:**

  `GET`
  
* **Ответ:**

   ```json
    {
        "_links": {
            "self": {"href": "/12345/threads"},
            "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
            "ma:find_thread": {"href": "/12345/threads/{threadId}", "templated": true}
        },
        "_embedded": [{
            "ma:thread_item": {
                "_links": {
                    "self": {"href": "/12345/threads/123456"},
                    "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
                },
                "_embedded": {
                    "ma:message_item": [{
                        "_links": {
                            "self": {"href": "/12345/messages/1234567"},
                            "curies": [{"name": "ma", "href": "http://example.com/docs/rels/{rel}", "templated": true}],
                            "ma:thread": {"href": "/12345/threads/123456"}
                        }
                        "body": string,
                        "to": [
                            string
                        ]
                        "cc": [
                            string
                        ],
                        "from": string,
                        "theme": string
                    }]
                }
            }
        }]
    }
    ```
