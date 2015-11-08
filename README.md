# Strace
My implementation of a simple [strace](http://linux.die.net/man/1/strace) for [42](http://www.42.fr/)

## Building
With docker:
```sh
./build.sh $PWD
```
## Development
### Auto-compile workflow
Still with docker:
```sh
docker build -t strace-watch -f Dockerfile.watch .
docker run \
    --detach \
    --name strace-watch \
    -v $PWD:/strace \
    -v /var/run/docker.sock:/var/run/docker.sock \
    strace-watch $PWD
```
### Generating the syscall table
Again with docker:
```sh
docker build -t strace-gen syscall_gen/
MAN2_PATH="/usr/share/man/man2" # Or whatever path to your man2 pages
docker run \
    --rm \
    -v $PWD/syscall_gen:/gen \
    -v $MAN2_PATH:/man2 \
    strace-gen
```
