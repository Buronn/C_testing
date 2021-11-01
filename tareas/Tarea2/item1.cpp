/*
    g++ item1.cpp -o main.o -lpthread
    LOS DIAS SERÁN LOS HILOS
    LAS TAREAS SON PROCESOS
    EL ARCHIVO DE SALIDA SE LLAMA OUTPUT.CSV
    CLASES A USAR
        NODE
        TAREA
        DIA
    EL INPUT ES IGUAL AL DEL ENUNCIADO
    x
    y y y y
    z z z z
    1
    (x7)


    SI SUMA MÁS DE 24 HORAS EXPLOTA EL PROGRAMA
    SI NO SE INTRODUCE NADA EXPLOTA EL PROGRAMA
    SI SUMA MENOS DE 24 HORAS NO EXPLOTA EL PROGRAMA
    SI DURA LO MISMO NO EXPLOTA EL PROGRAMA
*/

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <fstream>
#include <bits/stdc++.h>
using namespace std;
struct node
{
    char pname;
    int btime;       //BURST TIME
    int atime;       //ARRIVAL TIME
    int priority;    //PRIORITY (MENOR MANDA)
    int restime = 0; //RESIDUAL TIME
    int ctime = 0;   //COMPLETION TIME
    int wtime = 0;   //WAITING TIME
};
class Proceso
{
public:
    int id;
    int horas;
    bool Inter;
    vector<int> H; //H[i] = horas de proceso i
    vector<int> D; // D[i] = dias de proceso i
    vector<int> bursttime;

    void setid(int a)
    {
        id = a;
    }
    void sethoras(int a)
    {
        horas = a;
    }
    void setH(vector<int> a)
    {
        H = a;
    }
    void setD(vector<int> a)
    {
        D = a;
    }
    void setInterrupt(bool a)
    {
        Inter = a;
    }

    void display()
    {
        const char *a[7];
        a[0] = "Ayudantia";
        a[1] = "Correción";
        a[2] = "Clases";
        a[3] = "Almuerzo";
        a[4] = "Cena";
        a[5] = "Dormir";
        a[6] = "¡PACTO!";

        const char *days[7];
        days[0] = "Lunes";
        days[1] = "Martes";
        days[2] = "Miércoles";
        days[3] = "Jueves";
        days[4] = "Viernes";
        days[5] = "Sábado";
        days[6] = "Domingo";
        cout << "Tarea: " << a[id] << endl;
        cout << "\t\tBurst Time: " << horas << endl;
        for (size_t i = 0; i < D.size(); i++)
        {
            cout << "\t\t\t" << days[D.at(i)] << " a las " << H.at(i) << ":00 durante " << bursttime.at(i) << " hora/s" << endl;
        }
        if (Inter)
        {
            cout << "\t\tInterrumpible" << endl;
        }
        else
        {
            cout << "\t\tNo interrumpible" << endl;
        }
    }

    void bubbleSort()
    {
        bool swapp = true;
        while (swapp)
        {
            swapp = false;
            for (size_t i = 0; i < D.size() - 1; i++)
            {
                if (D.at(i) > D.at(i + 1))
                {

                    D.at(i) += D.at(i + 1);
                    D.at(i + 1) = D.at(i) - D.at(i + 1);
                    D.at(i) -= D.at(i + 1);
                    H.at(i) += H.at(i + 1);
                    H.at(i + 1) = H.at(i) - H.at(i + 1);
                    H.at(i) -= H.at(i + 1);
                    swapp = true;
                }
            }
        }
    }

    bool contains(int elem)
    {
        bool result = false;
        for (int i = 0; i < D.size(); i++)
        {
            if (D.at(i) == elem)
            {
                result = true;
                break;
            }
        }
        return result;
    }

    void distri()
    {
        int horas_dia = horas / D.size();
        int horas_restantes = horas % D.size();
        for (size_t i = 0; i < D.size(); i++)
        {
            if (horas_restantes == 0)
            {
                bursttime.push_back(horas_dia);
            }
            else
            {
                bursttime.push_back(horas_dia + 1);
                horas_restantes--;
            }
        }
    }
};
Proceso procesos[7];
class Day
{
public:
    vector<int> tareas;
    vector<int> horas;

    void setTareas(vector<int> a)
    {
        tareas = a;
    }
    void setHoras(vector<int> a)
    {
        horas = a;
    }
};
Day dias[7];
void print(std::vector<int> const &input)
{
    cout << "{";
    for (int i = 0; i < input.size(); i++)
    {
        cout << input.at(i);
        if (i != input.size() - 1)
            cout << ", ";
        if (i == input.size() - 1)
        {
            cout << "}";
        }
    }
    cout << endl;
}
void insert(int n, char pname[], int priority[], int atime[], int btime[], node a[])
{ //INSERTA EN LOS NODOS.
    for (int i = 0; i < n; i++)
    {
        a[i].pname = pname[i];
        a[i].priority = priority[i];
        a[i].atime = atime[i];
        a[i].btime = btime[i];
        a[i].wtime = -a[i].atime + 1;
    }
}
bool btimeSort(node a, node b)
{
    return a.btime < b.btime;
}
bool atimeSort(node a, node b)
{
    return a.atime < b.atime;
}
bool prioritySort(node a, node b)
{
    return a.priority < b.priority;
}

int k = 0, f = 0, r = 0; //K=PROCESOS EN COLA, F=PROCESOS FINALIZADOS, R=PROCESOS EN RESERVA

Day disp(int nop, int qt, node a[], node b[], node c[])
{                              // nop = NUMERO DE PROCESOS, qt = QUANTUM TIME
    int n = nop, q;            //n=NUMERO DE PROCESOS, q=QUANTUM TIME
    sort(a, a + n, atimeSort); //ORDENA POR ATIME
    int ttime = 0, i;          //TTIME = TIEMPO TOTAL
    int j, tArray[n];          //TARRAY = ARRAY DE TIEMPOS
    int alltime = 0;           //ALLTIME = TIEMPO TOTAL DE ESPERA
    bool moveLast = false;     //MOVE LAST = SI EL ULTIMO PROCESO SE MOVIO A LA COLA
    for (i = 0; i < n; i++)
    {                          //RECORRE LOS PROCESOS
        alltime += a[i].btime; //SUMA EL BURST TIME
    }
    alltime += a[0].atime; //SUMA EL TIEMPO DE ARRIVO
    for (i = 0; ttime <= alltime;)
    { //RECORRE HASTA QUE EL TIEMPO TOTAL SEA MAYOR AL ALLTIME
        j = i;
        while (a[j].atime <= ttime && j != n)
        { //RECORRE HASTA QUE EL ATIME SEA MENOR O IGUAL AL TIEMPO TOTAL
            b[r] = a[j];
            j++;
            r++;
        }
        if (r == f)
        {                                    //SI EL NUMERO DE PROCESOS FINALIZADOS ES IGUAL AL NUMERO DE PROCESOS EN COLA
            c[k].pname = 'L';                //SE CREA UN NUEVO PROCESO
            c[k].btime = a[j].atime - ttime; //SE LE ASIGNA EL TIEMPO DE ESPERA
            c[k].atime = ttime;              //SE LE ASIGNA EL TIEMPO DE ARRIVO
            ttime += c[k].btime;             //SE LE SUMA EL TIEMPO DE ESPERA
            k++;                             //SE INCREMENTA EL NUMERO DE PROCESOS EN COLA
            continue;                        //SE CONTINUA EL CICLO
        }
        i = j;
        if (moveLast == true)
        {                                     //SI EL ULTIMO PROCESO SE MOVIO A LA COLA
            sort(b + f, b + r, prioritySort); //ORDENA POR PRIORITY
            // b[r]=b[f];
            // f++;
            // r++;
        }
        j = f;
        if (b[j].btime > qt)
        { //SI EL BURST TIME ES MAYOR AL QUANTUM TIME
            c[k] = b[j];
            c[k].btime = qt;
            k++;
            b[j].btime = b[j].btime - qt;
            ttime += qt;
            moveLast = true;
            for (q = 0; q < n; q++)
            { //RECORRE LOS PROCESOS
                if (b[j].pname != a[q].pname)
                {
                    a[q].wtime += qt;
                }
            }
        }
        else
        { //SI EL BURST TIME ES MENOR AL QUANTUM TIME
            c[k] = b[j];
            k++;
            f++;
            ttime += b[j].btime;
            moveLast = false;
            for (q = 0; q < n; q++)
            { //RECORRE LOS PROCESOS
                if (b[j].pname != a[q].pname)
                {
                    a[q].wtime += b[j].btime;
                }
            }
        }
        if (f == r && i >= n)
            break; //SI EL NUMERO DE PROCESOS FINALIZADOS ES IGUAL AL NUMERO DE PROCESOS EN COLA Y EL PROCESO ACTUAL ES EL ULTIMO
    }
    tArray[i] = ttime;
    ttime += a[i].btime;
    for (i = 0; i < k - 1; i++)
    {
        if (c[i].pname == c[i + 1].pname)
        {
            c[i].btime += c[i + 1].btime;
            for (j = i + 1; j < k - 1; j++)
                c[j] = c[j + 1];
            k--;
            i--;
        }
    }

    int rtime = 0;
    for (j = 0; j < n; j++)
    {
        rtime = 0;
        for (i = 0; i < k; i++)
        {
            if (c[i].pname == a[j].pname)
            {
                a[j].restime = rtime;
                break;
            }
            rtime += c[i].btime;
        }
    }
    float averageWaitingTime = 0;
    float averageResponseTime = 0;
    float averageTAT = 0;

    rtime = 0;
    vector<int> v;
    for (i = 0; i < k; i++)
    {
        if (i != k)
        {
            if (c[i].pname != NULL)
            {
                cout << "|   P" << c[i].pname << "   ";

                if (c[i].pname == 'L')
                {
                    v.push_back(-1);
                }
                else
                {
                    int ia = c[i].pname - '0';
                    v.push_back(ia);
                }
            }
        }
        rtime += c[i].btime;
        for (j = 0; j < n; j++)
        {
            if (a[j].pname == c[i].pname)
                a[j].ctime = rtime;
        }
    }

    vector<int> v2;
    cout << "\n";
    rtime = 0;
    for (i = 0; i < k + 1; i++)
    {
        int aux = rtime;
        if ((rtime + c[i].btime) != aux)
        {
            cout << rtime << "\t";
            v2.push_back(rtime);
            rtime += c[i].btime;
        }
        else
        {
            rtime += c[i].btime;
        }
    }
    cout << "\n";
    Day day;
    day.setTareas(v);
    day.setHoras(v2);
    cout << "\n";
    cout << "\n";
    return day;
}

void *
insert_tarea(vector<int> tareas, Day &dia, int n)
{
    node a[1000];
    node b[1000];
    node c[1000];

    cout << "Dia " << n << endl;
    int tamano = tareas.size();
    int ids[tamano];
    int horas[tamano];
    int burst[tamano];
    int inter[tamano];

    for (size_t i = 0; i < tamano; i++)
    {
        ids[i] = tareas.at(i);

        for (size_t j = 0; j < procesos[ids[i]].D.size(); j++)
        {
            if (procesos[ids[i]].D.at(j) == n) // 0 2 5 6 = 0
            {

                horas[i] = procesos[ids[i]].H.at(j);
                burst[i] = procesos[ids[i]].bursttime.at(j);
            }
        }
        inter[i] = procesos[tareas.at(i)].Inter;
    }
    char cids[tamano];
    for (size_t i = 0; i < tamano; i++)
    {
        cids[i] = ids[i] + '0';
    }



    insert(tamano, cids, inter, horas, burst, a);
    dia = disp(tamano, 1, a, b, c);

    return 0;
}

void tokenize(string const &str, const char delim, vector<string> &out)
{
    stringstream ss(str);

    string s;
    while (getline(ss, s, delim))
    {
        out.push_back(s);
    }
}

void writecsv()
{
    ofstream myfile;
    myfile.open("output.csv");
    const char *a[7];
    a[0] = "Ayudantia";
    a[1] = "Correción";
    a[2] = "Clases";
    a[3] = "Almuerzo";
    a[4] = "Cena";
    a[5] = "Dormir";
    a[6] = "¡PACTO!";

    const char *days[7];
    days[0] = "Lunes";
    days[1] = "Martes";
    days[2] = "Miércoles";
    days[3] = "Jueves";
    days[4] = "Viernes";
    days[5] = "Sábado";
    days[6] = "Domingo";

    int sched[7][24];
    for (int i = 0; i < 7; i++)
    {
        for (int j = 0; j < 24; j++)
        {
            sched[i][j] = -1;
        }
    }
    for (int i = 0; i < 7; i++)
    {

        int y = 0;
        int s = 0;
        for (int j = 1; j < dias[i].horas.size(); j++) // {5, -1, 2, -1, 3, -1}     tareas
        {                                              // {0, 7, 8, 12, 13, 14, 17} horas
            int x = 0;
            x = dias[i].horas.at(j) - dias[i].horas.at(j - 1);
            for (int k = 0; k < x; k++)
            {
                sched[i][y] = dias[i].tareas.at(j - 1);
                y++;
            }
        }
    }
    for (int i = 0; i < 7; i++)
    {
        if(i<6)
        myfile << days[i] << ",";
        else
        myfile << days[i] << "\n";
    }
    for (int j = 0; j < 24; j++)
    {
        for (int i = 0; i < 7; i++)
        {
            if (i < 6)
            {
                if (sched[i][j] != -1)
                {
                    myfile << a[sched[i][j]] << ",";
                }
                else
                {
                    myfile << "Trabajo"
                           << ",";
                }
            }
            else
            {
                if (sched[i][j] != -1)
                {
                    myfile << a[sched[i][j]] << "\n";
                }
                else
                {
                    myfile << "Trabajo"
                           << "\n";
                }
            }
        }
    }
}

int main(int arg, char *argv[])
{

    const char delim = ' ';
    for (int i = 0; i < 7; i++) //INPUT
    {
        procesos[i].setid(i);
        string h;
        /*         cout << "Ingrese duracion total semanal \n"; */
        getline(cin, h);
        procesos[i].sethoras(stoi(h));

        vector<int> hi;
        vector<string> out1;
        string hi_in;
        /*         cout << "Ingrese lista de horas (separados por espacio)\n"; */
        getline(cin, hi_in);
        tokenize(hi_in, delim, out1);
        for (auto &hi_in : out1)
        {
            hi.push_back(stoi(hi_in));
        }
        procesos[i].setH(hi);

        vector<int> d;
        vector<string> out2;
        string d_in;
        /*         cout << "Ingrese lista de dias (separados por espacio) \n"; */
        getline(cin, d_in);
        tokenize(d_in, delim, out2);
        for (auto &d_in : out2)
        {
            d.push_back(stoi(d_in));
        }
        procesos[i].setD(d);

        string b;
        /*         cout << "Ingrese si es interruptible (1 o 0) \n"; */
        getline(cin, b);

        procesos[i].setInterrupt(stoi(b));
        /*         cout << "----------" << i << "------------" << endl; */
        procesos[i].bubbleSort();
        procesos[i].distri();
        /*         cout << "----------------------" << endl; */
    }
    for (int i = 0; i < 7; i++)
    {
        //Dia
        vector<int> tareas;
        for (int j = 0; j < 7; j++)
        {
            //Actividad
            auto result = procesos[j].contains(i);
            if (result)
            {
                tareas.push_back(j);
            }
        }
        thread t(insert_tarea, std::ref(tareas), std::ref(dias[i]), std::ref(i)); //crea un thread
        t.join();                                                                 //espera a que termine el thread
        print(dias[i].tareas);
        print(dias[i].horas);
    }
    writecsv();
}
