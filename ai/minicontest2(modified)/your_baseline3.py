# myTeam.py
# ---------
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


from captureAgents import CaptureAgent
import random, time, util
from game import Directions
import game
from util import nearestPoint

#################
# Team creation #
#################


def createTeam(firstIndex, secondIndex, isRed,
               first='DefensiveReflexAgent', second='ExpectiMaxAgent'):
  return [eval(first)(firstIndex), eval(second)(secondIndex)]

##########
# Agents #
##########

class DummyAgent(CaptureAgent):
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

    #print(gameState.getAgentState(self.index))
    return random.choice(bestActions)

  def getSuccessor(self, gameState, action):
    """
    Finds the next successor which is a grid position (location tuple).
    """
    successor = gameState.generateSuccessor(self.index, action)
    pos = successor.getAgentState(self.index).getPosition()
    if pos != nearestPoint(pos):
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

class DefensiveReflexAgent(DummyAgent):
  """
  A reflex agent that keeps its side Pacman-free. Again,
  this is to give you an idea of what a defensive agent
  could be like.  It is not the best or only way to make
  such an agent.
  """

  def getFeatures(self, gameState, action):
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
      dists = [self.getMazeDistance(myPos, a.getPosition()) for a in invaders]
      features['invaderDistance'] = min(dists)

    if action == Directions.STOP: features['stop'] = 1
    rev = Directions.REVERSE[gameState.getAgentState(self.index).configuration.direction]
    if action == rev: features['reverse'] = 1

    return features

  def getWeights(self, gameState, action):
    return {'numInvaders': -1000, 'onDefense': 100, 'invaderDistance': -10, 'stop': -100, 'reverse': -2}


class ExpectiMaxAgent(CaptureAgent):
  """
    using ExpectiMax
  """

  def registerInitialState(self, gameState):
    self.numEatenFood = 0
    self.agents = [self.index] + self.getOpponents(gameState)
    self.depth = 2
    self.start = gameState.getAgentPosition(self.index)
    CaptureAgent.registerInitialState(self, gameState)

  def chooseAction(self, gameState):
    actions = gameState.getLegalActions(self.index)

    self.numEatenFood = gameState.getAgentState(self.index).numCarrying

    foodLeft = len(self.getFood(gameState).asList())

    if foodLeft <= 2:
      bestDist = 9999
      for action in actions:
        successor = self.getSuccessor(gameState, action)
        pos2 = successor.getAgentPosition(self.index)
        dist = self.getMazeDistance(self.start, pos2)
        if dist < bestDist:
          bestAction = action
          bestDist = dist
      return bestAction

    successors = [gameState.generateSuccessor(self.index, action) for action in actions]
    values = [self.getValue(successor, self.depth, 1) for successor in successors]

    return actions[values.index(max(values))]

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

  def getValue(self, successor, depth, index):
    if self.agents[index] == self.index:
      depth -= 1

    if depth == 0 or successor.isOver():
      return self.evaluate(successor)

    if self.agents[index] == self.index:
      return self.maxValue(successor, depth, index)
    else:
      return self.expectiValue(successor, depth, index)

  def maxValue(self, successor, depth, index):
    actions = successor.getLegalActions(self.agents[index])
    nextSuccessors = [successor.generateSuccessor(self.agents[index], action) for action in actions]
    nextIndex = (index + 1) % len(self.agents)
    values = [self.getValue(nextSuccessor, depth, nextIndex) for nextSuccessor in nextSuccessors]

    return max(values)

  def expectiValue(self, successor, depth, index):
    actions = successor.getLegalActions(self.agents[index])
    nextSuccessors = [successor.generateSuccessor(self.agents[index], action) for action in actions]
    nextIndex = (index + 1) % len(self.agents)
    values = [self.getValue(nextSuccessor, depth, nextIndex) for nextSuccessor in nextSuccessors]

    normalize = [1 / len(actions) for action in actions]
    return sum(n * v for n, v in zip(normalize, values))

  def evaluate(self, gameState):
    """
    Computes a linear combination of features and feature weights
    """
    features = self.getFeatures(gameState)
    weights = self.getWeights(gameState)
    return features * weights

  def getFeatures(self, gameState):
    features = util.Counter()
    myState = gameState.getAgentState(self.index)
    myPos = myState.getPosition()

    foodList = self.getFood(gameState).asList()
    features['successorScore'] = -len(foodList)

    if len(foodList) > 0:
      features['distanceToFood'] = min([self.getMazeDistance(myPos, food) for food in foodList])

    # power pellet
    capsuleList = self.getCapsules(gameState)
    features['numCapsulesLeft'] = len(capsuleList)
    if features['numCapsulesLeft'] > 0:
      minDistance = min([self.getMazeDistance(myPos, food) for food in capsuleList])
      features['distanceToCapsule'] = minDistance

    features['enemyGhost'] = 0
    features['numScaredGhosts'] = 0
    features['enemyPacman'] = 0
    features['numEatPacman'] = 0

    enemyList = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
    for enemy in enemyList:
        if not enemy.isPacman: # 적팀 유령
            if enemy.scaredTimer > 0:
                features['enemyGhost'] += self.getMazeDistance(myPos, enemy.getPosition())
                features['numScaredGhosts'] += 1
            else:
                features['enemyGhost'] -= self.getMazeDistance(myPos, enemy.getPosition())
        else:
            features['invadors'] += self.getMazeDistance(myPos, enemy.getPosition())
            features['numEatPacman'] += 1
            if myState.scaredTimer > 0:
                features['enemyPacman'] = -features['enemyPacman']

    return features

  def getWeights(self, gameState):
    return {'successorScore': 10000, 'distanceToFood': -100,
            'distanceToCapsule': -120, 'numCapsulesLeft': -15000,
            'numScaredGhosts': -70, 'enemyGhost': -100,
            'numEatPacman': -190, 'invadors': -100}