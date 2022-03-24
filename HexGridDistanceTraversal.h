#pragma once
#include <iostream>
#include <vector>
#include <stack>
#include <queue>
//@A327
// Faster distance based Hex Grid traversal from source to destination including blocked cell avoidance. 
// 
// Hex Grid Cell
class Cell {
private:
    int x;
    int y;
    int grid_index{ -1 };
    bool traversed{ false };
    int cost{ INT_MAX - 1 };
    int move{ 0 };
public:
    Cell(int X, int Y) :
        x(X),
        y(Y)
    {
        grid_index = -1;
    }

    const int getX() const { return x; }
    const int getY() const { return y; }
    void setCost(const Cell& target)
    {
        int xCost = x - target.x;
        int yCost = y - target.y;
        cost = xCost * xCost + yCost * yCost;
    }
    const int getCost() const { return cost; }
    bool isTraversable()
    {
        if (traversed)
        {
            return false;
        }
        else
        {
            return cost != INT_MAX;
        }
    }
    void setBlocked()
    {
        cost = INT_MAX;
    }
    void setTraversed(const bool& val)
    {
        traversed = val;
    }
    void setGridIndex(int index)
    {
        grid_index = index;
    }
    bool operator == (const Cell& other)
    {
        return ((this->x == other.x) && (this->y == other.y));
    }
    bool operator != (const Cell& other)
    {
        return ((this->x != other.x) || (this->y != other.y));
    }
};

class HexGrid {
private:
    std::vector<Cell> grid;
    int minX;
    int minY;
    int Xt;
    int Yt;
public:
    HexGrid(const int minX, const int minY, const int maxX, const int maxY, const std::vector<Cell>& blockedCells)
    {
        this->minX = minX;
        this->minY = minY;
        Xt = maxX - minX + 1;
        Yt = maxY - minY + 1;

        int sz = blockedCells.size();
        for (int j = minY; j <= maxY; j++)
        {
            for (int i = minX; i <= maxX; i++)
            {
                Cell c(i, j);
                c.setGridIndex(grid.size());
                if (sz > 0)
                {
                    for (int k = 0; k < sz; k++)
                    {
                        if (c == blockedCells[k])
                        {
                            c.setBlocked();
                        }
                    }
                }
                grid.push_back(c);
            }
        }
    }
    size_t Index(int x, int y)
    {
        if ((x >= grid[0].getX()) && (x <= grid[grid.size() - 1].getX()))
        {
            if ((y >= grid[0].getY()) && (y <= grid[grid.size() - 1].getY()))
            {
                size_t index = (y - minY) * Xt + (x - minX);
                if ((index >= 0) && (index < grid.size()))
                {
                    return index;
                }
            }
        }
        return -1;
    }

    std::vector<int> getNeighbours(const int& cellIndex, const bool& traversable)
    {
        std::vector<int> neighbours;
        for (int i = -1; i < 2; i++)
        {
            for (int j = -1; j < 2; j++)
            {
                if (i == j)
                {
                    continue;
                }
                int index = Index(grid[cellIndex].getX() + i, grid[cellIndex].getY() + j);
                if (index >= 0)
                {
                    if (traversable)
                    {// return only traversable neighbours
                        if (grid[index].isTraversable())
                        {
                            neighbours.push_back(index);
                        }
                    }
                    else
                    {// return all neighbours
                        neighbours.push_back(index);
                    }
                }
            }
        }
        return neighbours;
    }
    bool getNextInPath(const int& cellIndex, std::stack<int>& movementStack)
    {
        grid[cellIndex].setTraversed(true);
        std::vector<int> cells = getNeighbours(cellIndex, true);
        int minCost = INT_MAX;
        for (size_t i = 0; i < cells.size(); i++)
        {
            int c = grid[cells[i]].getCost();
            if (minCost > c)
            {
                minCost = c;
            }
        }
        if (minCost < INT_MAX)
        {
            for (size_t i = 0; i < cells.size(); i++)
            {
                int c = grid[cells[i]].getCost();
                if (minCost == c)
                {
                    int index = this->Index(grid[cells[i]].getX(), grid[cells[i]].getY());
                    movementStack.push(index);
                }
            }
            return true;
        }
        else
        {
            return false;
        }
    }
    std::queue<int> findPath(const Cell& source, const Cell& destination)
    {
        std::queue<int> path;
        std::queue<int> cur_path;
        std::stack<int> movement;
        int sourceIndex = getGridIndex(source);
        int targetIndex = getGridIndex(destination);
        int minMoves;
        int curMoves;
        auto reset = [&]() {
            grid[sourceIndex].setTraversed(false);
            grid[targetIndex].setTraversed(false);
            while (!movement.empty())
            {
                movement.pop();
            }
            while (!cur_path.empty())
            {
                cur_path.pop();
            }
            movement.push(sourceIndex);
            curMoves = 0;
        };
        if (sourceIndex > -1)
        {
            setDestination(destination);
            movement.push(sourceIndex);
            minMoves = INT_MAX;
            curMoves = 0;
            while (!movement.empty())
            {
                int curIndex = movement.top();
                movement.pop();
                if (grid[curIndex].getCost() == 0)
                {
                    if (minMoves > curMoves)
                    {
                        while (!path.empty())
                        {
                            path.pop();
                        }
                        while (!cur_path.empty())
                        {
                            path.push(cur_path.front());
                            cur_path.pop();
                        }
                        minMoves = curMoves;
                        if (minMoves < 2)
                        {
                            break;
                        }
                    }
                    reset();
                }
                else
                {
                    if (getNextInPath(curIndex, movement))
                    {
                        curMoves++;
                        if (curMoves > minMoves)
                        {
                            // no need to move as we already a less moves path. 
                            reset();
                        }
                        else
                        {
                            cur_path.push(curIndex);
                        }
                    }
                    else
                    {
                        // stuck
                        if (curIndex == sourceIndex)
                        {
                            break;
                        }
                        else
                        {
                            reset();
                        }
                    }
                }
            }
        }
        return path;
    }

    void setDestination(const Cell& destination)
    {
        int sz = grid.size();
        for (int i = 0; i < sz; i++)
        {
            if (grid[i].isTraversable())
            {
                grid[i].setCost(destination);
            }
        }
    }
    int getGridIndex(const Cell& source)
    {
        int sz = grid.size();
        for (int i = 0; i < sz; i++)
        {
            if (grid[i] == source)
            {
                return i;
            }
        }
        return -1;
    }
    void resetGridTraversal()
    {
        int sz = grid.size();
        for (int i = 0; i < sz; i++)
        {
            grid[i].setTraversed(false);
        }
    }
};

void test() {
    /*
    Cell source(8, 8);
    Cell destination(-9, -9);
    std::vector<Cell> blockedCells= {
        Cell(7,8), 
        Cell(7,7), 
        Cell(6, 7)
    };
    HexGrid grid(-10, -10, 10, 10, blockedCells);
    */
    /*
    Cell source(17, 18);
    Cell destination(18, 19);
    std::vector<Cell> blockedCells= {
        Cell(17,19),
        Cell(18,18),
        Cell(19, 18)
    };
    HexGrid grid(15, 15, 20, 20, blockedCells);
    */
    /*
    Cell source(18, 18);
    Cell destination(-19, -19);
    std::vector<Cell> blockedCells = {
        Cell(17,19),
        Cell(18,18),
        Cell(19, 18)
    };
    HexGrid grid(-21, -21, 21, 20, blockedCells);
    */
    
    Cell source(-1, -1);
    Cell destination(0, 0);
    std::vector<Cell> blockedCells = {
        Cell(-1,0),
        Cell(0,-1),
        Cell(1, -1)
    };
    HexGrid grid(-3, -3, 2, 2, blockedCells);
    

    std::queue<int> path = grid.findPath(source, destination);
    int moves = 0;
    while (!path.empty())
    {
        int i = path.front();
        moves++;
        path.pop();
    }
    std::cout << "Number of moves: " << moves << std::endl;
}
