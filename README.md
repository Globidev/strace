# Strace
My implementation of strace for 42

## Building
With docker:
```sh
./build.sh $PWD
```
## Development
```sh
docker build -t strace-watch -f Dockerfile.watch .
docker run \
    --detached \
    --name strace-watch \
    -v $PWD:/strace \
    -v /var/run/docker.sock:/var/run/docker.sock \
    strace-watch $PWD
```
