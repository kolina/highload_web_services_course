Нагрузочное тестирование https://overload.yandex.net/5644#tab=test_data&tags=&plot_groups=main&machines=8&metrics=&compress_ratio=1&slider_start=1482983669&slider_end=1482983999.

Проводилось линейным увеличением RPS до 10000 за 5 минут. По графику наблюдаем, что разладка наступает при уровне около 4600 RPS (появляется много сетевых и http ошибок, стабильно начинает расти 98 процентиль ответа).

По графику мониторинга https://overload.yandex.net/5644#tab=monitoring&tags=&plot_groups=main&machines=8&metrics=&compress_ratio=1&slider_start=1482983669&slider_end=1482983999 можно сделать вывод, что скорее всего, мы упираемся в cpu. Увеличив количество рабочих процессов у nginx и количество потоков в fastcgi-демоне, картина действительно улучшилась https://overload.yandex.net/5653#tab=test_data&tags=&plot_groups=main&machines=&metrics=&slider_start=1482988108&slider_end=1482988408. Полной разладки на графиках нет, с увеличением лишь начинает просаживаться время ответов.

При стабильной нагрузке в 3000 RPS получаем стабильную работу сервиса https://overload.yandex.net/6143, 99-й процентиль на уровне 7 мс.
