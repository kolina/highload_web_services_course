all: clean
	docker-compose build
	docker-compose up -d --force-recreate

clean:
	docker-compose kill
	docker-compose rm -f

test:
	cd load_testing && make
