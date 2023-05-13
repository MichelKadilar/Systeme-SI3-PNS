all: build

build:
	docker build -t node-app .

clean:
	docker images -a | grep none | awk '{ print $$3; }' | xargs -t 2>/dev/null docker rmi

mrproper: clean
	rm -r node_modules
