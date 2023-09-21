#include <bits/stdc++.h>
#include <SFML/Graphics.hpp>
using namespace std;

#define inf 1000000000
#define ninf -1000000000
int max_depth = 2;

const int WIDTH = 800;
int screen[8][8];


void updateScreen(const vector<pair<int,int>>& pieces)
{

	memset(screen, 0, sizeof(screen));
	for(int i = 0; i < 24; i++)
	{
		if (pieces[i].first == -1 && pieces[i].second == -1) continue;
		if (i<12) screen[pieces[i].first][pieces[i].second] = 1;
		else screen[pieces[i].first][pieces[i].second] = 2;
	}
}

void drawScreen(sf::RenderWindow& win)
{
	int len = WIDTH / 8;
	int radius = len/2 - 10;
	for (int i = 0; i < WIDTH; i += len) {
		for (int j = 0; j < WIDTH; j += len) {
			sf::RectangleShape rect(sf::Vector2f(len, len));
			rect.setPosition(i, j);
			rect.setFillColor(((i/len)&1) == ((j/len)&1) ? sf::Color::Black : sf::Color::White);
			win.draw(rect);
			if (screen[j/len][i/len] == 1) {
				sf::CircleShape circle(radius);
				circle.setFillColor(sf::Color::Red);
				circle.setPosition(i+10, j+10);
				win.draw(circle);
			} else if (screen[j/len][i/len] == 2) {
				sf::CircleShape circle(radius);
				circle.setFillColor(sf::Color::Blue);
				circle.setPosition(i+10, j+10);
				win.draw(circle);
			}
		}		
	}

}


int CountPlayer(bool player, vector<vector<int>> table)
{
	int x = (player)? 0:1;
	int count =0;
	for(int i = 0; i < 8; i++)
		for(int j = 0; j < 8; j++)
			if(table[i][j] == x)count++;

	return count;
}
int evaluate(vector<vector<int>> table, bool Max)
{
	int nWhite = CountPlayer(1, table);
    	int nBlack = CountPlayer(0, table);

    	if (nWhite == 0) {
        	return (Max) ? ninf : inf;
    	} else if (nBlack == 0) {
        	return (Max) ? inf : ninf;
    	} else {
        	return (Max) ? (nWhite - nBlack) : (nBlack - nWhite);
    	}
	/*
	   int nWhite = CountPlayer(1,table);
	   int nBlack = CountPlayer(0,table);

	   if(Max) return (nWhite > nBlack)?nWhite:nBlack;
	   else return (nWhite > nBlack)? nBlack:nWhite;
	   */
}
vector<vector<int>> move(vector<vector<int>> table, pair<int,int> movimiento, int ficha,vector<pair<int,int>> &pieces)
{
	if(abs(abs(movimiento.first) - abs(pieces[ficha].first)) == 2 )

	{
		pair<int,int> dead = make_pair( pieces[ficha].first + (((pieces[ficha].first - movimiento.first) * -1)/2) , pieces[ficha].second +( (pieces[ficha].second - movimiento.second) * -1)/2 );

		for(int i = 0; i < 24; i++)
		{
			if(pieces[i].first == dead.first && pieces[i].second == dead.second)
			{
				table[dead.first][dead.second] = -1; 
				pieces[i].first = -1;
				pieces[i].second = -1;
			}
		}
	}

	table[pieces[ficha].first][pieces[ficha].second] = -1;
	pieces[ficha].first = movimiento.first;
	pieces[ficha].second = movimiento.second;	
	table[pieces[ficha].first][pieces[ficha].second] = (ficha>11)?0:1;

	return table;
}

vector<vector<pair<int,int>>> getMoves(vector<vector<int>> table, bool Max, vector<pair<int,int>> pieces)
{
	// x = f
	// y = cvector<pair<int,int>> piecees
	vector<vector<pair<int,int>>> result(24,vector<pair<int,int>>());
	if (Max)
	{
		for(int i = 0; i < 12; i++)
		{
			int f = pieces[i].first;
			int c = pieces[i].second;
			if (f != -1 && c != -1)
			{
				if(( c+1<8 && f+1 <8)) 
				{
					if(table[f+1][c+1] == -1)result[i].push_back(make_pair(f+1,c+1));

				}

				if(c+2<8 && f+2<8 && table[f+1][c+1] == 0 && table[f+2][c+2] == -1)result[i].push_back(make_pair(f+2,c+2));
				if(c-1>-1 && f+1< 8) 
				{
					if(table[f+1][c-1] == -1)result[i].push_back(make_pair(f+1,c-1));
				}
				if(c-2>-1 && f+2<8 && table[f+1][c-1] == 0 && table[f+2][c-2] == -1)result[i].push_back(make_pair(f+2,c-2));


			}



		}

	}
	else
	{
		for(int i = 12; i< 24;i++)
		{
			int f = pieces[i].first;
			int c = pieces[i].second;

			if (f == -1 && c == -1)continue;
			if(c+1<8 && f-1>-1)
			{
				if(table[f-1][c+1] == -1)result[i].push_back(make_pair(f-1,c+1));
			}	
			if(c+2 < 8 && f-2>-1 && table[f-1][c+1] == 1 && table[f-2][c+2] == -1)result[i].push_back(make_pair(f-2, c+2));
			if(c-1>-1 && f-1 >-1)
			{
				if(table[f-1][c-1] == -1)result[i].push_back(make_pair(f-1,c-1));

			}
			if(c-2 < 8 && f-2>-1 && table[f-1][c-1] == 1 && table[f-2][c-2] == -1)result[i].push_back(make_pair(f-2, c-2));
		}

	}

	if(pieces[22].first == 5 && pieces[22].second == 6) 
	{
		
		for(int i = 0; i < 12;i++)
		{
			for(int j = 0; j < result[i].size(); j++)
			{
				cout<<"XDXDXDLa ficha "<<i<<" -> ("<<result[i][j].first<<","<<result[i][j].second<<")"<<endl;
			}
		}
		cout<<"DEBUG pieza 0-> (" <<pieces[0].first<<","<<pieces[0].second<<")"<<endl;

	}

	return result;
}


// 1 = white = Max [0,11]
// 0 = black = Min [12,23]
pair<int,pair<int,pair<int,int>>> MinMax(vector<vector<int>> table, int depth, bool Max, int Alpha, int Beta,int ficha, pair<int,int> jugada, vector<pair<int,int>> pieces)
{
	if(depth == max_depth )
	{

		//	cout<<"La ficha "<<i<<" -> ("<<result[i][j].first<<","<<result[i][j].second<<")"<<endl;
		return make_pair(evaluate(table, Max),  make_pair(ficha, jugada));
	}
	if(Max)
	{
		int best = ninf;
		vector<vector<pair<int,int>>> Allmoves = getMoves(table, Max, pieces);
		for(int i =0; i< 12; i++ )
		{
			for(int j = 0; j < Allmoves[i].size();j++)
			{
				ficha = i;
				jugada = Allmoves[i][j];
				pair<int,pair<int,pair<int,int>>> val = MinMax(move(table,Allmoves[i][j],i, pieces),depth+1, false, Alpha,Beta, i, Allmoves[i][j],pieces);

				best = max(best,val.first);
				Alpha = max(Alpha,best);

				if(Beta <= Alpha){
					return make_pair(best,  make_pair(ficha, jugada));
				}


			}

		}
	}
	else
	{
		int best = inf;
		vector<vector<pair<int,int>>> Allmove = getMoves(table, Max, pieces);
		for(int i =12; i< 24; i++ )
		{
			for(int j = 0; j < Allmove[i].size();j++)
			{
				ficha = i;
				jugada = Allmove[i][j];

				pair<int,pair<int,pair<int,int>>> val = MinMax(move(table,Allmove[i][j],i, pieces),depth+1, true, Alpha,Beta, i, Allmove[i][j], pieces);

				best = min(best,val.first);
				Alpha = min(Alpha,best);

				if(Beta <= Alpha){
					return make_pair(best,  make_pair(ficha, jugada));
				}


			}

		}

	}


}

int main()
{
	vector<pair<int,int>> pieces;
	vector<vector<int>> table(8, vector<int>(8,-1));

	bool temp = true;
	for(int i = 0; i < 8; i++ )
	{
		for(int j = 0; j < 8; j++)
		{
			if(temp == false)
			{
				if(i == 0 || i== 1 || i==2)
				{
					table[i][j] = 1;
					pieces.push_back(make_pair(i,j));

				}
				if(i == 5 || i== 6 || i==7)
				{
					table[i][j] = 0;
					pieces.push_back(make_pair(i,j));
				}
			}
			temp = !temp;
		}
		temp = !temp;
	}



	//JUEGO SIMULACION 
	bool player = false;
	sf::RenderWindow window(sf::VideoMode(WIDTH, WIDTH), "MinMax");
	window.setFramerateLimit(60);
	int count = 30;
	while(window.isOpen())
	{
		++count;
		sf::Event event;
		while (window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					window.close();
					break;
				default:
					break;	
			}
		}
		window.clear();
		drawScreen(window);

		if (count >= 30) 
		{
			updateScreen(pieces);// cout<<endl;
			pair<int ,pair<int,pair<int,int>>> Movimiento =  MinMax(table, 0, player, inf, ninf,0,make_pair(0,0) ,pieces);
			
	if(pieces[22].first == 5 && pieces[22].second == 6)sleep(100); 
			//if(pieces[5].first == 7 && pieces[5].second == 4)sleep(100); 
			cout<<"El mejor movimiento es-> Ficha "<<Movimiento.second.first<<" -> ("<<Movimiento.second.second.first<<","<<Movimiento.second.second.second<<")"<<endl;

			//cout<<endl<<endl;
	
			table = move(table, Movimiento.second.second, Movimiento.second.first, pieces);
			player = !player;

			
			count = 0;

		}
		window.display();
	}
}
