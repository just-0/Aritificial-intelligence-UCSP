#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <math.h>
#include <fstream>
#include <cmath>

float f(int x, int y, int z);
int binaryToDecimal(std::string n);
std::string DecimalToBinary(int num, int bits);
std::string reverse(std::string str);
int random_num(int start, int end);
void save_stats(std::vector<float> means, std::vector<float> total, std::vector<float> max);

#define PI 3.14159265359

class individual
{
    public:
    std::string x, y, z;
    std::string chromosome;
   float value;

    individual(std::string _x, std::string _y, std::string _z)
    {
        this->x = _x;
        this->y = _y;
        this->z = _z;
        this->chromosome = _x+_y+_z;
        this->value = this->get_value();
    }

    float get_value()
    {
        return f(binaryToDecimal(this->x), binaryToDecimal(this->y), binaryToDecimal(this->z));
    }

    individual combine(individual ind, int crossing_points)
    {
        //std::cout << "combining: " << this->chromosome << " and " << ind.chromosome << " with points: ";
        if (crossing_points > chromosome.length()/2)
        {
            crossing_points = 3;
        }

        std::vector<int> points;

        while (points.size() < crossing_points)
        {
            int n = random_num(0, chromosome.length()-2);
            if (std::find(points.begin(), points.end(), n) == points.end())
            {
                //std::cout << n << " ";
                points.push_back(n);
            }
        }
        points.push_back(chromosome.length()-1);
        std::sort(points.begin(), points.end());

        std::string res = "";
        bool turn = false;
        int idx = 0;
        for (int i = 0; i < points.size(); i++)
        {
            int cur_point = points[i];
            if (!turn)
            {
                while (idx <= cur_point)
                {
                    int prob = random_num(0, 100);
                    //std::cout << "prob: " << prob << std::endl;
                    if (prob <= 98)
                    {
                        res += this->chromosome[idx];
                    }
                    else {res += std::to_string(random_num(0, 1));}
                    idx++;
                }
                turn = !turn;
            }
            else
            {
                while (idx <= cur_point)
                {
                    int prob = random_num(0, 100);
                    //std::cout << "prob: " << prob << std::endl;
                    if (prob <= 98)
                    {
                        res += ind.chromosome[idx];
                    }
                    else {res += std::to_string(random_num(0, 1));}
                    idx++;
                }
                turn = !turn;
            }
        }

        std::string strx, stry, strz;
        strx = "";
        for (int i = 0; i < this->x.length(); i++)
        {
            strx += res[i];
        }

        stry = "";
        for (int i = 0; i < this->y.length(); i++)
        {
            stry += res[i+this->x.length()];
        }
        
        strz = "";
        for (int i = 0; i < this->z.length(); i++)
        {
            strz += res[i+this->x.length()+this->y.length()];
        }

        //std::cout << "result: " << res << std::endl;
        //std::cout << "result: " << strx << stry << strz << std::endl;
        return individual(strx, stry, strz);   
    }

    void operator =(const individual &a)
    {
        x = a.x;
        y = a.y;
        z = a.z;
        chromosome = a.chromosome;
        value = a.value;
    }
};

individual generate_ind(int Xmax, int Ymax, int Zmax);
bool cmp(const individual &a, const individual &b);
std::vector<individual> process_population(std::vector<individual> &population);
void print_generation(std::vector<individual> population);
void print_generation_stats(std::vector<individual> population);

int main()
{
    srand((unsigned)(time(0)));

    std::vector<float> means;
    std::vector<float> totals;
    std::vector<float> maxs;

    float total;
    float local_max;

    int ind_num, xbits, ybits, zbits, cross_points, generations;
    std::vector<individual> population;

    //Getting parameters
    std::cout << "Number of individuals per generation: ";
    std::cin >> ind_num;

    std::cout << "Number of bits for x: ";
    std::cin >> xbits;

    std::cout << "Number of bits for y: ";
    std::cin >> ybits;

    std::cout << "Number of bits for z: ";
    std::cin >> zbits;

    std::cout << "Number cross points (< population size / 2): ";
    std::cin >> cross_points;

    std::cout << "Number of generations: ";
    std::cin >> generations;

    //Generating initial population
    for (int i = 0; i < ind_num; i++)
    {
        population.push_back(generate_ind(xbits, ybits, zbits));
    }

    for (int gen = 0; gen < generations; gen++)
    {
        std::sort(population.begin(), population.end(), cmp);
        std::cout << "Generation " << gen << std::endl;
        print_generation(population);
        std::vector<individual> buf_pop = population;

        std::vector<individual> pro_population = population;
        
        std::vector<individual> newgen;
        total = 0;
        local_max = 0;
        for (int i = 0; i < pro_population.size(); i++)
        {
            total += pro_population[i].value;
            local_max = std::max(local_max, pro_population[i].value);

            int mate = i;
            while (i == mate)
            {
                mate = random_num(0, pro_population.size()-1);
            }
            newgen.push_back(pro_population[i].combine(pro_population[mate], cross_points));
        }

        for (int i = 0; i < 10*buf_pop.size()/100; i++)
        {
            newgen[i] = buf_pop[i];
        }

        means.push_back(total/pro_population.size());
        totals.push_back(total);
        maxs.push_back(local_max);

        population = newgen;
    }
    std::cout << "Generation " << generations << std::endl;
    print_generation(population);

    save_stats(means, totals, maxs);
}

void save_stats(std::vector<float> means, std::vector<float> total, std::vector<float> max)
{
    int size = means.size();

    std::ofstream file("means.csv");
    for (int i = 0; i < size; i++)
    {
        file << i << "," << means[i] << "\n";
    }
    file.close();

    std::ofstream file1("total.csv");
    for (int i = 0; i < size; i++)
    {
        file1 << i << "," << total[i] << "\n";
    }
    file1.close();

    std::ofstream file2("max.csv");
    for (int i = 0; i < size; i++)
    {
        file2 << i << "," << max[i] << "\n";
    }
    file2.close();
}

//x*x + log10(x) - y*y
//x*x - y*log10(x) + y*y - 69
//(x-5)*(x-5) + (y-6)*(y-6) + (z-8)*(z-8) - 16

float f(int x, int y, int z)
{
    return (x-5)*(x-5) - (y-6)*(y-6) + (z-8)*(z-8) - 16;
}

bool cmp(const individual &a, const individual &b)
{
    return a.value > b.value;
}

std::string DecimalToBinary(int num, int bits)
{
    std::string str;
      while(num){
      if(num & 1) 
        str+='1';
      else 
        str+='0';
      num>>=1; 
    }
    std::string res = reverse(str);   
    return std::string(bits-res.length(), '0') + res;
}
 
std::string reverse(std::string str)
{
    std::string res = "";
    for(int i=str.size()-1 ; i>=0 ; i--)
    {
        res += str[i];
    }
    return res;
}

int binaryToDecimal(std::string n)
{
    std::string num = n;
    int dec_value = 0;
    int base = 1;
  
    int len = num.length();
    for (int i = len - 1; i >= 0; i--) {
        if (num[i] == '1')
            dec_value += base;
        base = base * 2;
    }
  
    return dec_value;
}

int random_num(int start, int end)
{
    int range = (end-start)+1;
    int random_int = start+(rand()%range);
    return random_int;
}

individual generate_ind(int Xmax, int Ymax, int Zmax)
{
    std::string x_bin = DecimalToBinary(random_num(0, binaryToDecimal(std::string(Xmax, '1'))), Xmax);
    std::string y_bin = DecimalToBinary(random_num(0, binaryToDecimal(std::string(Ymax, '1'))), Ymax);
    std::string z_bin = DecimalToBinary(random_num(0, binaryToDecimal(std::string(Zmax, '1'))), Zmax);

    return individual(x_bin, y_bin, z_bin);
}

std::vector<individual> process_population(std::vector<individual> &population)
{
    float mean = 0;
    int population_size = population.size();
    bool cap = false;
    for (int i = 0; i < population_size; i++)
    {
        mean += (float) population[i].value;
    }

    mean /= population_size;

    std::vector<individual> res;
    for (int i = 0; i < population_size && !cap; i++)
    {
        int num = std::ceil((double) population[i].value/mean);
        for (int j = 0; j < num && !cap; j++)
        {
            res.push_back(population[i]);
            if (res.size() == population_size){cap = true;}
        }
    }

    return res;
}

void print_generation(std::vector<individual> population)
{
    for (int i = 0; i < population.size(); i++)
    {
        std::cout << "Individual #" << i << ": x(" << binaryToDecimal(population[i].x) << ") y(" << binaryToDecimal(population[i].y) << ") z("
        << binaryToDecimal(population[i].z) << ") val(" << population[i].value << ") chro(" << population[i].chromosome << ")" << std::endl;
    }
}

void print_generation_stats(std::vector<individual> population)
{
    float total, maxi;
    total = 0;
    maxi = population[0].value;
    for (int i = 0; i < population.size(); i++)
    {
        total += population[i].value;
        maxi = std::max(maxi, (float) population[i].value);
    }

    std::cout << "Mean: " << total/population.size() << std::endl << "Total: " << total << std::endl << "Max: " << maxi << std::endl;
}
