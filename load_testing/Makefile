all:
	rm -rf ammo.txt
	bash -c 'python gen_input_for_ammo.py | python make_ammo.py > ammo.txt'
	docker run -v $(shell pwd):/var/loadtest -v $(HOME)/.ssh:/root/.ssh --net host -it direvius/yandex-tank
