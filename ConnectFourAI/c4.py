import os
import time
import copy
import random

class Grid(object):
    def __init__(self, xDim=7, yDim=6):
        self.xDim = xDim
        self.yDim = yDim
        self.grid = [[0 for y in range(yDim)] for x in range(xDim)]
        for y in range(yDim):
            for x in range(xDim):
                self.grid[x][y] = '_'
    def __getitem__(self, xind, yind):
        if xind < 0 or yind < 0:
            raise IndexError
        else:
            return self.grid[xind][yind]

    def __setitem__(self, xind, yind, value):
        self.grid[xind][yind] = value

    def printGrid(self):
        os.system('clear')
        for x in range(self.xDim):
            print(' ', x+1, end='')
        print()
        for y in range(self.yDim):
            for x in range(self.xDim):
                print(' ', self.grid[x][y], end='')
            print()

    def checkTerminal(self):
        #check column
        for x in range(self.xDim):
            for y in range(self.yDim-3):
                if(not self.grid[x][y] == '_'
                and self.grid[x][y] == self.grid[x][y+1]
                and self.grid[x][y+1] == self.grid[x][y+2]
                and self.grid[x][y+2] == self.grid[x][y+3]):
                    self.grid[x][y] = self.grid[x][y].upper()
                    self.grid[x][y+1] = self.grid[x][y+1].upper()
                    self.grid[x][y+2] = self.grid[x][y+2].upper()
                    self.grid[x][y+3] = self.grid[x][y+3].upper()
                    return self.grid[x][y].lower()
        #check row
        for x in range(self.xDim-3):
            for y in range(self.yDim):
                if(not self.grid[x][y] == '_'
                and self.grid[x][y] == self.grid[x+1][y]
                and self.grid[x+1][y] == self.grid[x+2][y]
                and self.grid[x+2][y] == self.grid[x+3][y]):
                    self.grid[x][y] = self.grid[x][y].upper()
                    self.grid[x+1][y] = self.grid[x+1][y].upper()
                    self.grid[x+2][y] = self.grid[x+2][y].upper()
                    self.grid[x+3][y] = self.grid[x+3][y].upper()
                    return self.grid[x][y].lower()
        #check diagonals descending
        for x in range(self.xDim-3):
            for y in range(self.yDim-3):
                if(not self.grid[x][y] == '_'
                and self.grid[x][y] == self.grid[x+1][y+1]
                and self.grid[x+1][y+1] == self.grid[x+2][y+2]
                and self.grid[x+2][y+2] == self.grid[x+3][y+3]):
                    self.grid[x][y] = self.grid[x][y].upper()
                    self.grid[x+1][y+1] = self.grid[x+1][y+1].upper()
                    self.grid[x+2][y+2] = self.grid[x+2][y+2].upper()
                    self.grid[x+3][y+3] = self.grid[x+3][y+3].upper()
                    return self.grid[x][y].lower()
        #check diagonals ascending
        for x in range(self.xDim-3):
            for y in range(self.yDim-3):
                if(not self.grid[x][y+3] == '_'
                and self.grid[x][y+3] == self.grid[x+1][y+2]
                and self.grid[x+1][y+2] == self.grid[x+2][y+1]
                and self.grid[x+2][y+1] == self.grid[x+3][y]):
                    self.grid[x][y+3] = self.grid[x][y+3].upper()
                    self.grid[x+1][y+2] = self.grid[x+1][y+2].upper()
                    self.grid[x+2][y+1] = self.grid[x+2][y+1].upper()
                    self.grid[x+3][y] = self.grid[x+3][y].upper()
                    return self.grid[x][y+3].lower()

    def selection(self, sel, player):
        if not self.grid[sel-1][0] == '_':
            pass
        elif self.grid[sel-1][self.yDim-1] == '_':
            self.grid[sel-1][self.yDim-1] = player
        else:
            for y in range(1, self.yDim):
                if not self.grid[sel-1][y] == '_':
                    self.grid[sel-1][y-1] = player
                    break

class Game(object):
    def __init__(self, players=2):
        self.players = players
        self.grid = Grid()

    def onePlayerGame(self):
        sel = 9999 # arbitrarily high
        turn = 1
        ai = AI()
        while(sel):
            self.grid.printGrid()
            if(turn == 1): #player turn
                sel = int(input('\n  Enter a row, 0 to quit: ')) #add exception thingy
                self.grid.selection(sel, 'x')
                turn = 2
                win = self.grid.checkTerminal()
                if(win == 'x'):
                    self.grid.printGrid()
                    print('X wins !')
                    time.sleep(5)
                    sel = 0
            else: #computer turn
                # Alpha-Beta selection call
                sel = ai.alphaBeta(self.grid, 5)
                print(sel)
                self.grid.selection(sel, 'o')
                turn = 1
                win = self.grid.checkTerminal()
                if(win == 'o'):
                    self.grid.printGrid()
                    print('O wins!')
                    time.sleep(5)
                    sel = 0

class AI(object):
    def __init__(self, label='Artie'):
        self.label = label

    def alphaBeta(self, grid, depth):
        value = self.maxValue(grid, 0, 9999, depth)
        return value[0]+1

    def maxValue(self, grid, alpha, beta, depth):
        if grid.checkTerminal() == 'o':
            return [None, self.utility2(grid, 'o')]
        value = [None, -9999]
        for x in range(grid.xDim):
            testGrid = copy.deepcopy(grid)
            testGrid.selection(x+1, 'o')
            testVal = self.utility2(testGrid, 'o')
            if depth > 0:
                if value[1] < self.minValue(testGrid, alpha, beta, depth-1)[1]:
                    value[1] = self.minValue(testGrid, alpha, beta, depth-1)[1]
                    value[0] = x
                if value[1] >= beta:
                    return value
                if value[1] > alpha:
                    alpha = value[1]
            else:
                if value[1] < testVal:
                    value[1] = testVal
                    value[0] = x
        return value

    def minValue(self, grid, alpha, beta, depth):
        if grid.checkTerminal() == 'x':
            return [None, self.utility2(grid, 'x')]
        value = [None, 9999]
        for x in range(grid.xDim):
            testGrid = copy.deepcopy(grid)
            testGrid.selection(x+1, 'x')
            testVal = self.utility2(testGrid, 'x')
            if depth > 0:
                if value[1] > self.maxValue(testGrid, alpha, beta, depth-1)[1]:
                    value[1] = self.maxValue(testGrid, alpha, beta, depth-1)[1]
                    value[0] = x
                if value[1] <= alpha:
                    return value
                if value[1] < beta:
                    beta = value[1]
            else:
                if value[1] > testVal:
                    value[1] = testVal
                    value[0] = x
        return value
    #returns utility value of a state based on stuff
    def utility(self, grid, player): # player should be char representing player
        utilVal = 0
        for x in range(grid.xDim):
            for y in range(grid.yDim):
                if grid.grid[x][y] == player:
                    try:
                        for z in range(1,4):
                            if grid.grid[x][y+z] == player:
                                utilVal += 3
                            elif grid.grid[x][y+z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x+z][y+z] == player:
                                utilVal += 3
                            elif grid.grid[x+z][y+z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x+z][y] == player:
                                utilVal += 3
                            elif grid.grid[x+z][y] == '_':
                                utilVal += 1
                            else:
                                #utilVal -=3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x+z][y-z] == player:
                                utilVal += 3
                            elif grid.grid[x+z][y-z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x][y-z] == player:
                                utilVal += 3
                            elif grid.grid[x][y-z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x-z][y-z] == player:
                                utilVal += 3
                            elif grid.grid[x][y-z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x-z][y] == player:
                                utilVal += 3
                            elif grid.grid[x-z][y] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
                    try:
                        for z in range(1,4):
                            if grid.grid[x-z][y+z] == player:
                                utilVal += 3
                            elif grid.grid[x-z][y+z] == '_':
                                utilVal += 1
                            else:
                                #utilVal -= 3
                                break
                    except IndexError:
                        pass
        return utilVal

    def utility2(self, grid, player):
        utilVal = 0
        #check columns
        checklist = []
        for x in range(grid.xDim):
            for y in range(grid.yDim-3):
                for z in range(4):
                    checklist.append(grid.grid[x][y+z])
        for x in checklist:
            if x == player:
                utilVal += 1
            elif x == '_':
                pass
            else:
                utilVal -= 1

        #check rows
        checklist = []
        for x in range(grid.xDim-3):
            for y in range(grid.yDim):
                for z in range(4):
                    checklist.append(grid.grid[x+z][y])
        for x in checklist:
            if x == player:
                utilVal += 1
            elif x == '_':
                pass
            else:
                utilVal -= 1

        #check diagonals descending
        checklist = []
        for x in range(grid.xDim-3):
            for y in range(grid.yDim-3):
                for z in range(4):
                    checklist.append(grid.grid[x+z][y])
        for x in checklist:
            if x == player:
                utilVal += 1
            elif x == '_':
                pass
            else:
                utilVal -= 1

        #check diagonals ascending
        checklist = []
        for x in range(grid.xDim-3):
            for y in range(grid.yDim-3):
                for z in range(4):
                    checklist.append(grid.grid[x+z][y+3-z])
        for x in checklist:
            if x == player:
                utilVal += 1
            elif x == '_':
                pass
            else:
                utilVal -= 1
        return utilVal


## ~~~~~~~~~ Main function ~~~~~~~~~ ##
game = Game()
game.onePlayerGame()
