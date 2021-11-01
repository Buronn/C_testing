# C_testing
tareas/controles

---
## Deployment ##
```sh
docker-compose up --build -d
```
**Run files** <br>
If it is a ```.c``` file
```sh
gcc ~/myfile.c -o executable -lpthread
```
If it is a ```.cpp``` file
```sh
g++  ~/myfile.cpp -o executable.o -lpthread
```
Use ```-lpthread``` if the file is threaded.
