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
 * This program is completed by JieLiu for solving the GCP with the Tabu search,  
 * For more information about this algorithm, visit to: http://www.cnblogs.com/JiePro/
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
#include<math.h>
#include<conio.h>
#include<ctype.h>
#define VN 2000

//#define SHOWDEBUG

using namespace std;

char * File_Name;
FILE *fp ;
char outfilename[30];
int Seed ;
int Total_Iterations ;
int Max_Non_Improv_One = 2000000 ;
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

int N, K, G_K;  // node number and color number
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

 //             Colored = new int [N];
 //             Coloring_Cost = new int*[N];
 //             for (x = 0 ; x < N ; x++ )
 //                   Coloring_Cost[x]= new int[K];
//              Num_Avail_Colors = new int [N];

              A_Matrix = new Adjacent *[N];
              for( i = 0 ; i < N; i ++ )
                 {
                   A_Matrix[ i ] = new Adjacent ;
                   A_Matrix[ i ]->neighbor = 0 ;
                   A_Matrix[ i ]->next = NULL ;
                 }

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
                 A_Matrix[ x1 ]->neighbor ++ ;  //x1节点的相邻节点总数目
                 while( p1->next != NULL )
                      p1 = p1->next;
                 q1 = new Adjacent ;
                 q1->neighbor = x2 ;        //节点编号
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
	cout << "Input --data_file_name  --color_number  (--Max_Non_Improv_Step)" << endl;
	cout << "Just like: DSJC125.1.col 10 (2000000)" << endl;
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
	for(int i = 0; i < N; ++i)
	{
		srand (time(NULL));
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
	//Update f_best
	if(f < f_best)
	{
		f_best = f;
		for(int i = 0; i < N; ++i)
		{
			Best_Color[i] = Color[i];
		}
	}
	//Update Adjcaent_Color_Table
	Adjacent * p = A_Matrix[step.v];
	while(p->next)
	{
		p = p->next;
		Adjcaent_Color_Table[p->neighbor][step.sc] -= 1;
		Adjcaent_Color_Table[p->neighbor][step.dc] += 1;
	}
}

bool GetParam(int argc, char **argv)
{
	if(argc == 1)
	{
		K = 30;
		File_Name = "DSJC125.5.col";
		help();
	}
	else if(argc == 3)
	{
		File_Name = argv[1];
		K = atoi(argv[2]);
	}
	else if(argc == 4)
	{
		File_Name = argv[1];
		K = atoi(argv[2]);
		Max_Non_Improv_One = atoi(argv[3]);
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
		
		InitColor();
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
				if(Max_Non_Improv_One < breakcount) //Termination
				{
					break;
				}
			}
			else
			{
				breakcount = 0;
			}
			Update(step);   //update Adjcaent_Color_Table & remember the best solution 
		}
		if(Max_Non_Improv_One < breakcount)
		{
			cout<< "Can not find better one!!!" << endl;
			cout << "Best K: " << K+1 << endl;
			break;
		}
		for(int i =0; i < N; ++i)
		{
			Global_Best_Color[i] = Best_Color[i];
		}
		finish=clock();
		totaltime=(double)(finish-start)/CLOCKS_PER_SEC;
		cout << "K: " << K << "\tIteration: " << count << "\tTimeCost: "<< totaltime<<"s"<<endl;
		--K;
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

