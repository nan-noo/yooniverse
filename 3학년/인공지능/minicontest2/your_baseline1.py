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
               first = 'OffensiveAgent', second = 'DefensiveAgent'):
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

  # The following line is an example only; feel free to change it.
  return [eval(first)(firstIndex), eval(second)(secondIndex)]

##########
# Agents #
##########

class DummyAgent(CaptureAgent):
  """
    baseline.py의 reflex agent 기반해서 구현
  """

  def registerInitialState(self, gameState):
    self.numEatenFood = 0 # 먹은 food 수
    self.start = gameState.getAgentPosition(self.index)
    CaptureAgent.registerInitialState(self, gameState)

  def chooseAction(self, gameState):
    """
    Picks among the actions with the highest Q(s,a).
    """
    actions = gameState.getLegalActions(self.index)

    # You can profile your evaluation time by uncommenting these lines
    # start = time.time()
    values = [self.evaluate(gameState, a) for a in actions] # Q-value 계산
    # print 'eval time for agent %d: %.4f' % (self.index, time.time() - start)

    maxValue = max(values)
    bestActions = [a for a, v in zip(actions, values) if v == maxValue]

    nextAction = random.choice(bestActions)

    curfoodLeft = len(self.getFood(gameState).asList())
    self.numEatenFood = gameState.getAgentState(self.index).numCarrying

    if curfoodLeft <= 2:
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
    return nextAction

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

class OffensiveAgent(DummyAgent):
  """
    공격 역할을 하는 agent
    food, ghost의 위치에 따라 Q-value가 변함. 
  """

  def getFeatures(self, gameState, action):
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)
    foodList = self.getFood(successor).asList()    
    features['successorScore'] = -len(foodList) #self.getScore(successor)
    myPos = successor.getAgentState(self.index).getPosition()
    
    enemies = [successor.getAgentState(i) for i in self.getOpponents(successor)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None] # 적팀에서 팩맨인 agents
    enemyGhosts = [g for g in enemies if g not in invaders] # 적팀에서 유령인 agents
    # Compute distance to the nearest food

    if len(foodList) > 0 :
      myPos = successor.getAgentState(self.index).getPosition()
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
      features['distanceToFood'] = minDistance
    
    if len(invaders) > 0: # enemy pacman exist
      features['invaders'] = min([self.getMazeDistance(myPos, i.getPosition()) for i in invaders])

    features['scaredGhost'] = 0
    if len(enemyGhosts) > 0: # enemy ghost exist
      for ghost in enemyGhosts:
        if ghost.scaredTimer > 2: # scaredTime이 남아 있을 때 고려
          features['scaredGhost'] += 1
        elif ghost.scaredTimer == 0: # enemy ghost is not scared
          features['enemyGhost'] = min([self.getMazeDistance(myPos, ghost.getPosition()) for ghost in enemyGhosts])
          break
  
    if features['enemyGhost'] > 2: # 3칸 이상 떨어진 유령은 고려 안함
      features['enemyGhost'] = 0

    # power pellet
    capsuleList = self.getCapsules(gameState)
    features['numCapsulesLeft'] = len(capsuleList)
    if features['numCapsulesLeft'] > 0:
      features['distanceToCapsule'] = min([self.getMazeDistance(myPos, capsule) for capsule in capsuleList])
    
    if self.numEatenFood > 1: # 2개 이상 먹었을 경우, 돌아가도록 설정
      features['distToStart'] = self.getMazeDistance(myPos, self.start)
    else: features['distToStart'] = 0
    
    if action == Directions.STOP:
      features['stop'] = 1

    return features

  def getWeights(self, gameState, action):
    return {'successorScore': 100, 'distanceToFood': -1, 'numCapsulesLeft': -100, 
    'distanceToCapsule': -120, 'invaders': -10, 'enemyGhost': -100, 
    'scaredGhost': 100, 'distToStart': -10, 'stop': -100}

class DefensiveAgent(DummyAgent):
  """
    수비 역할을 하는 agent
    invader 위치에 따라 Q-value가 변함. 
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
    enemyGhosts = [g for g in enemies if g not in invaders]
    
    features['numInvaders'] = len(invaders)
    
    if len(invaders) > 0:
      dists = [self.getMazeDistance(myPos, a.getPosition()) for a in invaders]
      features['invaderDistance'] = min(dists)

    if len(enemyGhosts) > 0: # enemy ghost exist
      for ghost in enemyGhosts:
        if ghost.scaredTimer == 0: # enemy ghost is not scared
          features['enemyGhost'] = min([self.getMazeDistance(myPos, ghost.getPosition()) for ghost in enemyGhosts])
          break

    if action == Directions.STOP: features['stop'] = 1
    rev = Directions.REVERSE[gameState.getAgentState(self.index).configuration.direction]
    if action == rev: features['reverse'] = 1

    return features

  def getWeights(self, gameState, action):
    return {'numInvaders': -100, 'onDefense': 50, 'invaderDistance': -10, 'stop': -100, 'reverse': -2, 'enemyGhost': -10}
