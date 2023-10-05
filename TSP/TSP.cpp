#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <array>
#include <cmath>
#include <string>
#include <random>
#include <algorithm>
#include <set>
#include <utility>

const int NUM_COLS = 21;
const int NUM_ROWS = 15;
const int VERT_SEPARATION = 50;
const int VERT_RADIUS = 10;
const int SCREEN_WIDTH = VERT_RADIUS * 2 + VERT_SEPARATION * (NUM_COLS - 1);
const int SCREEN_HEIGHT = VERT_RADIUS * 2 + VERT_SEPARATION * (NUM_ROWS - 1);

using namespace std;

sf::Vector2f vertPosition(int v, int offset = 0)
{
    int y = v / NUM_COLS, x = v % NUM_COLS;
    return sf::Vector2f(x * VERT_SEPARATION + offset, y * VERT_SEPARATION + offset);
}

void drawEdge(sf::RenderWindow& window, int u, int v, sf::Color color = sf::Color(127, 127, 127))
{
    sf::Vertex line[] = {
        sf::Vertex(vertPosition(u, VERT_RADIUS), color),
        sf::Vertex(vertPosition(v, VERT_RADIUS), color)
    };

    window.draw(line, 2, sf::Lines);
}

bool valid(int i, int j)
{
    return 0 <= i and i < NUM_ROWS and 0 <= j and j < NUM_COLS;
}

double distance(int v, int u)
{
    int y1 = v / NUM_COLS, x1 = v % NUM_COLS;
    int y2 = u / NUM_COLS, x2 = u % NUM_COLS;
    return sqrt((x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2));
}

// ----------------------------TSP------------------------------------------------------------------
// Función para inicializar la población aleatoriamente
std::vector<std::vector<int>> inicializarPoblacion(int NPoblacion, vector<int> ciudades) {
    std::vector<std::vector<int>> poblacion;
    int numCiudades= ciudades.size();
    for (int i = 0; i < NPoblacion; i++) {
        // Crear una permutación aleatoria de ciudades (representa un individuo)
        // Cada individuo representa los indices de las ciudades
        std::vector<int> individuo(numCiudades);
        for (int j = 0; j <= numCiudades; ++j) {
            individuo[j] = j;
        }
        std::random_shuffle(individuo.begin()+1, individuo.end());
        individuo.push_back(0);
        poblacion.push_back(individuo);
    }
    return poblacion;
}

// Calculando el fitness: suma total del camino
double calcularFitness(vector<int> individuo, vector<int> ciudades){
    double fitness= 0;
    int numIndividuos= individuo.size();
    for(int i= 0; i< numIndividuos-1; ++i){
        int ciudadActual= ciudades[individuo[i]];
        int ciudadSiguiente= ciudades[individuo[i+1]];
        fitness+= distance(ciudadActual, ciudadSiguiente);
    }
    // Agregando distancia de regreso al origen
    int ciudadActual= ciudades[individuo[numIndividuos-1]];
    int ciudadSiguiente= ciudades[individuo[0]];
    fitness+= distance(ciudadActual, ciudadSiguiente);
    return fitness;
}

// Función para seleccionar padres usando selección por ruleta
// Retorna los indices de los individos aptos para ser padres
pair<int, int> seleccionarPadres(vector<double>& fitness, int porcentaje) {
    int numIndividuos = fitness.size()*porcentaje/100;
    vector<double> probabilidades(numIndividuos);

    // Calcular la aptitud inversa como base para la selección por ruleta
    double sumaAptitudes = 0.0;
    for (int i = 0; i < numIndividuos; i++) {
        double aptitud = 1.0 / fitness[i];
        probabilidades[i] = aptitud;
        sumaAptitudes += aptitud;
    }

    // Normalizar las probabilidades
    for (int i = 0; i < numIndividuos; i++) {
        probabilidades[i] /= sumaAptitudes;
    }

    // Selección de padres usando la ruleta
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);

    double r1 = distribucion(gen);
    double r2 = distribucion(gen);

    int padre1 = -1;
    int padre2 = -1;
    double sumaProbabilidades = 0.0;

    for (int i = 0; i < numIndividuos; i++) {
        sumaProbabilidades += probabilidades[i];
        if (padre1 == -1 && sumaProbabilidades >= r1) {
            padre1 = i;
        }
        if (padre2 == -1 && sumaProbabilidades >= r2) {
            padre2 = i;
        }
    }

    return make_pair(padre1, padre2);
}

// Función para realizar el cruce entre dos padres y generar un hijo
std::vector<int> cruzar(const std::vector<int>& padre1, const std::vector<int>& padre2) {
    int numCiudades = padre1.size();

    // Elegir un punto de corte aleatorio
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> distribucion(1, numCiudades - 2);

    int puntoDeCorte = distribucion(gen);

    // Crear un vector para el hijo
    std::vector<int> hijo(numCiudades, -1);

    // Copiar una parte del padre1 al hijo
    for (int i = 0; i < puntoDeCorte; i++) {
        hijo[i] = padre1[i];
    }

    // Completar el hijo con elementos del padre2
    int indicePadre2 = 0;
    for (int i = 0; i < numCiudades-1; i++) {
        if (hijo[i] == -1) {
            // Buscar el siguiente elemento único del padre2 que no esté en el hijo
            while (std::find(hijo.begin(), hijo.end(), padre2[indicePadre2]) != hijo.end()) {
                indicePadre2++;
            }
            hijo[i] = padre2[indicePadre2];
            indicePadre2++;
        }
    }
    //Completamos con la ciudad inicial para formar un bucle
    hijo[numCiudades-1]= hijo[0];
    return hijo;
}

// Función para aplicar una mutación aleatoria a un individuo
void mutar(std::vector<int>& individuo) {
    int numCiudades = individuo.size();

    // Probabilidad de mutación (ajusta esto según tus necesidades)
    double probabilidadMutacion = 0.1; // Por ejemplo, 10%

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribucion(0.0, 1.0);

    for (int i = 0; i < numCiudades; i++) {
        if (distribucion(gen) < probabilidadMutacion) {
            // Realizar la mutación intercambiando dos ciudades aleatorias
            std::uniform_int_distribution<int> distribucionCiudades(1, numCiudades - 2);
            int ciudad1 = distribucionCiudades(gen);
            int ciudad2 = distribucionCiudades(gen);

            // Intercambiar las ciudades
            std::swap(individuo[ciudad1], individuo[ciudad2]);
        }
    }
}

// Funcion que retorna el individuo con mejor fitness
vector<int> calcularMejorFitness(vector<vector<int>>& poblacion, vector<double>& fitness){
    int pos_mn= min_element(fitness.begin(),fitness.end()) - fitness.begin();
    return poblacion[pos_mn];
}

// Funcion que ordena la poblacion 
vector<vector<int>> ordenarPoblacion(vector<vector<int>> &poblacion, vector<double> &antFitness){
    // Vector que guarda el fitness y el indice para cada individuo
    vector<pair<double,int>> fitness(poblacion.size());
    // Calculando el fitness para cada individuo
    for(int i= 0; i< poblacion.size(); i++){
        fitness[i].first= antFitness[i];
        fitness[i].second= i;
    }
    sort(fitness.begin(),fitness.end());
    vector<vector<int>> poblacionOrdenada(poblacion.size());
    for(int i= 0; i< poblacion.size(); i++){
        int pos= fitness[i].second;
        poblacionOrdenada[i]= poblacion[pos];
    }
    return poblacionOrdenada;
}

// Funcion para calcular todos los fitness
vector<double> calcularTodosFitness(vector<vector<int>> poblacion, vector<int> ciudades){
    vector<double> fitness(poblacion.size());
    for(int i= 0; i< poblacion.size(); i++){
        fitness[i]= calcularFitness(poblacion[i], ciudades);
    }
    return fitness;
}


void imprimirPoblacion(vector<vector<int>> poblacion){
    for(int i= 0; i< poblacion.size(); i++){
        cout<<"{";
        for(int j= 0; j< poblacion[i].size(); j++){
            cout<<poblacion[i][j]<<" ";
        }
        cout<<"}"<<endl;
    }
}

// Funcion para crear una nueva poblacion
vector<vector<int>> crearNuevaPoblacion(vector<vector<int>> antiguaPoblacion, vector<int> ciudades){
    // Guardando su fitness por cada individuo
    vector<double> fitness= calcularTodosFitness(antiguaPoblacion,ciudades);

    vector<vector<int>> poblacionOrdenada= ordenarPoblacion(antiguaPoblacion,fitness);
    // Actualizamos los nuevos fitness
    fitness= calcularTodosFitness(poblacionOrdenada,ciudades);
    vector<vector<int>> nuevaPoblacion;

    // Pasamos el 50% de individuos de la antigua poblacion a la nueva poblacion
    int fiftyPercent= antiguaPoblacion.size()/2;
    for(int i= 0; i < fiftyPercent; i++){
        nuevaPoblacion.push_back(poblacionOrdenada[i]);
    }
    // LLenamos el 40% de la nuevaPoblacio cruzando la antiguaPoblacion
    int fortyPercent= 2*antiguaPoblacion.size()/5;
    for(int i= 0; i< fortyPercent; i++){
        pair<int,int> padres= seleccionarPadres(fitness, 50);
        vector<int> hijo= cruzar(poblacionOrdenada[padres.first],poblacionOrdenada[padres.second]);
        nuevaPoblacion.push_back(hijo);
    }
    // Finalmente rellenamos el otro 10%(sobrante) con mutaciones del mejor individuo
    // de la antigua poblacion
    int resto= antiguaPoblacion.size()-fiftyPercent-fortyPercent;
    for(int i= 0; i < resto; i++){
        mutar(poblacionOrdenada[0]);
        nuevaPoblacion.push_back(poblacionOrdenada[0]);
    }
    return nuevaPoblacion;
}   
// -------------------------------------------------------------------------------------------------
int main()
{
    bool disabled[NUM_ROWS * NUM_COLS]{};
    vector<sf::CircleShape> verts(NUM_ROWS * NUM_COLS, sf::CircleShape(VERT_RADIUS, 50));
    vector<vector<pair<int, double> > > adj(NUM_ROWS * NUM_COLS); //pair<nodo Objetivo, distancia entre esos nodos)
    vector<pair<int, int> >
        dif1 = {{1,0}, {-1,0}, {0,1}, {0,-1}},
        dif2 = {{1,1}, {1,-1}, {-1,1}, {-1,-1}};

    int NCiudades;
    cout << "Ingresa el numero de ciudades: ";
    cin >> NCiudades;
    mt19937 rnd;
    vector<int> indices;
    // Generar lista de indices aleatorios para eliminar
    for(int i= 0; i< NUM_ROWS*NUM_COLS; i++){
        indices.push_back(i);
    }
    shuffle(indices.begin(),indices.end(),rnd);
    for(int i= 0; i< NUM_ROWS*NUM_COLS-NCiudades; i++){
        disabled[indices[i]]= true;
    }
    
    // Lista de ciudades obtenida
    vector<int> ciudades;
    for(int i= 0; i< NUM_ROWS; ++i){
        for(int j= 0; j< NUM_COLS; ++j){
            int u= i * NUM_COLS + j;
            if(disabled[u])
                continue;
            else ciudades.push_back(u);
        }
    }
    cout<<"Ciudades"<<endl;
    for(int i= 0; i< NCiudades; i++){
        cout<<ciudades[i]<<" ";
    }cout<<endl;

    int NPoblacion;
    cout << "Ingresa el numero de poblacion: ";
    cin >> NPoblacion;

    int NGeneraciones;
    cout<< "Ingresa el numero de generaciones: ";
    cin >> NGeneraciones;

    // Inicializando poblacion
    vector<vector<int>> ans;
    std::vector<vector<int>> poblacion = inicializarPoblacion(NPoblacion, ciudades);
    for(int i= 0; i< NGeneraciones; i++){
        // Crear nueva poblacion a partir de la poblacion antigua
        poblacion= crearNuevaPoblacion(poblacion,ciudades);
        //imprimirPoblacion(poblacion);
        vector<double> fitness= calcularTodosFitness(poblacion,ciudades);
        vector<int> mejorIndividuo= calcularMejorFitness(poblacion,fitness);// Mejor individuo
        cout<<"Fitness "<<i+1<<"generacion: "<<*max_element(fitness.begin(),fitness.end())<<endl;
        vector<int> ruta;
        for(int i= 0; i< mejorIndividuo.size(); i++){
            int pos= mejorIndividuo[i];
            ruta.push_back(ciudades[pos]);
        }
        ans.push_back(ruta);
    }
    //imprimiendo lista de respuestas
    //imprimirPoblacion(ans);
    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), L"Búsqueda");
    int start = -1, end = -1;
    int state = 1;
    vector<int> route;
    int i= 0;
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
                break;
            
            case sf::Event::KeyPressed:
                if (event.key.code == sf::Keyboard::Escape) {
                    state = 1;
                    route.clear();
                    for (auto& v : verts)
                        v.setFillColor(sf::Color::White);
                }
                break;
            
            default:
                break;
            }
        }

        window.clear();

    //    for(int i= 0; i< ans.size(); i++){
            for(int j= 0; j< ans[i].size(); j++){
                int y= ans[i][j]/NUM_COLS;
                int x= ans[i][j]%NUM_COLS;
                sf::CircleShape circ(5);
                auto pos= vertPosition(ans[i][j]);
                circ.setPosition(pos.x+5, pos.y+5);
                circ.setFillColor(sf::Color::Green);

                window.draw(circ);
            }
            for(int j= 0; j< ans[i].size()-1; j++){
                drawEdge(window, ans[i][j], ans[i][j+1], sf::Color::Green);
            }
    //    }
        window.display();
        if(i+1< ans.size()) ++i;
        sf::sleep(sf::seconds(0.2));
    }
    return 0;
}
