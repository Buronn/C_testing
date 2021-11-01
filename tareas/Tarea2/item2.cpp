/*
    g++ item2.cpp -o main2.o -lpthread
*/
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>
#include <time.h>
using namespace std;
volatile int energia = 100;
class AireAcondicionado
{
public:
    int temperatura;
    int habitacion;
    int gasto;
    bool encendido = false;
    bool bloqueado = false;
    mutex mtx;

    void bloquear()
    {
        mtx.lock();
        bloqueado = true;
    }
    void desbloquear()
    {
        bloqueado = false;
        mtx.unlock();
    }

    bool encenderAire()
    {
        if (encendido)
        {
            return false;
        }
        encendido = true;
        return true;
    }

    bool apagarAire()
    {
        if (!encendido)
        {
            return false;
        }
        encendido = false;
        return true;
    }

    int getTemperatura()
    {
        return temperatura;
    }
    int getHabitacion()
    {
        return habitacion;
    }
    void setTemperatura(int temperatura)
    {
        this->temperatura = temperatura;
    }
    void setHabitacion(int habitacion)
    {
        this->habitacion = habitacion;
    }
    int getGasto()
    {
        return gasto;
    }
    void setGasto(int gasto)
    {
        this->gasto = gasto;
    }

    void bajarTemperatura(int cantidad)
    {
        temperatura -= cantidad;
    }
};
class Persona
{
public:
    int id;
    int habitacion_actual;
    int cambios_habitacion; //cuantas veces cambio de habitacion
    int tiempo_habitacion;  //tiempo que esta en la habitacion
    int tiempo_espera;      //tiempo que esta esperando
    int maximo_cambios;     //cantidad maxima de cambios de habitacion
    int getId()
    {
        return id;
    }
    int getHabitacionActual()
    {
        return habitacion_actual;
    }
    int getCambiosHabitacion()
    {
        return cambios_habitacion;
    }
    int getTiempoHabitacion()
    {
        return tiempo_habitacion;
    }
    void setId(int id)
    {
        this->id = id;
    }
    void setHabitacionActual(int habitacion_actual)
    {
        this->habitacion_actual = habitacion_actual;
    }
    void setCambiosHabitacion(int cambios_habitacion)
    {
        this->cambios_habitacion = cambios_habitacion;
    }
    void setTiempoHabitacion(int tiempo_habitacion)
    {
        this->tiempo_habitacion = tiempo_habitacion;
    }
    int getMaximoCambios()
    {
        return maximo_cambios;
    }
    void setMaximoCambios(int maximo_cambios)
    {
        this->maximo_cambios = maximo_cambios;
    }
    int getTiempoEspera()
    {
        return tiempo_espera;
    }
    void addTiempoEspera(int tiempo_espera)
    {
        this->tiempo_espera += tiempo_espera;
    }
};

//hay 7 aires acondicionados
AireAcondicionado habitaciones[7];
void calentamiento(AireAcondicionado &habitacion) //aumenta constantemente la temperatura de la habitacion siempre y cuando esté desbloqueado
{
    while (true)
    {
        if (!habitacion.bloqueado && habitacion.temperatura <= 30)
        {
            habitacion.temperatura += 1;
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
        else
        {
            this_thread::sleep_for(chrono::milliseconds(1000));
        }
    }
}
int cambiarHabitacion(Persona &persona) //se desbloquea habitacion que el compadre acaba de usar
{
    habitaciones[persona.habitacion_actual].desbloquear();
    if (persona.habitacion_actual == -1) //se verifica si la persona ya no tiene mas tareas
    {
        return -1;
    }
    if (!habitaciones[persona.habitacion_actual].bloqueado) //si la se libera
    {
        cout << "habitación " << persona.habitacion_actual << " libre" << endl;
    }
    int habitacion_nueva = rand() % 7; //se elige una habitacion aleatoria
    persona.cambios_habitacion++;      //se aumenta la cantidad de cambios de habitacion
    cout << "Persona " << persona.getId() << " ha cambiado " << persona.cambios_habitacion << "/" << persona.maximo_cambios << " veces de habitación" << endl;
    if (persona.cambios_habitacion >= persona.maximo_cambios) //si ya se cambió la cantidad de veces que se puede
    {
        persona.habitacion_actual = -1; //se le asigna una habitacion nula
        return -1;
    }
    else
    {
        int count = 0;
        while (habitaciones[habitacion_nueva].bloqueado) // se comienza un ciclo en donde el compadre espera que la habitacion se desocupe
        {
            cout << "Persona " << persona.id << " esperando entrar a " << habitacion_nueva << endl;
            if (count == 0)
            {
                persona.setHabitacionActual(-1);
            }
            this_thread::sleep_for(chrono::milliseconds(1000));
            persona.addTiempoEspera(1);
            count++;
        }
        persona.setHabitacionActual(habitacion_nueva); //se le asigna la nueva habitacion
        persona.setTiempoHabitacion(rand() % 10 + 1);  //se le asigna un tiempo de habitacion aleatorio
        cout << "Persona " << persona.getId() << "\n\t entra a: " << persona.getHabitacionActual() << " con tmax:" << persona.getTiempoHabitacion() << endl;
        return habitacion_nueva; //se retorna la habitacion a la que se le asigno
    }
}
void usarAire(int habitacion, Persona &persona)
{
    if (habitacion == -1)
    {
        cout << "Persona " << persona.id << " se fue" << endl;
        return;
    }
    habitaciones[habitacion].bloquear();
    cout << "Persona " << persona.id << " entra a la habitacion " << habitacion << endl;
    if (energia - habitaciones[habitacion].gasto < 0)
    {
        cout << "No hay energia suficiente para usar el aire acondicionado" << endl;
        usarAire(cambiarHabitacion(persona), persona);
        return;
    }

    cout << "Se enciende el aire " << habitacion << " y su tiempo de habitacion es " << persona.getTiempoHabitacion() << endl;
    energia -= habitaciones[habitacion].gasto;
    habitaciones[habitacion].encenderAire();
    while (persona.tiempo_habitacion > 0) //mientras la persona tenga tiempo restante dentro de la habitación
    {
        if (habitaciones[habitacion].getTemperatura() != 20)
        {
            habitaciones[habitacion].temperatura -= 1;
        }
        cout << "Persona " << persona.id << " en [" << habitacion << "] con " << habitaciones[habitacion].getTemperatura() << "°" << endl
             << " tiempo restante " << persona.tiempo_habitacion - 1 << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));
        persona.tiempo_habitacion -= 1;
    }
    habitaciones[habitacion].apagarAire();
    cout << "Se apaga el aire " << habitacion << endl;
    int nueva_habitacion = cambiarHabitacion(persona);
    if (nueva_habitacion == -1)
    {
        cout << "Persona " << persona.id << " se fue" << endl;
    }
    else
    {
        cout << "Habitación nueva " << nueva_habitacion << endl;
        usarAire(nueva_habitacion, persona);
    }
    return;
}

int main()
{

    int temperatura_maxima = 30; //varia entre 20 y 30
    int temperatura_minima = 20;
    int energia_maxima = 10; //HAY 100 DE ENERGIA DE LA CASA y se le asigna entre 1 a 10 a cada aire acondicionado
    int energia_minima = 1;
    // inicializar habitaciones
    srand(time(NULL));
    for (int i = 0; i < 7; i++)
    {
        habitaciones[i].setTemperatura(rand() % (temperatura_maxima - temperatura_minima + 1) + temperatura_minima);
        habitaciones[i].setHabitacion(i);
        habitaciones[i].setGasto(rand() % (energia_maxima - energia_minima + 1) + energia_minima);
    }
    //ver habitaciones
    for (int i = 0; i < 7; i++)
    {
        cout << "Habitacion " << habitaciones[i].getHabitacion() << " Temperatura: " << habitaciones[i].getTemperatura() << " Gasto: " << habitaciones[i].getGasto() << endl;
    }

    int M; //cantidad de personas
    cout << "Ingrese el numero de personas: ";
    cin >> M;
    if (M > 8)
    {
        cout << "El numero de personas no puede ser mayor a 8" << endl;
        return 0;
    }

    vector<thread> threads;
    vector<thread> threadsv2;

    Persona personas[M];
    for (int i = 0; i < M; i++) //se crean las personas
    {
        personas[i].id = i;                         //se le asigna un id
        personas[i].habitacion_actual = rand() % 7; //se le asigna una habitacion aleatoria
        personas[i].cambios_habitacion = 0;
        personas[i].tiempo_habitacion = rand() % 10; //se le asigna un tiempo de habitacion aleatorio
        personas[i].maximo_cambios = 3;              //se le asigna un maximo de cambios de habitaciones
        cout << "Persona " << personas[i].id << " tiempo habitacion: " << personas[i].tiempo_habitacion << " maximo cambios: " << personas[i].maximo_cambios << endl;
    }

    for (int i = 0; i < 7; i++) //se crean los threads que calientan constantemente el aire
    {
        threadsv2.push_back(thread(calentamiento, ref(habitaciones[i])));
    }

    for (int i = 0; i < M; i++) //se crean los threads de las habitaciones
    {
        threads.push_back(thread(usarAire, ref(personas[i].habitacion_actual), ref(personas[i])));
    }

    for (int i = 0; i < M; i++) //se espera a que las personas terminen de usar las habitaciones
    {
        threads[i].join();
    }
    cout << "Presione CTRL+C para salir" << endl;

    for (int i = 0; i < 7; i++)
    {
        threadsv2[i].join();
    }
}
