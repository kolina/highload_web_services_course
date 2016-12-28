all: clean
	docker-compose build
	docker-compose up -d --force-recreate

clean:
	docker-compose kill
	docker-compose rm -f

restart:
	docker-compose restart fastcgi

build:
	docker-compose run --rm app make

reload: build restart

test:
	cd load_testing && make
