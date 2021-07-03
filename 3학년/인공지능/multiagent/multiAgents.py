# multiAgents.py
# --------------
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


from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
    """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change
    it in any way you see fit, so long as you don't touch our method
    headers.
    """


    def getAction(self, gameState):
        """
        You do not need to change this method, but you're welcome to.

        getAction chooses among the best options according to the evaluation function.

        Just like in the previous project, getAction takes a GameState and returns
        some Directions.X for some X in the set {NORTH, SOUTH, WEST, EAST, STOP}
        """
        # Collect legal moves and successor states
        legalMoves = gameState.getLegalActions()

        # Choose one of the best actions
        scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
        bestScore = max(scores)
        bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
        chosenIndex = random.choice(bestIndices) # Pick randomly among the best

        "Add more of your code here if you want to"

        return legalMoves[chosenIndex]

    def evaluationFunction(self, currentGameState, action):
        """
        Design a better evaluation function here.

        The evaluation function takes in the current and proposed successor
        GameStates (pacman.py) and returns a number, where higher numbers are better.

        The code below extracts some useful information from the state, like the
        remaining food (newFood) and Pacman position after moving (newPos).
        newScaredTimes holds the number of moves that each ghost will remain
        scared because of Pacman having eaten a power pellet.

        Print out these variables to see what you're getting, then combine them
        to create a masterful evaluation function.
        """
        # Useful information you can extract from a GameState (pacman.py)
        successorGameState = currentGameState.generatePacmanSuccessor(action) # possible next state
        newPos = successorGameState.getPacmanPosition() # current pacman position (1, 1)
        newFood = successorGameState.getFood() # FTFFF
        newGhostStates = successorGameState.getGhostStates() # [<game.AgentState object at 0x000001381A626898>]
        newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates] #[0]

        "*** YOUR CODE HERE ***"
        #print(successorGameState)
        #print(newPos)
        #print(newFood.asList()) # food position list
        #print(newFood)
        #print(newGhostStates[0].configuration.getPosition()) # closest ghost position(8, 5)
        #print(newGhostStates[0].configuration.getDirection()) # closest ghost direction West
        #print(newScaredTimes)
        #print(successorGameState.getScore())
        #print(action)
        score = successorGameState.getScore()

        # ghost와 거리가 멀고, food와 거리가 가까울수록 높은 점수
        # stop action은 마이너스 점수 -> 이동하는 게 더 좋음!
        g_pos = newGhostStates[0].configuration.getPosition() #closest ghost position
        ghostDistance = manhattanDistance(newPos, g_pos) #distance between pacman and closest ghost
        foodDistance = [manhattanDistance(newPos, f_pos) for f_pos in newFood.asList()] #distance between pacman and food list
        #print(minFoodDist) # 1~3
        
        if action == 'Stop':
            score -= 10
        
        if len(foodDistance) == 0: # no food left
            return score
        
        minFoodDist = min(foodDistance)
        if min(newScaredTimes) > 0: # ghost is scared -> do not consider ghosts
            return score + 1 / minFoodDist * 5
        #print(ghostDistance / min(foodDistance)) # 1~10
        
        # score
        eval_score = score + ghostDistance / minFoodDist
        return eval_score

def scoreEvaluationFunction(currentGameState):
    """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.

    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
    """
    return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
    """
    This class provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.

    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.

    Note: this is an abstract class: one that should not be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.
    """

    def __init__(self, evalFn = 'scoreEvaluationFunction', depth = '2'):
        self.index = 0 # Pacman is always agent index 0
        self.evaluationFunction = util.lookup(evalFn, globals())
        self.depth = int(depth)

class MinimaxAgent(MultiAgentSearchAgent):
    """
    Your minimax agent (question 2)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action from the current gameState using self.depth
        and self.evaluationFunction.

        Here are some method calls that might be useful when implementing minimax.

        gameState.getLegalActions(agentIndex):
        Returns a list of legal actions for an agent
        agentIndex=0 means Pacman, ghosts are >= 1

        gameState.generateSuccessor(agentIndex, action):
        Returns the successor game state after an agent takes an action

        gameState.getNumAgents():
        Returns the total number of agents in the game

        gameState.isWin():
        Returns whether or not the game state is a winning state

        gameState.isLose():
        Returns whether or not the game state is a losing state
        """
        "*** YOUR CODE HERE ***"
        # for Pacman(index = 0)
        # return (maxscore, maxaction)
        def maxValue(state, depth, index, numAgents): 
            if depth == self.depth or state.isWin() or state.isLose(): # terminal
                return (self.evaluationFunction(state), "") 
            
            max_score = float("-inf")
            max_action = ""
            legalActions = state.getLegalActions(index)
            for action in legalActions:
                successor = state.generateSuccessor(index, action) 
                score = minValue(successor, depth, index + 1, numAgents)[0] # ghost's turn
                
                if score > max_score: # update max_score
                    max_score = score
                    max_action = action
            
            return (max_score, max_action)
        
        # for ghosts(index >= 1)
        # return (minscore, minaction)
        def minValue(state, depth, index, numAgents):
            if depth == self.depth or state.isWin() or state.isLose():
                return self.evaluationFunction(state), ""

            min_score = float("inf")
            min_action = ""
            legalActions = state.getLegalActions(index)
            for action in legalActions:
                successor = state.generateSuccessor(index, action) # ghost state
                successor_index = index + 1

                if successor_index == numAgents: # all agents done -> pacman's turn
                    score = maxValue(successor, depth + 1, 0, numAgents)[0]
                else:
                    score = minValue(successor, depth, successor_index, numAgents)[0]
                
                if score < min_score: # update min_score
                    min_score = score
                    min_action = action
            
            return (min_score, min_action)

        numAgents = gameState.getNumAgents()
        maxAction = maxValue(gameState, 0, 0, numAgents)[1] # pacman start
        return maxAction

class AlphaBetaAgent(MultiAgentSearchAgent):
    """
    Your minimax agent with alpha-beta pruning (question 3)
    """

    def getAction(self, gameState):
        """
        Returns the minimax action using self.depth and self.evaluationFunction
        """
        "*** YOUR CODE HERE ***"
        # for Pacman(index = 0)
        # return (maxscore, maxaction)
        def maxValue(state, depth, index, numAgents, a, b): 
            if depth == self.depth or state.isWin() or state.isLose(): # terminal
                return (self.evaluationFunction(state), "") 
            
            max_score = float("-inf")
            max_action = ""
            legalActions = state.getLegalActions(index)
            for action in legalActions:
                successor = state.generateSuccessor(index, action) # ghost turn
                score = minValue(successor, depth, index + 1, numAgents, a, b)[0]

                if score > max_score: # update max_score
                    max_score = score
                    max_action = action

                if max_score > b: return (max_score, max_action) # pruning
                a = max(a, max_score) # update alpha
            return (max_score, max_action)
        
        # for ghosts(index >= 1)
        # return (minscore, minaction)
        def minValue(state, depth, index, numAgents, a, b):
            if depth == self.depth or state.isWin() or state.isLose():
                return self.evaluationFunction(state), ""

            min_score = float("inf")
            min_action = ""
            legalActions = state.getLegalActions(index)
            for action in legalActions:
                successor = state.generateSuccessor(index, action) # ghost state
                successor_index = index + 1

                if successor_index == numAgents: # all agents done -> pacman's turn
                    score = maxValue(successor, depth + 1, 0, numAgents, a, b)[0]
                else:
                    score = minValue(successor, depth, successor_index, numAgents, a, b)[0]
                
                if score < min_score: #update min_score
                    min_score = score
                    min_action = action

                if min_score < a: return (min_score, min_action) # pruning
                b = min(b, min_score) # update beta

            return (min_score, min_action)
        
        numAgents = gameState.getNumAgents()
        maxAction = maxValue(gameState, 0, 0, numAgents, float("-inf"), float("inf"))[1] # pacman start
        return maxAction

class ExpectimaxAgent(MultiAgentSearchAgent):
    """
      Your expectimax agent (question 4)
    """

    def getAction(self, gameState):
        """
        Returns the expectimax action using self.depth and self.evaluationFunction

        All ghosts should be modeled as choosing uniformly at random from their
        legal moves.
        """
        "*** YOUR CODE HERE ***"
        util.raiseNotDefined()

def betterEvaluationFunction(currentGameState):
    """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).

    DESCRIPTION: <write something here so we know what you did>
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

# Abbreviation
better = betterEvaluationFunction
