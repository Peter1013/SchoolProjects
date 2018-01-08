""" 
	Originally written by Professor David Claveau, altered by 
	Pedro De Los Reyes to fit the requirements of assignment 3
"""

from MDP import ValueIteration, ValueIterationStep, PolicyExtraction, QExtraction, PrintDict
from GridMDP import GridMDP, orientations
from copy import copy, deepcopy
import time
import random

k = 0.6
ghostPos = [(1, 0), (0, 1), (1, 2), (2, 1)]
ghostFlag = 0
gridworld = GridMDP( [[0.1, 0.1, 0.1, 0.1],
                      [0.1, 0.1,0.1, 0.1],
                      [0.1, 0.1, 0.1, 0.1]], terminals=[((3, 2),1,(1,0)), ((3, 1),-1,(1,0))] )

dots = [[True, True, True, True],
        [True, True, True, True],
        [True, True, True, True]]
activeDots = deepcopy(dots)
# gridworld = GridMDP( [[0.0, 0.0, 0.0, 0.0, 0.0],
#                      [0.0, None,0.0, 0.0, 0,0],
#                      [0.0, 0.0, 0.0, None, 0,0],
#                      [0.0, 0.0, 0.0, 0.0, 0.0]], terminals=[((4, 3),1,(1,0)), ((4, 2),-1,(1,0))] )

waiting = True
def onclick(event):
    global waiting, a
    waiting = not waiting
    if event.key == 'right':
        a = 0
    elif event.key == 'up':
        a = 1
    elif event.key == 'left':
        a = 2
    elif event.key == 'down':
        a = 3

def dot(s, a):
    global gridworld
    ns = (s[0] + a[0], s[1] + a[1])
    reward = 0
    try:
        if(activeDots[ns[0]+1][ns[1]]):
            reward += 0.25
    except IndexError:
        pass
    try:
        if(activeDots[ns[0]][ns[1]+1]):
            reward += 0.25
    except IndexError:
        pass
    try:
        if(activeDots[ns[0]-1][ns[1]]):
            reward += 0.25
    except IndexError:
        pass
    try:
        if(activeDots[ns[0]][ns[1]-1]):
            reward += 0.25
    except IndexError:
        pass
    return reward

def gst(s, a, gpos):
    ns = (s[0] + a[0], s[1] + a[1])
    if ns == (2, gpos):
        return -1
    try:
        if((ns[0]+1, ns[1]) == (2,gpos)):
            return -0.5
    except IndexError:
        pass
    try:
        if((ns[0], ns[1]+1) == (2,gpos)):
            return -0.5
    except IndexError:
        pass
    try:
        if((ns[0]-1, ns[1]) == (2,gpos)):
            return -0.5
    except IndexError:
        pass
    try:
        if((ns[0], ns[1]-1) == (2,gpos)):
            return -0.5
    except IndexError:
        pass
    return 0

def DrawPacman(s):
    circle = plt.Circle((s[0]*CSTEP+CSTEP/4, s[1]*RSTEP+RSTEP/8), radius=RSTEP/16, ec='black', fc='yellow', zorder=2)
    plt.gca().add_patch(circle)

def ErasePacman(s):
    circle = plt.Circle((s[0]*CSTEP+CSTEP/4, s[1]*RSTEP+RSTEP/8), radius=RSTEP/15.6, ec='white', fc='white', zorder=2)
    plt.gca().add_patch(circle)

def DrawGhost(s):
    gh = plt.Circle((2*CSTEP+CSTEP/4, s[1]*RSTEP+2*RSTEP/8), radius=RSTEP/16, ec='black', fc='red', zorder=2)
    plt.gca().add_patch(gh)
    gg = plt.Circle((2*CSTEP+CSTEP/4, s[0]*RSTEP+2*RSTEP/8), radius=RSTEP/15.6, ec='white', fc='white', zorder=2)
    plt.gca().add_patch(gg)

def resetDots():
    global gridworld
    global activeDots
    for x in range(gridworld.cols):
        for y in range(gridworld.rows):
            dot = plt.Circle((x*CSTEP+CSTEP/4, y*RSTEP+RSTEP/8), radius=RSTEP/32, ec='black', fc='white', zorder=2)
            plt.gca().add_patch(dot)
    activeDots = deepcopy(dots)


plt = gridworld.DrawGrid( gridworld )
plt.ion()
plt.gcf().canvas.draw()
plt.gcf().canvas.flush_events()

time.sleep(2)

plt.gcf().canvas.mpl_connect('key_press_event', onclick)


Qs = dict([(s, [0 for a in gridworld.actions(s)] ) for s in gridworld.states  ])
#gridworld.DrawQs( gridworld, Qs )

print(Qs[(0,0)][1])

ROWS = gridworld.rows
COLS = gridworld.cols
RSTEP = 1/ROWS
CSTEP = 1/COLS

P1 = {(0,0):1, (0,1):1, (0,2):0, (1,1):0, (1,2):0, (2,2):0, (3,2):0, (1,0):2, (2,0):1, (2,1):1, (3,0):2, (3,1):0}
P2 = {(0,0):1, (0,1):1, (0,2):0, (1,1):0, (1,2):0, (2,2):3, (3,2):0, (1,0):2, (2,0):1, (2,1):0, (3,0):2, (3,1):0}

#feature weights
gw = 0.8
dw = 0.3

alpha = 0.5
i = 0
while i in range(25):
    s = (0,0)
    term = False
    resetDots()
    DrawPacman(s)

    while not term:
        time.sleep(0.2)
        plt.show()
        if waiting == True: #False:
            waiting = True

            if random.random() <= 0.2:
                if random.random() <= 0.8:
                    if random.random() <= 0.8:
                        a = P1[s]
                    elif random.random() <= 0.5:
                        a = (P1[s]-1)  % len(orientations)
                    else:
                        a = (P1[s]+1)  % len(orientations)
                else:
                    a = random.randint(0,3)
            else:
                if random.random() <= 0.8:
                    a = P2[s]
                else:
                    a = random.randint(0,3)


            # get next state by doing action
            sp = gridworld.go(s, orientations[a])
            activeDots[sp[1]][sp[0]] = False

            if s == (2, ghostPos[ghostFlag%4][1]):
                for a in range(4):
                    Qs[s][a] = alpha*Qs[s][a] + alpha*(-1)
                #gridworld.DrawQ( gridworld, s, a, Qs[s][a] )
                term = True
            # elif s == gridworld.terminals[1][0]:
            #     for a in range(4):
            #         Qs[s][a] = alpha*Qs[s][a] + alpha*(-1)
            #     gridworld.DrawQ( gridworld, s, a, Qs[s][a] )
            #     term = True
            else:
                # do QL update
                #Qs[s][a] = (1-alpha)*Qs[s][a]  + alpha*(gridworld.R(sp) + max(Qs[sp][a] + k/gridworld.visits[sp[0]][sp[1]] for a in range(4) ))
                Qs[s][a] = gw * gst(s, orientations[a], ghostPos[ghostFlag%4][1]) + dw * dot(s, orientations[a])
                #gridworld.DrawQ( gridworld, s, a, Qs[s][a] )
# ghost shit
            ghostFlag += 1
            DrawGhost(ghostPos[ghostFlag%4])

            ErasePacman(s)
            DrawPacman(sp)

            # update state
            s = sp

            if not any(True in l for l in activeDots):
                term = True

        plt.title('Episode: %d' % (i))
        plt.gcf().canvas.draw()
        plt.gcf().canvas.flush_events()
    ErasePacman(s)
    i = i + 1
