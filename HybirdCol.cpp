                                                                                /*
 *  Algorithme ITS
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * This program demonstrates the use of the heuristic algorithm for solving
 * the problem of graph coloring. For more information about this algorithm,
 * email to: lu@info-univ.angers.fr.
 */

/*
 * This program is completed by JieLiu for solving the GCP with the Hybird  
 * Evolutionary algorithm search, For more information about this algorithm, 
 * visit to: http://www.cnblogs.com/JiePro/
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string.h>
#include <time.h>
#include <ctime>
#include <vector>
#include <list>
#include<math.h>
#include<ctype.h>
#define VN 2000

//#define SHOWDEBUG

using namespace std;

char * File_Name;
FILE *fp ;
char outfilename[30];
int Seed ;
int Total_Iterations ;
int Max_Non_Improv_One = 20000 ;
int Max_CrossOver_NO = 2000;
double starting_time, finishing_time, once_total_time, total_time ;

typedef struct Adjacent_Matrix{
         int neighbor ;
         struct Adjacent_Matrix *next ;
         }Adjacent ;
Adjacent * *A_Matrix ;


typedef struct NeighborStep{
	int v ; // vertex number
	int sc ; // src color
	int dc ; // dst color

	NeighborStep(int nv, int srccolor, int dstcolor)
	{
		v = nv;
		sc = srccolor;
		dc = dstcolor;
	}
	NeighborStep()
	{
		v = -1;
		sc = -1;
		dc = -1;
	}
}Nstep ;

struct Population
{
	int f_p;
	int *color;
};
Population *Popu;
list<int> *vertex_no[3];// vertex number with the same color

int N, K, P_N;  // node number and color number
int f, f_best, global_f_best;
int * Color; // color array for each vertex
int * Best_Color, * Global_Best_Color; // color array for each vertex
int ** Adjcaent_Color_Table;  // incremental matrix for each vertex being colored each color
int ** Edge;   // adjacent matrix
int ** TabuTenure;  // tabu tenure



/*****************************************************************************/
/*****************          1. Initializing           ************************/
/*****************************************************************************/
//1.1 Intializations
void Initializing()
{
     int i, j, x, y, x1, x2;
	 ifstream FIC;
     FIC.open(File_Name);
     if ( FIC.fail() )
     {
           cout << "### Erreur open, File_Name " << File_Name << endl;
           exit(0);
     }
     char StrReading[100];
     FIC >> StrReading;
     if ( FIC.eof() )
     {
           cout << "### Error open, File_Name " << File_Name << endl;
           exit(0);
     }
     //int nb_vtx=0 ;
     int nb_edg=-1, max_edg=0;
     while ( ! FIC.eof() )
     {
           char bidon[50];
           if ( strcmp(StrReading, "p" )==0 )
           {
              FIC >> bidon >> N >> nb_edg;
              cout << "Number of vertexes = " << N << endl;
              cout << "Number of edges = " << nb_edg << endl;

              Color = new int[N];
              Best_Color = new int[N];
              Global_Best_Color = new int[N];
           //   Move_Freq = new int[N];

              Edge=new int*[N];
              for (x = 0 ; x < N ; x++ )
                  Edge[x]= new int[N];

              A_Matrix = new Adjacent *[N];
              for( i = 0 ; i < N; i ++ )
                 {
                   A_Matrix[ i ] = new Adjacent ;
                   A_Matrix[ i ]->neighbor = 0 ;
                   A_Matrix[ i ]->next = NULL ;
                 }

			  Popu = new Population[P_N];
			  for(int i = 0; i < P_N; ++i)
			  {
				  Popu[i].color = new int[N];
			  }

			  vertex_no[0] = new list<int>[K]; // one of parents
			  vertex_no[1] = new list<int>[K]; // one of parents
			  vertex_no[2] = new list<int>[K]; // result of crossover


              Adjcaent_Color_Table=new int*[N];
              for (x=0;x<N;x++) Adjcaent_Color_Table[x]=new int[K];
                  TabuTenure=new int*[N];

			  for (x=0;x<N;x++) TabuTenure[x]=new int[K];

              for (x=0;x<N;x++)
                   for (y=0;y<N;y++)
                            {
                               Edge[x][y]=0;
                            }
              break ;
			}
			FIC >> StrReading;
        }

      while ( ! FIC.eof() )
         {
		   if ( strcmp(StrReading, "e")==0 )
           {
                 FIC >> x1 >> x2;
                 // cout << x1 << x2 << endl;
                 x1--; x2--;
                 if ( x1<0 || x2<0 || x1>=N || x2 >=N )
                 {
                       cout << "### Error of node : x1="
                            << x1 << ", x2=" << x2 << endl;
                       exit(0);
                 }
                 Edge[x1][x2]=Edge[x2][x1]=1;
                 max_edg++;
                 // add x2 to x1's neighbor list
                 Adjacent *p1, *q1;
				 p1 = A_Matrix[ x1 ] ;
                 A_Matrix[ x1 ]->neighbor ++ ;  //x1�ڵ�����ڽڵ�����Ŀ
                 while( p1->next != NULL )
                      p1 = p1->next;
                 q1 = new Adjacent ;
                 q1->neighbor = x2 ;        //�ڵ���
                 q1->next = NULL ;
                 p1->next = q1 ;

                 // add x1 to x2's neighbor list
                 p1 = A_Matrix[ x2 ] ;
                 A_Matrix[ x2 ]->neighbor ++ ;
                 while( p1->next != NULL )
                    p1 = p1->next;
                 q1 = new Adjacent ;
                 q1->neighbor = x1 ;
                 q1->next = NULL ;
                 p1->next = q1 ;
           }
           FIC >> StrReading;
     }
     cout << "Density = " << (float) 2 * max_edg/(N*(N-1)) << endl;
     if ( 0 && max_edg != nb_edg )
     {
           cout << "### Error de lecture du graphe, nbre aretes : annonce="
                 << nb_edg << ", lu=" << max_edg  << endl;
           exit(0);
     }

     FIC.close();
}

void help()
{
	cout << "Input --data_file_name  --color_number(K)  --population number (--Max_Iteration_NO)" << endl;
	cout << "Just like: DSJC125.1.col 10 20 (200000)" << endl;
}

void InitTenure()
{
	for (int x=0;x<N;x++) 
	{
		for (int y=0;y<K;y++)
		{
			TabuTenure[x][y] = 0;
		} 
	}
}

void InitColor()
{
	srand (time(NULL));
	for(int i = 0; i < N; ++i)
	{
		Color[i] = rand() % K ;
	}
}

void InitAdjcaent_Color_Table()
{
	Adjacent * p = NULL;
	
	for(int i = 0; i < N; ++i)
		for(int j = 0; j < K; ++j)
			Adjcaent_Color_Table[i][j] = 0;

	for(int i = 0; i < N; ++i)
	{
		p = A_Matrix[i];
		while(p->next)
		{
			p = p->next;
			Adjcaent_Color_Table[i][Color[p->neighbor]]++;
		}
	}

#ifdef SHOWDEBUG
	cout << "Adjcaent_Color_Table" << endl;
	for(int i = 0; i < N; ++i)
	{
		for(int j = 0; j < K; ++j)
			cout << Adjcaent_Color_Table[i][j] << " ";
		cout << endl;
	}
#endif
}

void InitConflictNumber()
{
	f = 0;
	for(int i = 0; i < N; ++ i)
	{
		f = f + Adjcaent_Color_Table[i][Color[i]];
	}
	f = f/2;
	f_best = f;
	global_f_best = f;

#ifdef SHOWDEBUG
	cout << "f: " << f << endl;
#endif
}


int NeighborMotion(Nstep & step, int currentcount)
{
	int Delta = 0,Delta_min = 65535,Delta_T = 0,Delta_T_min = 65535;
	vector<Nstep> steps,steps_T;
	for(int i = 0; i < N; ++i)
	{
		if(Adjcaent_Color_Table[i][Color[i]] != 0)
		{
			int m_color = 0;
			while (m_color < K)
			{
				if(m_color == Color[i])
				{
					++m_color;
					continue;
				}
				if(TabuTenure[i][m_color] > currentcount)  //for tabu NeighborMotion
				{
					Delta_T = Adjcaent_Color_Table[i][m_color] - Adjcaent_Color_Table[i][Color[i]];
					if (Delta_T < Delta_T_min)
					{
						Delta_T_min = Delta_T;
						steps_T.clear();
						steps_T.push_back(Nstep(i,Color[i],m_color));//remember moves(u,i,j), u-th vertex change color-i to color-j;
					}
					else if(Delta_T == Delta_T_min)
					{
						steps_T.push_back(Nstep(i,Color[i],m_color));//add to result
					}
				}
				else    //for normal NeighborMotion
				{
					Delta = Adjcaent_Color_Table[i][m_color] - Adjcaent_Color_Table[i][Color[i]];
					if (Delta < Delta_min)
					{
						Delta_min = Delta;
						steps.clear();
						steps.push_back(Nstep(i,Color[i],m_color));//remember moves(u,i,j), u-th vertex change color-i to color-j;
					}
					else if(Delta == Delta_min)
					{
						steps.push_back(Nstep(i,Color[i],m_color));//add to result
					}
				}
				++m_color;
			}
		}
	}

	if(f + Delta_T_min < f_best && Delta_T_min < Delta_min)
	{
		int k = rand();
		k = k % steps_T.size();
		step = steps_T[k];//choose to random one from tabu move
		f = f + Delta_T_min;
		return Delta_T_min;
	}
	else
	{		
		if(steps.size() == 0)
		{
			step = Nstep(-1,-1,-1);
		}
		else
		{
			int k = rand();
			k = k % steps.size();
			step = steps[k];//choose to random one from normal move
			f = f + Delta_min;
		}	
		return Delta_min;
	}
}

void Update(Nstep & step)
{
	//Update f
	if(f < f_best)
	{
		global_f_best = f_best = f;
		for(int i = 0; i < N; ++i)
		{
			Best_Color[i] = Color[i];
		}
	}
	//Update Delta_Matirix
	Adjacent * p = A_Matrix[step.v];
	while(p->next)
	{
		p = p->next;
		Adjcaent_Color_Table[p->neighbor][step.sc] -= 1;
		Adjcaent_Color_Table[p->neighbor][step.dc] += 1;
	}
}

int TabuCol(int Max_Non_Improv_Step)
{
	InitAdjcaent_Color_Table();
	InitTenure();
	InitConflictNumber();
	int count = 0;
	int breakcount = 0;
	
	while(f_best)
	{
		Nstep step;
		int delta = NeighborMotion(step,count);
		if(step.v == -1)
			continue;
		Color[step.v] = step.dc;
		
		TabuTenure[step.v][step.sc] = count + f + (rand() % 10);//*delta
			
		count++;
		if( f_best <= f)
		{
			++breakcount;
			if(Max_Non_Improv_Step < breakcount)
			{
				return 0;
			}
		}
		else
		{
			breakcount = 0;
		}
		Update(step);  //update Adjcaent_Color_Table & remember the best solution 
	}
	return count;
}

void InitPopulation()
{
	for(int i=0; i < P_N; ++i)
	{
		InitColor();
		TabuCol(20);
		if(f_best == 0)
		{
			global_f_best = 0;
			break;
		}
		for (int j=0; j < N; ++j)
		{
			Popu[i].color[j] = Best_Color[j];
		}
		Popu[i].f_p = f_best;
	}
}

void CrossOver()
{
	for(int i = 0; i < K; ++i)
	{
		vertex_no[0][i].clear();
		vertex_no[1][i].clear();
		vertex_no[2][i].clear();
	}

	int n = rand() % P_N;
	int m;
	while((m = rand() % P_N) == n);
	
	for(int i = 0; i < N; ++i)
	{
		//random choose two parents, puts the vertex number into a same group according to the color
		vertex_no[0][Popu[n].color[i]].push_back(i);
		vertex_no[1][Popu[m].color[i]].push_back(i);
	}
	int count = 0, NewColorIndex = 0;
	bool EmptyFlag0 = 0,EmptyFlag1 = 0;
	int total_add_in = 0;

	while (total_add_in != N)
	{
		int max = 0,index0 = 0,index1 = 0;
		vector<int> index;
		if(count%2 == 0 && !EmptyFlag0)
		{
			index0 = 0;
			for(int i = 0; i < K; ++i)
			{
				int temp = vertex_no[0][i].size();
				if(temp == 0)
					continue;
				if(max < temp)
				{
					max = temp;
					index.clear();
					index.push_back(i);
				}
				else if(max == temp)
				{
					index.push_back(i);
				}
			}
			if(index.size() == 0)
			{
				++count;
				EmptyFlag0 = true;
				continue;
			}
			else
				index1 = index[rand()%index.size()];
		}
		else if(count%2 == 1 && !EmptyFlag1)
		{
			index0 = 1;
			for(int i = 0; i < K; ++i)
			{
				int temp = vertex_no[1][i].size();
				if(temp == 0)
					continue;
				if(max < temp)
				{
					max = temp;
					index.clear();
					index.push_back(i);
				}
				else if(max == temp)
				{
					index.push_back(i);
				}
			}
			if(index.size() == 0)
			{
				++count;
				EmptyFlag1 = true;
				continue;
			}
			else
				index1 = index[rand()%index.size()];
		}
		else
		{
			++count;
			continue;
		}
		
		if(NewColorIndex < K)
		{
			vertex_no[2][NewColorIndex] = vertex_no[index0][index1];
			++NewColorIndex;
		}
		else
		{
			//all color of offspring has vertex,then add the rest of vertex to a random color
			int k = rand()%K;
			for(list<int>::iterator it = vertex_no[index0][index1].begin(); it != vertex_no[index0][index1].end(); ++it)
			{
				vertex_no[2][k].push_back(*it);
			}	
		}
	
		int size = vertex_no[index0][index1].size();
		total_add_in += size;
		count++;
		
		//erase the vertexs added in the offspring
		for(list<int>::iterator it1 = vertex_no[index0][index1].begin(); it1 != vertex_no[index0][index1].end();)
		{
			bool flag_found = false;
			for(int i = 0; i < K; ++i)
			{
				for(list<int>::iterator it2 = vertex_no[1-index0][i].begin(); it2 != vertex_no[1-index0][i].end();)
				{
					if(*it1 == *it2)
					{
						it2 = vertex_no[1-index0][i].erase(it2);
						flag_found = true;
						break;
					}
					else
					{
						++it2;
					}
				}
				if(flag_found)
					break;
			}
			++it1;
		}
		vertex_no[index0][index1].clear();
	}

	//put the offspring to the current s
	for(int i = 0; i < K; ++i)
		for(list<int>::iterator it = vertex_no[2][i].begin(); it != vertex_no[2][i].end(); ++it)
		{
			Color[*it]=i;
		}
}

void UpdatePopulation()
{
	//add the result to the population,and delete the solution with max f(S) in the population randomly.
	int max = 0;
	vector<int> index;
	for(int i = 0; i < P_N; ++i)
	{
		if(max < Popu[i].f_p)
		{
			max = Popu[i].f_p;
			index.clear();
			index.push_back(i);
		}
		else if(max = Popu[i].f_p)
		{
			index.push_back(i);
		}
	}
	int k = rand() % index.size();
	int ind = index[k];
	Popu[ind].f_p = f_best;
	for(int i = 0; i < N; ++i)
	{
		Popu[ind].color[i] = Best_Color[i];
	}
}

bool GetParam(int argc, char **argv)
{
	if(argc == 1)
	{
		K = 18;
		strcpy(File_Name, "/home/haimin/CLionProjects/yunchouxiu/instances/DSJC500.5.col");
		P_N = 10;
		help();
	}
	else if(argc == 4)
	{
		File_Name = argv[1];
		K = atoi(argv[2]);
		P_N = atoi(argv[3]); //population number
	}
	else if( argc == 5)
	{
		File_Name = argv[1];
		K = atoi(argv[2]);
		P_N = atoi(argv[3]);
		int temp = atoi(argv[4]);
		temp = sqrt((double)temp/10.0) ;    // temp = Max_CrossOver_NO * Max_Non_Improv_One & Max_Non_Improv_One = 10 * Max_CrossOver_NO;
		Max_Non_Improv_One = temp * 10;
		Max_CrossOver_NO = temp;
	}
	else
	{
		help();
		return 0;
	}
	return 1;
}
/*****************************************************************************/
/*****************            Main Function           ************************/
/*****************************************************************************/
int main(int argc, char **argv)
{
	if(GetParam(argc,argv) == 0)
		return 0;

	Initializing();
	
	while(K)
	{		
		clock_t start,finish;
		double totaltime;
		start=clock();
		
		InitPopulation();
		int count = 0,count_in_Tabu = 0;
		while(global_f_best && count < Max_CrossOver_NO)
		{
			count++;
			CrossOver();
			count_in_Tabu = TabuCol(Max_Non_Improv_One); //Max_Non_Improv_One 20W ,return 0,when no find;return iteration number > 0,find the solution
			if(count_in_Tabu) 
				break;
			else
				UpdatePopulation();
		}
		
		
		if(global_f_best == 0)
		{
			finish=clock();
			totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
			for(int i =0; i < N; ++i)
			{
				Global_Best_Color[i] = Best_Color[i];
			}
			int iterNo = (count-1)*Max_Non_Improv_One + count_in_Tabu;
			cout << "K: " << K << "\tIteration: " << iterNo << "\tTimeCost: "<< totaltime<<"s"<<endl;
			--K;
		}
		else
		{
			cout<< "Can not find better one!!!" << endl;
			cout << "Best K: " << K+1 << endl;
			break;
		}
		
	}

#ifdef SHOWDEBUG
	cout <<  "Vertices  Color" << endl;
	for(int i = 0; i < N; ++ i)
	{
		cout << i << "---" << Global_Best_Color[i] << "\t";
		if (i % 5 == 4)
		{
			cout << endl;
		}
	}
#endif
	return 1;
}

