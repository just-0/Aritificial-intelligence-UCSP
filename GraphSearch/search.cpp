#include <vector>
#include <iostream>
#include <array>
#include <cmath>
#include <string>
#include <random>
#include <utility>

#include <SFML/Graphics.hpp>

#include "algo/bfs.h"
#include "algo/dfs.h"
#include "algo/dijkstra.h"
#include "algo/hill_climbing.h"
#include "algo/a_star.h"

const int NUM_COLS = 50;
const int NUM_ROWS = 25;
const int NUM_VERTS = NUM_COLS * NUM_ROWS;
const int VERT_SEPARATION = 50;
const int VERT_RADIUS = 14;
const int SCREEN_WIDTH = VERT_RADIUS * 2 + VERT_SEPARATION * (NUM_COLS - 1);
const int SCREEN_HEIGHT = VERT_RADIUS * 2 + VERT_SEPARATION * (NUM_ROWS - 1);

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

int main()
{
    bool disabled[NUM_ROWS * NUM_COLS]{};
    std::vector<sf::CircleShape> verts(NUM_ROWS * NUM_COLS, sf::CircleShape(VERT_RADIUS, 4));
    std::vector<std::vector<std::pair<int, double> > > adj(NUM_ROWS * NUM_COLS);
    std::vector<std::pair<int, int> >
        dif1 = {{1,0}, {-1,0}, {0,1}, {0,-1}},
        dif2 = {{1,1}, {1,-1}, {-1,1}, {-1,-1}};

    int percentage;
    while (true) {
        std::cout << "Porcentaje de nodos a eliminar [0, 100]: ";
        std::cin >> percentage;
        if (0 <= percentage && percentage < 100) {
            break;
        } else {
            std::cout << "Porcentaje no valido\n\n";
        }
    }

    int num = (NUM_ROWS * NUM_COLS) * percentage;
    std::mt19937 rnd;
    std::uniform_int_distribution<> uid(1, 100);

    for (int i = 0; i < NUM_ROWS; ++i) {
        if (num <= 0)
            break;

        for (int j = 0; j < NUM_COLS; ++j) {
            if (num <= 0)
                break;

            if (uid(rnd) <= percentage) {
                disabled[i * NUM_COLS + j] = true;
                --num;
            }
        }
    }

    for (int i = 0; i < NUM_ROWS; ++i) {
        for (int j = 0; j < NUM_COLS; ++j) {
            int v = i * NUM_COLS + j;
            verts[v].setPosition(vertPosition(v));

            if (disabled[v])
                continue;
            
            for (const auto& p : dif1) {
                if (valid(i + p.first, j + p.second)) {
                    int u = (i + p.first) * NUM_COLS + j + p.second;
                    if (disabled[u])
                        continue;

                    adj[v].push_back(std::make_pair(u, distance(u, v)));
                    adj[u].push_back(std::make_pair(v, distance(u, v)));
                }
            }

            if (i % 2 == 1 && j % 2 == 1) {
                for (const auto& p : dif2) {
                    if (valid(i + p.first, j + p.second)) {
                        int u = (i + p.first) * NUM_COLS + j + p.second;
                        if (disabled[u])
                            continue;

                        adj[v].push_back(std::make_pair(u, distance(u, v)));
                        adj[u].push_back(std::make_pair(v, distance(u, v)));
                    }
                }
            }
        }
    }
    
    for (auto& vec : adj) {
        sort(vec.begin(), vec.end());
        vec.erase(std::unique(vec.begin(), vec.end()), vec.end());
    }
    
    auto search_func = dijkstra;
    bool exit = false;
    while (!exit) {
        std::cout << "Que algoritmo prefieres?:\n"
            << "\t1) Busqueda en profundidad\n"
            << "\t2) Busqueda en amplitud\n"
            << "\t3) Hill Climbing\n"
            << "\t4) A*\n"
            << "\t5) Dijkstra\n"
            << "Eleccion: ";

        int choice;
        std::cin >> choice;
        switch (choice) {
            case 1:
                search_func = dfs;
                exit = true;
                break;
            case 2:
                search_func = bfs;
                exit = true;
                break;
            case 3:
                search_func = hillClimbing;
                exit = true;
                break;
            case 4:
                search_func = AStar;
                exit = true;
                break;
            case 5:
                exit = true;
                break;
            default:
                std::cout << "Eleccion incorrecta, prueba otra vez\n\n";
                break;
        }
    }

    sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), L"Búsqueda");
    int start = -1, end = -1;
    int state = 1;
    std::vector<int> route;
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

        for (int v = 0, n = verts.size(); v < n; ++v) {
            if (not disabled[v]) {
                for (const auto& p : adj[v])
                    drawEdge(window, v, p.first);

                auto rect = verts[v].getGlobalBounds();
                switch (state) {
                case 1:
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                        rect.contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
                        state = 2;
                        start = v;
                        verts[v].setFillColor(sf::Color::Green);
                    }
                    break;
                
                case 2:
                    if (start != v && sf::Mouse::isButtonPressed(sf::Mouse::Left) &&
                        rect.contains(sf::Vector2f(sf::Mouse::getPosition(window)))) {
                        state = 3;
                        end = v;
                        verts[v].setFillColor(sf::Color::Green);
                        route = search_func(adj, start, end);
                        for (int node : route)
                            verts[node].setFillColor(sf::Color::Green);
                    }
                    break;

                default:
                    break;
                }
            }
        }

        for (int v = 0, n = verts.size(); v < n; ++v) {
            if (not disabled[v])
                window.draw(verts[v]);
        }
        
        for (int i = 0, n = route.size(); i + 1 < n; ++i)
            drawEdge(window, route[i], route[i + 1], sf::Color::Green);

        window.display();
    }

    return 0;
}
