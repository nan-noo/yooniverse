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
import random, time, util, math
from game import Directions
import game
from util import nearestPoint

#################
# Team creation #
#################

def createTeam(firstIndex, secondIndex, isRed,
               first = 'DefensiveAgent', second = 'AlphaBetaAgent'):
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
  A Dummy agent to serve as an example of the necessary agent structure.
  You should look at baselineTeam.py for more details about how to
  create an agent as this is the bare minimum.
  """

  def registerInitialState(self, gameState):
    self.init = True
    self.mode = 'Offense'
    self.numEatenFood = 0
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

  def isEnemyRegion(self, gameState): # 적 진영에 있는지 아닌지
    pos = gameState.getAgentPosition(self.index)
    if gameState.isOnRedTeam(self.index) and not gameState.isRed(pos): # 레드팀인데 블루팀 진영
      return True
    elif not gameState.isOnRedTeam(self.index) and gameState.isRed(pos): # 블루팀인데 레드팀 진영
      return True
    else:
      return False

  def isGhost(self, gameState, index): # 해당 index가 ghost인지 아닌지
    ghostList = []
    if gameState.isOnRedTeam(self.index): 
      enemyList = gameState.getBlueTeamIndices()
      for enemy in enemyList:
        if not gameState.isRed(gameState.getAgentPosition(enemy)): # 적이 블루팀인데 블루 진영
          ghostList.append(enemy)
    else:
      enemyList = gameState.getRedTeamIndices()
      for enemy in enemyList:
        if gameState.isRed((gameState.getAgentPosition(enemy))): # 적이 레드팀인데 레드진영
          ghostList.append(enemy)

    return index in ghostList

class OffensiveAgent(DummyAgent):
  # def chooseAction(self, gameState): # 모드에 따라 agent 변경
  #   print(self.mode)
  #   ghostList = self.getOpponents(gameState)
  #   trueGhostList = []
  #   for ghost in ghostList:
  #     if self.isGhost(gameState, ghost) and gameState.getAgentState(ghost).scaredTimer == 0:
  #       trueGhostList.append(ghost)

  #   if not trueGhostList: minGhostDistance = 10
  #   else:
  #     minGhostDistance = min(
  #     [self.getMazeDistance(gameState.getAgentPosition(self.index), gameState.getAgentPosition(ghost)) for ghost in
  #      trueGhostList])

  #   enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
  #   invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]

    # 모드바꿔보기: recursion error 뜸
    # if self.mode == "Offense":
    #   if invaders:
    #     self.mode = "Defense"
    #     self.init = True
    #     return self.chooseAction(gameState)
    #   if self.isEnemyRegion(gameState):
    #     self.mode = "AlphaBetaAgent"
    #     self.init = True
    #     return self.chooseAction(gameState)
    #   if self.init == True:
    #     self.Agent = DummyAgent(self.index)
    #     self.Agent.registerInitialState(gameState)
    #     self.init = False
    #   action = self.Agent.chooseAction(gameState)

    #   if action == Directions.STOP:
    #     self.mode = "AlphaBetaAgent"
    #     self.init = True
    #     return self.chooseAction(gameState)
    #   return action

    # if self.mode == "AlphaBetaAgent":
    #   if minGhostDistance <= 5 or self.numEatenFood > 2:
    #     self.mode = "Offense"
    #     self.init = True
    #     return self.chooseAction(gameState)
    #   if self.init == True:
    #     self.Agent = AlphaBetaAgent(self.index)
    #     self.Agent.registerInitialState(gameState)
    #     self.init = False
    #   action = self.Agent.getAction(gameState)
    #   return action

    # if self.mode == "Defense":
    #   if not invaders:
    #     self.mode = "Offense"
    #     self.init = True
    #     return self.chooseAction(gameState)
    #   if self.init == True:
    #     self.Agent = DefensiveAgent(self.index)
    #     self.Agent.registerInitialState(gameState)
    #     self.init = False
    #   action = self.Agent.chooseAction(gameState)
    #   return action

  def getFeatures(self, gameState, action):
    features = util.Counter()
    successor = self.getSuccessor(gameState, action)
    foodList = self.getFood(successor).asList()    
    features['successorScore'] = -len(foodList) #self.getScore(successor)
    myPos = successor.getAgentState(self.index).getPosition()
    
    enemies = [successor.getAgentState(i) for i in self.getOpponents(successor)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
    enemyGhosts = [g for g in enemies if g not in invaders]
    # Compute distance to the nearest food

    if len(foodList) > 0 :
      myPos = successor.getAgentState(self.index).getPosition()
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
      features['distanceToFood'] = minDistance
    
    if len(invaders) > 0: # enemy pacman exist
      features['invaders'] = min([self.getMazeDistance(myPos, i.getPosition()) for i in invaders])

    if len(enemyGhosts) > 0: # enemy ghost exist
      for ghost in enemyGhosts:
        if ghost.scaredTimer == 0: # enemy ghost is not scared
          features['enemyGhost'] = min([self.getMazeDistance(myPos, ghost.getPosition()) for ghost in enemyGhosts])
          break
  
    if features['enemyGhost'] > 2: # 3칸 이상 떨어진 유령은 고려 안함
      features['enemyGhost'] = 0

    # power pellet
    capsuleList = self.getCapsules(gameState)
    features['numCapsulesLeft'] = len(capsuleList)
    if features['numCapsulesLeft'] > 0:
      features['distanceToCapsule'] = min([self.getMazeDistance(myPos, capsule) for capsule in capsuleList])
    
    if self.numEatenFood > 1:
      features['distToStart'] = self.getMazeDistance(myPos, self.start)
    else: features['distToStart'] = 0
    
    if action == Directions.STOP:
      features['stop'] = 1

    return features

  def getWeights(self, gameState, action):
    return {'successorScore': 100, 'distanceToFood': -1, 'numCapsulesLeft': -100, 
    'distanceToCapsule': -120, 'invaders': -10, 'enemyGhost': -100, 'distToStart': -10, 'stop': -100}

class DefensiveAgent(DummyAgent):
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


class ExpectiMaxAgent(CaptureAgent):
  """
    using MiniMax + ExpectiMax
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
        if not enemy.isPacman:
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

MAXDEPTH = 3
class AlphaBetaAgent(DummyAgent):
  """
  Your minimax agent with alpha-beta pruning (question 3)
  """
  def minimaxTree_node(self, gameState, k, maxDepth, parrentNode, alpha, beta): # minimax value를 계산해서 그 score를 return
    n = gameState.getNumAgents()
    depth = k // n + 1

    if not depth == 1 and gameState.getAgentPosition(self.index) == gameState.getInitialAgentPosition(self.index) \
            or gameState.isOver() \
            or depth > maxDepth and k % n == self.index:
      return self.evaluate_invade(gameState) # 값 계산

    agentIndex = k % n
    actionList = gameState.getLegalActions(agentIndex)

    if agentIndex == self.index: # pacman
      maxscore = -math.inf
      for action in actionList:
        nextState = gameState.generateSuccessor(agentIndex, action)
        thisActionTreeNode = [[action], [], ['pacman']]
        score = self.minimaxTree_node(nextState, k + 1, maxDepth, thisActionTreeNode, alpha, beta)
        maxscore = max(score, maxscore)
        thisActionTreeNode[0].append(score) # [[action, score], []]
        parrentNode[1].append(thisActionTreeNode)
      return maxscore
    elif self.isGhost(gameState, agentIndex): # ghost
      pacman_Position = gameState.getAgentPosition(self.index)
      ghostPositions = []
      ghostActions = []
      for action in actionList:
        nextState = gameState.generateSuccessor(agentIndex, action)
        ghostPositions.append(nextState.getAgentPosition(agentIndex))  # [Position, Action]
        ghostActions.append(action)
 
      distList = [manhattanDistance(pacman_Position, ghost) for ghost in ghostPositions]

      ## Pacman die

      minDistance = min(distList)
      greedyIndex = distList.index(minDistance)
      greedyAction = ghostActions[greedyIndex]
      nextState = gameState.generateSuccessor(agentIndex, ghostActions[greedyIndex])
      thisActionTreeNode = [[greedyAction], [], ['ghost']]
      score = self.minimaxTree_node(nextState, k + 1, maxDepth, thisActionTreeNode, alpha, beta)

      thisActionTreeNode[0].append(score)  # [[action, score], []]
      parrentNode[1].append(thisActionTreeNode)
      return score
    else:
      return self.minimaxTree_node(gameState, k + 1, maxDepth, parrentNode, alpha, beta)

  def findPacmanPath(self, gameState, treeNode, maxDepth, k, actions):
    n = gameState.getNumAgents()
    goDeep = k // n
    if goDeep > maxDepth:
      return
    if not treeNode[1]: return
    if treeNode[2][0]=='pacman':
      maxScore = - math.inf
      for i in range(len(treeNode[1])):
        if treeNode[1][i][0][1] > maxScore:
          # [1]: child node list, [i]: ith child node, [0]: child node action and score, [1]: child node score
          maxScore = treeNode[1][i][0][1]  
          action = treeNode[1][i][0][0]
          index = i
      actions.append(action)
    elif treeNode[2][0]=='ghost': 
      index = 0

    self.findPacmanPath(gameState, treeNode[1][index], maxDepth, k + 1, actions)

  def getAction(self, gameState):
    """
    Returns the minimax action using self.depth and self.evaluationFunction
    """
    "*** YOUR CODE HERE ***"
    myPos = gameState.getAgentPosition(self.index)
    ghostList = self.getOpponents(gameState)
    for ghost in ghostList:
      if self.isGhost(gameState, ghost) == False:
        ghostList.remove(ghost)

    ghostDistList = [self.getMazeDistance(myPos, gameState.getAgentPosition(ghost)) for ghost in ghostList]
    minGhostDistance = min(ghostDistList)
    minDistGhostIndex = ghostDistList.index(minGhostDistance)
    minDistGhost = ghostList[minDistGhostIndex] # 가장 가까운 유령 거리
    if minGhostDistance > 5:
      maxDepth = 1
    else:
      if gameState.getAgentState(minDistGhost).scaredTimer > 5:
        maxDepth = 1
      else:
        maxDepth = MAXDEPTH

    tree = [["first"], [], ['pacman']]
    finalscore = self.minimaxTree_node(gameState, self.index, maxDepth, tree, -math.inf, math.inf)

    actions = []
    self.findPacmanPath(gameState, tree, maxDepth, 0, actions)

    return actions[0]

  def getWeights(self, gameState):
    return {'successorScore': 500, 'distanceToFood': 10, 'rDistanceToGhost': 100, 'getEaten': 10000, 'distanceToCapsule': 20, \
            'goToEatCapsule': 5000, 'borderDist': 10000 }

  def getFeatures(self, gameState):
    features = util.Counter()
    foodList = self.getFood(gameState).asList()
    myPos = gameState.getAgentState(self.index).getPosition()
    features['successorScore'] = -len(foodList)  # self.getScore(successor)
    
    if len(foodList) > 0:  # This should always be True,  but better safe than sorry
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
      features['distanceToFood'] = -minDistance
    else:
      return features

    capsulesList = self.getCapsules(gameState)
    if len(capsulesList) > 0:
      minCapsuleDistance = min([self.getMazeDistance(myPos, capsule) for capsule in capsulesList])
      features['distanceToCapsule'] = -minCapsuleDistance
    else:
      features['distanceToCapsule'] = 0
    capsulesList = self.getCapsules(gameState)
    features['goToEatCapsule'] = - len(capsulesList)

    enemyIndex = self.getOpponents(gameState)
    newGhostStates = []
    for index in enemyIndex:
      ghost = gameState.getAgentState(index)
      if self.isGhost(gameState, index): # 적 유령인가?
        if ghost.scaredTimer == 0: # scared 상태가 아니면
          newGhostStates.append(gameState.getAgentState(index))

    if newGhostStates:
      minGhostDistance = min([self.getMazeDistance(myPos, ghost.getPosition()) for ghost in newGhostStates])
    else:
      minGhostDistance = 10
    features['rDistanceToGhost'] = - 1 / minGhostDistance

    if gameState.getAgentPosition(self.index) == self.start:
      features['getEaten'] = -1
    else:
      features['getEaten'] = 0

    # 3개 이상 들고 있을 시 or 시간이 별로 안남았을 때에는 영역으로 복귀
    if self.numEatenFood > 2 or \
      gameState.data.timeleft < 200 and gameState.getAgentState(self.index).numCarrying != 0:
        width = gameState.getWalls().width
        height = gameState.getWalls().height
        wallMatrix = gameState.getWalls()
        borderList = []
        for i in range(height):
          if not wallMatrix[width // 2][i] == True:
            borderList.append((width // 2, i))
        if abs(myPos[0]-width//2) > 2:
          minDistance = min([self.getMazeDistance(myPos, border) for border in borderList])
          features['borderDist'] = -minDistance
        else:
          features['borderDist'] = 0

    return features

  def evaluate_invade(self, gameState):
    """
    Computes a linear combination of features and feature weights
    """

    ############################
    #cache the score           #
    ############################
    features = self.getFeatures(gameState)
    weights = self.getWeights(gameState)
    return features * weights

def manhattanDistance(xy1, xy2):
  "The Manhattan distance heuristic for a PositionSearchProblem"
  return abs(xy1[0] - xy2[0]) + abs(xy1[1] - xy2[1])