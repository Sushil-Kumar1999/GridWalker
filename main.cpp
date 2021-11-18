//To be completed:
// Student id:  967565
// Student name: Sushil Kumar Kumar Satheesh Kumar
// date: 19th November 2021

#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

#define N 200
//#define N 1000
#define S 25
#define MAX_WALKERS_PER_LOCATION 3
#define MAX_WALKERS_PER_EDGE 4

typedef struct Walker
{
    int currentX, currentY, finalX, finalY;
    bool hasArrived;
    void Init() 
    {
        currentX = rand() % S;
        currentY = rand() % S;
        finalX = rand() % S;
        finalY = rand() % S;
        hasArrived = false;
    }
} ;

int originalGridCount[S][S];  //used to make sure the number of walkers per location is within the limits. Do not modify!
int finalGridCount[S][S];  //used to set the result target.  Do not modify!
int obtainedGridCount[S][S];  //something you may use for the results
Walker walkers[N];

void Lock(std::mutex* m)
{
    m->lock();
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); //Sleeps to simulate longer execution time and increase the probability of an issue
}

void Unlock(std::mutex* m)
{
    m->unlock();
}

void PrintGrid(std::string message, int grid[S][S])
{
    std::cout << message;
    for (int i = 0; i < S; i++)
    {
        for (int j = 0; j < S; j++)
            std::cout << (grid[i][j]<10?"  " : " ") << grid[i][j]; //Initialising grids
        std::cout << "\n";
    }
}

void SetObtainedGrid()
{
    for (int i = 0; i < N; i++) //Initialising walkers' locations
    {
        obtainedGridCount[walkers[i].currentY][walkers[i].currentX]++;
        if (!walkers[i].hasArrived)
        {
            std::cout << "\nAt least one walker had not arrived!\n";
            return;
        }
    }
}

void CompareGrids(int a[S][S], int b[S][S])
{
    for (int i = 0; i < S; i++)
        for (int j = 0; j < S; j++)
            if (a[i][j] != b[i][j])
            {
                std::cout<<"\nError: results are different!\n";
                return;
            }
    std::cout << "\nSeems to be OK!\n";
}


// ####################   Write Most of your code here ################################################################
std::mutex mutex;
std::mutex walkerMutexes[N];
std::mutex locMutexes[S][S];
int tc = 0;
// 0 -> North
// 1 -> South
// 2 -> East
// 3 -> West
int GetRandomDirection(int id)
{
    std::vector<int> directions = { 0, 1, 2, 3 };
    if (walkers[id].currentY == 0)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), 0));
    }
    if (walkers[id].currentY == (S - 1))
    {
        directions.erase(std::remove(directions.begin(), directions.end(), 1));
    }
    if (walkers[id].currentX == 0)
    {
        directions.erase(std::remove(directions.begin(), directions.end(), 3));
    }
    if (walkers[id].currentX == (S - 1))
    {
        directions.erase(std::remove(directions.begin(), directions.end(), 2));
    }

    int index = rand() % directions.size();
    return directions.at(index);
}

void MoveWalker(int id, int direction)
{
    if (direction == 0) // North
    {
        walkers[id].currentY--;
    }
    else if (direction == 1) // South
    {
        walkers[id].currentY++;
    }
    else if (direction == 2) // East
    {
        walkers[id].currentX++;
    }
    else // West
    {
        walkers[id].currentX--;
    }
}

bool HasReachedFinalDestination(int id)
{
    bool hasReachedfinalY = walkers[id].currentY == walkers[id].finalY;
    bool hasReachedfinalX = walkers[id].currentX == walkers[id].finalX;
    return hasReachedfinalY && hasReachedfinalX;
}

int GetWalkersAtLocation(int y, int x)
{
    int count = 0;
    for (int i = 0; i < N; i++)
    {
        if (walkers[i].currentY == y && walkers[i].currentX == x)
        {
            count++;
        }
    }

    return count;
}

void WalkerI(int id)
{
    while (!HasReachedFinalDestination(id))
    {
        int direction = GetRandomDirection(id);
        int currentX = walkers[id].currentX;
        int currentY = walkers[id].currentY;
        int nextX, nextY;
        
        if (direction == 0) // North
        {
            nextX = walkers[id].currentX;
            nextY = walkers[id].currentY - 1;
        }
        else if (direction == 1) // South
        {
            nextX = walkers[id].currentX;
            nextY = walkers[id].currentY + 1;
        }
        else if (direction == 2) // East
        {
            nextX = walkers[id].currentX + 1;
            nextY = walkers[id].currentY;
        }
        else // West
        {
            nextX = walkers[id].currentX - 1;
            nextY = walkers[id].currentY;
        }

        //Lock(&locMutexes[currentY][currentX]);
        //Lock(&locMutexes[nextY][nextX]);
        if (GetWalkersAtLocation(nextY, nextX) < MAX_WALKERS_PER_LOCATION)
        {
            MoveWalker(id, direction);
        }
        //Unlock(&locMutexes[nextY][nextX]);
        //Unlock(&locMutexes[currentY][currentX]);
        

    }
    walkers[id].hasArrived = true;
    Lock(&mutex);
    //std::cout << "Walker " << id << " has arrived"<<std::endl;
    std::cout << "Threads Completed: " << ++tc << std::endl;
    Unlock(&mutex);
}

// ####################   End of your code ################################################################


void InitGame()
{
    for (int i = 0; i < S; i++)
        for (int j = 0; j < S; j++)
            originalGridCount[i][j] = finalGridCount[i][j] = obtainedGridCount[i][j] = 0; //Initialising grids
    for (int i = 0; i < N; i++) //Initialising walkers' locations
    {
        do walkers[i].Init();
        while (originalGridCount[walkers[i].currentY][walkers[i].currentX]>= MAX_WALKERS_PER_LOCATION);
        originalGridCount[walkers[i].currentY][walkers[i].currentX]++;
    }
    for (int i = 0; i < N; i++) //Initialising walkers' locations
        finalGridCount[walkers[i].finalY][walkers[i].finalX]++;
}

int main()
{
    InitGame();

    //Start your threads here.
    srand(time(NULL));
    std::thread t[N];
    for (int i = 0; i < N; i++)
    {
        t[i] = std::thread(WalkerI, i);
    }
    for (int i = 0; i < N; i++)
    {
        t[i].join();
    }

    PrintGrid("Original locations:\n\n", originalGridCount);
    PrintGrid("\n\nIntended Result:\n\n", finalGridCount);
    SetObtainedGrid();
    PrintGrid("\n\nObtained Result:\n\n", obtainedGridCount);
    CompareGrids(finalGridCount, obtainedGridCount);
}
