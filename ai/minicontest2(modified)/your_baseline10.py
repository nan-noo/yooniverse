# baselineTeam.py
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


# baselineTeam.py
# ---------------
# Licensing Information: Please do not distribute or publish solutions to this
# project. You are free to use and extend these projects for educational
# purposes. The Pacman AI projects were developed at UC Berkeley, primarily by
# John DeNero (denero@cs.berkeley.edu) and Dan Klein (klein@cs.berkeley.edu).
# For more info, see http://inst.eecs.berkeley.edu/~cs188/sp09/pacman.html


from captureAgents import CaptureAgent
import distanceCalculator
import random, time, util, sys
from game import Directions
import game
from util import nearestPoint
from distanceCalculator import Distancer
#################
# Team creation #
#################

def createTeam(firstIndex, secondIndex, isRed,
               first = 'OffensiveReflexAgent', second = 'DefensiveReflexAgent'):
  """
  This function should return a list of two agents that will form the
  team, initialized using firstIndex and secondIndex as their agent
  index numbers.  isRed is True if the red team is being created, and
  will be False if the blue team is being created.
  As a potentially helpful development aid, this function can take
  additional string-valued keyword arguments ("first" and "second" are
  such arguments in the case of this function), which will come from
  the --redOpts and --blueOpts command-line arguments to capture.py.
  For the nightly contest, however, your team will be created without
  any extra arguments, so you should make sure that the default
  behavior is what you want for the nightly contest.
  """
  return [eval(first)(firstIndex), eval(second)(secondIndex)]

##########
# Agents #
##########

class ReflexCaptureAgent(CaptureAgent):
  """
  A base class for reflex agents that chooses score-maximizing actions
  """

  def registerInitialState(self, gameState):
    self.start = gameState.getAgentPosition(self.index)
    CaptureAgent.registerInitialState(self, gameState)

  def chooseAction(self, gameState):
    """
    Picks among the actions with the highest Q(s,a).
    """
    actions = gameState.getLegalActions(self.index)

    # You can profile your evaluation time by uncommenting these lines
    # start = time.time()
    values = [self.evaluate(gameState, a) for a in actions]
    # print 'eval time for agent %d: %.4f' % (self.index, time.time() - start)

    maxValue = max(values)
    bestActions = [a for a, v in zip(actions, values) if v == maxValue]

    foodLeft = len(self.getFood(gameState).asList())

    if foodLeft <= 2:
      bestDist = 9999
      for action in actions:
        successor = self.getSuccessor(gameState, action)
        pos2 = successor.getAgentPosition(self.index)
        dist = self.getMazeDistance(self.start,pos2)
        if dist < bestDist:
          bestAction = action
          bestDist = dist
      return bestAction

    return random.choice(bestActions)

  def getSuccessor(self, gameState, action):
    """
    Finds the next successor which is a grid position (location tuple).
    """
    successor = gameState.generateSuccessor(self.index, action)
    pos = successor.getAgentState(self.index).getPosition()
    if pos != nearestPoint(pos):
      # Only half a grid position was covered
      return successor.generateSuccessor(self.index, action)
    else:
      return successor

  def evaluate(self, gameState, action):
    """
    Computes a linear combination of features and feature weights
    """
    features = self.getFeatures(gameState, action)
    weights = self.getWeights(gameState, action)
    return features * weights

  def getFeatures(self, gameState, action):
    """
    Returns a counter of features for the state
    """
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)
    features['successorScore'] = self.getScore(successor)
    return features

  def getWeights(self, gameState, action):
    """
    Normally, weights do not depend on the gamestate.  They can be either
    a counter or a dictionary.
    """
    return {'successorScore': 1.0}

class OffensiveReflexAgent(ReflexCaptureAgent):
  """
  A reflex agent that seeks food. This is an agent
  we give you to get an idea of what an offensive agent might look like,
  but it is by no means the best or only way to build an offensive agent.
  """
  def getFeatures(self, gameState, action):
    distancer = Distancer(gameState.data.layout)
    distancer.getMazeDistances()
    #if have more food, make it more urgent to get back
    #move away from the ghosts
    #if on the defense side from getting set back, sort of act like a defender but don't make sacrifices
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)
    foodList = self.getFood(successor).asList()
    features['foodLeft'] = len(foodList)#self.getScore(successor)

    #Run away from ghosts when on other side
    enemyIndices = self.getOpponents(successor)
    enemyPositions = [successor.getAgentPosition(index) for index in enemyIndices if not successor.getAgentState(index).isPacman] #only adds ghosts to the tally
    enemyDistances = []
    for pos in enemyPositions:
        enemyDistances.append(distancer.getDistance(successor.getAgentPosition(self.index), pos))
    ghostClose = 10
    if len(enemyDistances) > 0 and min(enemyDistances) < 3 and successor.getAgentState(self.index).isPacman: #only cares about ghosts if its a pacman and the distance is less than three
        ghostClose = min(enemyDistances)
    features['ghostClose'] = ghostClose


    numEat = gameState.getAgentState(self.index).numCarrying
    #now, find out if the next move is moving closer to home
    nextMoveCloser = 0
    currentX, currentY = gameState.getAgentPosition(self.index)
    succX, succY = successor.getAgentPosition(self.index)
    #if the next move is closer and numEat > 5, then homeUrgent is 1 and weight it
    ######SAM STUFF
    targetX = gameState.data.layout.width // 2
    if gameState.isOnRedTeam(self.index):
        targetX = targetX - 1


    targetPositions = [(targetX, y) for y in range(0, gameState.data.layout.height)]

    distancer = Distancer(gameState.data.layout)
    distancer.getMazeDistances()

    targetDistances = []
    for position in targetPositions:
        try:
            targetDistances.append(distancer.getDistance(position, successor.getAgentPosition(self.index)))
        except:
            doNothing = 0
    ##############
    minDist = min(targetDistances)
    features['homeUrgent'] = 30
    if numEat > 0:
        features['homeUrgent'] = minDist


    enemies = [successor.getAgentState(i) for i in self.getOpponents(successor)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
    features['numInvaders'] = len(invaders)

    targetX = gameState.data.layout.width // 2
    if not gameState.isOnRedTeam(self.index):
        targetX = targetX - 1

    targetPositions = [(targetX, y) for y in range(0, gameState.data.layout.height)]

    distancer = Distancer(gameState.data.layout)
    distancer.getMazeDistances()

    targetsFarEnoughAway = []
    for position in targetPositions:
        try:
            minDistance = min([distancer.getDistance(position, enemy.getPosition()) for enemy in enemies])
            if minDistance > 7: #changed from 7 to 10
                targetsFarEnoughAway.append(position)
        except:
            doNothing = 0

    myPos = successor.getAgentState(self.index).getPosition()
    distanceToClosestGhost = min([distancer.getDistance(myPos, enemy.getPosition()) for enemy in enemies])
    distanceToTPs = [distancer.getDistance(myPos, position) for position in targetsFarEnoughAway]

    if(gameState.getAgentState(self.index).isPacman):
        features['distanceToTargetCrossings'] = 0
        features['ghostClose'] = 0 if distanceToClosestGhost > 4 else distanceToClosestGhost
    else:
        features['ghostClose'] = 0
        features['distanceToTargetCrossings'] = min(distanceToTPs) if len(distanceToTPs) > 0 else 0

    #numeats is bigger than 5 and the next move is closer to home, then set homeUrgent to positive
    #if numeats > 5 and next move is farther from home, set homeUrgent to negative
    #otherwise set homeUrgent to 0

    # Compute distance to the nearest food

    if len(foodList) > 0: # This should always be True,  but better safe than sorry
      myPos = successor.getAgentState(self.index).getPosition()
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
#      if not successor.getAgentState(self.index).isPacman: #this basically means we only look for food if we're on the other side
#        minDistance = 100
      features['distanceToFood'] = minDistance
    return features

  def getWeights(self, gameState, action):
    return {'foodLeft': -100, 'distanceToFood': -1, 'ghostClose': 100, 'homeUrgent': -100, 'distanceToTargetCrossings': -100, "numInvaders": -20}

class DefensiveReflexAgent(ReflexCaptureAgent):
  """
  A reflex agent that keeps its side Pacman-free. Again,
  this is to give you an idea of what a defensive agent
  could be like.  It is not the best or only way to make
  such an agent.
  """
  coopMode = False

  def getFeatures(self, gameState, action):
    if self.coopMode:
        return self.getCoopModeFeatures(gameState, action, defIndex)
    else:
        return self.getSoloModeFeatures(gameState, action)


  def getSoloModeFeatures(self, gameState, action):
    distancer = Distancer(gameState.data.layout)
    distancer.getMazeDistances()
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)

    myState = successor.getAgentState(self.index)
    myPos = myState.getPosition()

    # Computes whether we're on defense (1) or offense (0)
    features['onDefense'] = 1
    if myState.isPacman: features['onDefense'] = 0

    # Computes distance to invaders we can see
    enemies = [successor.getAgentState(i) for i in self.getOpponents(successor)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
    features['numInvaders'] = len(invaders)

    if len(invaders) > 0:
        distance = [self.getMazeDistance(myPos, a.getPosition()) for a in invaders]
        features['invaderDistance'] = min(distance)
    
    if action == Directions.STOP: features['stop'] = 1
    rev = Directions.REVERSE[gameState.getAgentState(self.index).configuration.direction]
    if action == rev: features['reverse'] = 1

    return features

  def getCoopModeFeatures(self, gameState, action):
    defIndex = 1
    distancer = Distancer(gameState.data.layout)
    distancer.getMazeDistances()
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)

    myState = successor.getAgentState(self.index)
    myPos = myState.getPosition()

    # Computes whether we're on defense (1) or offense (0)
    features['onDefense'] = 1
    if myState.isPacman: features['onDefense'] = 0

    # Computes distance to invaders we can see
    enemies = [successor.getAgentState(i) for i in self.getOpponents(successor)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
    features['numInvaders'] = len(invaders)
    distance = enemies[defIndex]

    return features

  def getWeights(self, gameState, action):
    if self.coopMode:
        return self.getCoopWeights(gameState, action)
    else:
        return self.getSoloWeights(gameState, action)

  def getCoopWeights(self, gameState, action):
      return {'numInvaders': -1000, 'onDefense': 100, 'invaderDistance': -10 }

  def getSoloWeights(self, gameState, action):
      return {'numInvaders': -1000, 'onDefense': 100, 'invaderDistance': -10, 'stop': -100, 'reverse': -2, 'waitmiddle': -100}