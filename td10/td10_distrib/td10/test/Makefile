all: build

build:
	docker build -t node-client .

run:
	docker run --network mynetwork -e TZ=Europe/Paris -itd node-client /bin/bash # docker run --user userName -p hostPort:containerPort imageName

clean:
	docker images -a | grep none | awk '{ print $$3; }' | xargs -t 2>/dev/null docker rmi

mrproper: clean
	rm -r node_modules
