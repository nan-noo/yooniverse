# myAgents.py
# ---------------
# Licensing Information:  You are free to use or extend these projects for
# educational purposes provided that (1) you do not distribute or publish
# solutions, (2) you retain this notice, and (3) you provide clear
# attribution to UC Berkeley, including a link to http://ai.berkeley.edu.
#
# Attribution Information: The Pacman AI projects were developed at UC Berkeley.
# The core projects and autograders were primarily created by John DeNero
# (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# Student side autograding was added by Brad Miller, Nick Hay, and
# Pieter Abbeel (pabbeel@cs.berkeley.edu).

from game import Agent
from searchProblems import PositionSearchProblem

import util
import time
import search

"""
IMPORTANT
`agent` defines which agent you will use. By default, it is set to ClosestDotAgent,
but when you're ready to test your own agent, replace it with MyAgent
"""
def createAgents(num_pacmen, agent='ClosestDotAgent'): 
    # global numPacman 
    # numPacman = num_pacmen
    # list = [ClosestDotAgent(index=0), ClosestDotAgent(index=1)]
    # list = list + [eval(agent)(index=i) for i in range(2, num_pacmen)]
    return [eval(agent)(index=i) for i in range(num_pacmen)]
    #return list

class MyAgent(Agent):
    """
    Implementation of your agent.
    """

    def getAction(self, state):
        """
        Returns the next action the agent will take
        """

        "*** YOUR CODE HERE ***"
        # scoring: +10(eat pellet)/ +500(eat all pellets)/ 0.4(action taken)/ -time*1000(computing cost)
        # 처음 path를 계산하고 action을 취해도, 그 path가 끝날 때까지 다시 계산하지 않음(computing cost때문)
        # 다른 agent가 target으로 삼지 않은 food를 goalstate로 선택

        if len(self.path) == 0: # start or no actions left
            problem = AnyFoodSearchProblem(state, self.index)
            self.path, self.target = self.myCDA(problem) # test71에서 BFS보다 점수가 10점가량 더 높음
            #self.path, self.target = self.myBFS(problem) # 기본실행은 둘 다 비슷, 평균적으로 CDA가 더 나음
            action = self.path[0]
            del self.path[0]
            return action
        else: # path가 남아 있으면 다시 계산하지 않고 진행
            # food = state.getFood().asList() # [(1, 1), ...]
            # if self.target not in food: # path는 남아있는데, target food를 다른 agent가 먹음
            #     problem = AnyFoodSearchProblem(state, self.index)
            #     self.path, self.target = self.myCDA(problem) # 경로 재계산 -> 계산 cost때문에 더 안좋음
            action = self.path[0]
            del self.path[0]
            return action

        #raise NotImplementedError()

    def initialize(self):
        """
        Intialize anything you want to here. This function is called
        when the agent is first created. If you don't need to use it, then
        leave it blank
        """

        "*** YOUR CODE HERE"
        self.target = (-1,-1)
        self.path = [] # initializing each agent's path

        if self.index == 0: # 처음 한번만 초기화
            global targetList
            targetList = []
        #raise NotImplementedError()
    
    def myBFS(self, problem):
        """Search the shallowest nodes in the search tree first."""
        fringe = util.Queue()
        current = (problem.getStartState(), [])
        fringe.push(current)
        closed = []
        
        while not fringe.isEmpty():
            node, path = fringe.pop()
            if problem.isGoalState(node):
                return path, node # goal state에 해당하는 node도 return
            if not node in closed:
                closed.append(node)
                for coord, move, cost in problem.getSuccessors(node):
                    fringe.push((coord, path + [move]))
        return path, node

    def myCDA(self, problem):
        """
        Returns a path (a list of actions) to the closest dot, starting from
        gameState.
        """
        pacmanCurrent = [problem.getStartState(), [], 0]
        visitedPosition = set()
        # visitedPosition.add(problem.getStartState())
        fringe = util.PriorityQueue()
        fringe.push(pacmanCurrent, pacmanCurrent[2])
        while not fringe.isEmpty():
            pacmanCurrent = fringe.pop()
            if pacmanCurrent[0] in visitedPosition:
                continue
            else:
                visitedPosition.add(pacmanCurrent[0])
            if problem.isGoalState(pacmanCurrent[0]):
                return pacmanCurrent[1], pacmanCurrent[0]
            else:
                pacmanSuccessors = problem.getSuccessors(pacmanCurrent[0])
            Successor = []
            for item in pacmanSuccessors:  # item: [(x,y), 'direction', cost]
                if item[0] not in visitedPosition:
                    pacmanRoute = pacmanCurrent[1].copy()
                    pacmanRoute.append(item[1])
                    sumCost = pacmanCurrent[2]
                    Successor.append([item[0], pacmanRoute, sumCost + item[2]])
            for item in Successor:
                fringe.push(item, item[2])
        return pacmanCurrent[1], pacmanCurrent[0]
"""
Put any other SearchProblems or search methods below. You may also import classes/methods in
search.py and searchProblems.py. (ClosestDotAgent as an example below)
"""

class ClosestDotAgent(Agent):

    def findPathToClosestDot(self, gameState):
        """
        Returns a path (a list of actions) to the closest dot, starting from
        gameState.
        """
        # Here are some useful elements of the startState
        startPosition = gameState.getPacmanPosition(self.index)
        food = gameState.getFood()
        walls = gameState.getWalls()
        problem = AnyFoodSearchProblem(gameState, self.index)


        "*** YOUR CODE HERE ***"

        pacmanCurrent = [problem.getStartState(), [], 0]
        visitedPosition = set()
        # visitedPosition.add(problem.getStartState())
        fringe = util.PriorityQueue()
        fringe.push(pacmanCurrent, pacmanCurrent[2])
        while not fringe.isEmpty():
            pacmanCurrent = fringe.pop()
            if pacmanCurrent[0] in visitedPosition:
                continue
            else:
                visitedPosition.add(pacmanCurrent[0])
            if problem.isGoalState(pacmanCurrent[0]):
                return pacmanCurrent[1]
            else:
                pacmanSuccessors = problem.getSuccessors(pacmanCurrent[0])
            Successor = []
            for item in pacmanSuccessors:  # item: [(x,y), 'direction', cost]
                if item[0] not in visitedPosition:
                    pacmanRoute = pacmanCurrent[1].copy()
                    pacmanRoute.append(item[1])
                    sumCost = pacmanCurrent[2]
                    Successor.append([item[0], pacmanRoute, sumCost + item[2]])
            for item in Successor:
                fringe.push(item, item[2])
        return pacmanCurrent[1]

    def getAction(self, state):
        return self.findPathToClosestDot(state)[0]

class AnyFoodSearchProblem(PositionSearchProblem):
    """
    A search problem for finding a path to any food.

    This search problem is just like the PositionSearchProblem, but has a
    different goal test, which you need to fill in below.  The state space and
    successor function do not need to be changed.

    The class definition above, AnyFoodSearchProblem(PositionSearchProblem),
    inherits the methods of the PositionSearchProblem.

    You can use this search problem to help you fill in the findPathToClosestDot
    method.
    """

    def __init__(self, gameState, agentIndex):
        "Stores information from the gameState.  You don't need to change this."
        # Store the food for later reference
        self.food = gameState.getFood()

        # Store info for the PositionSearchProblem (no need to change this)
        self.walls = gameState.getWalls()
        self.startState = gameState.getPacmanPosition(agentIndex)
        self.costFn = lambda x: 1
        self._visited, self._visitedlist, self._expanded = {}, [], 0 # DO NOT CHANGE

    def isGoalState(self, state):
        """
        The state is Pacman's position. Fill this in with a goal test that will
        complete the problem definition.
        """
        x,y = state
        # 다른 agent가 target으로 삼지 않았을 때도 조건으로 추가
        if state not in targetList and self.food[x][y] == True: 
            targetList.append(state)
            return True
        return False

