all: build

build:
	docker build -t node-app .

run:
	npm start -p 8888:8080

clean:
	docker images -a | grep none | awk '{ print $$3; }' | xargs -t 2>/dev/null docker rmi

mrproper: clean
	rm -r node_modules
