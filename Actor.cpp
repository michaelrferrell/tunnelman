#include "Actor.h"
#include "StudentWorld.h"
#include <algorithm>
using namespace std;

// Actor Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
// getRadius function
double Actor::getRadius(double x1, double y1, double x2, double y2) {
	return sqrt(pow((x2 - x1), 2) + pow((y2 - y1), 2)); // returns length of radius between two points in 2d space
}
// isValidMove function
bool Actor::isValidMove(int x, int y) {
	// if attempting to move outside of the moveable grid returns false
	if ((x < 0 || x>60) || (y < 0 || y>60)) return false;
	// otherwise return true
	return true;
}

// Boulder Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Boulder::doSomething() {
	// check if Boulder is alive
	if (!isAlive()) return; // returns if Boulder is dead

	StudentWorld* world = getWorld();
	// Next check if Boulder is in stable state
	// If so checks for Earth in the four squares directly below its current position
	if (isStable) {
		if (world->checkStability(getX(), getY())) return; // if currently stable will return and do nothing
		else {
			// otherwise switches the state to waiting
			isStable = false;
			isWaiting = true;
		}
	}

	// Check if boulder is in waiting state
	else if (isWaiting) {
		// Check if 30 ticks have elapsed
		if (waitingTicks <= 1) { // If so boulder enters falling state
			isWaiting = false;
			isFalling = true;
			world->playSound(SOUND_FALLING_ROCK); // play falling sound
		}
		else waitingTicks--;; // Otherwise decrements waitingTicks
	}

	// Otherwise boulder is in falling state
	else {
		moveTo(getX(), getY() - 1); // Move boulder down by one
		// Check the stopping conditions of a falling boulder
		if (getY() < 0) { // If attempts to move to y=-1 will setDead and return
			setDead();
			return; 
		}

		if (world->checkStability(getX(), getY())) { // If it hits Earth will setDead and return
			setDead();
			return;
		}

		// If statement to check if it hits another boulder
		if (world->boulderOnBoulder(this)) { // If it hits Boulder will setDead and return
			setDead();
			return;
		}

		// Check if it hits a human object
		TunnelMan* tMan = world->getTunnelMan();
		if (getRadius(getX(), getY(), tMan->getX(), tMan->getY()) <= 3) { // For right now only checking if it hits the tunnel man
			tMan->getAnnoyed(10); // kills the tunnel man
		}
		world->annoyProtestersHelper(10, getX(), getY());
	}
}

// Barrel Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Barrel::doSomething() {
	// check if Barrel is alive
	if (!isAlive()) return; // returns if Barrel is dead

	StudentWorld* world = getWorld();
	// Check if not visible and if Tunnel Man is within radius 4.0
	// If so become visible and return
	if (!isVisible() && getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 4.00) {
		setVisible(true);
		return;
	}

	// Check if Tunnel Man is within radius 3.0
	// If so Barrel becomes dead, plays sound effect, increase player score 1000 points
	if (getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 3.00) {
		setDead();
		world->playSound(SOUND_FOUND_OIL);
		world->increaseScore(1000);
		world->foundOil(); // informs world that an oil object has been picked up
	}
}

// Gold Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Gold::doSomething() {
	// check if Gold is alive
	if (!isAlive()) return; // returns if Gold is dead

	StudentWorld* world = getWorld();
	// Check if not visible and if Tunnel Man is within radius 4.0
	// If so become visible and return
	if (!isVisible() && getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 4.00) {
		setVisible(true);
		return;
	}
	
	// Check if in permanent state and if Tunnel Man is within radius 3.0
	// If so can be picked up by player
	if (permanentState && getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 3.00) {
		setDead();
		world->playSound(SOUND_GOT_GOODIE);
		world->increaseScore(10);
		world->getTunnelMan()->foundGold();
	}

	// Otherwise is in temporary state
	else if (!permanentState) {
		// Check if 100 tick lifetime has expired
		// If so will delete
		if (m_Ticks < 1) {
			setDead();
			return;
		}
		// check if within radius 3 of a protester
		// if so one will pick up the gold
		Actor* temp = world->briberyHelper(getX(), getY());
		if (temp != nullptr) {
			setDead(); // sets itself to dead
			Protester* pMan = dynamic_cast<Protester*>(temp);
			pMan->foundGold(); // selected protester will run the foundGold function which varies depending on type of protester
		}

		// if not in range of protester will decrement m_Ticks
		else {
			m_Ticks--;
			return;
		}
	}
}

// WaterPool Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void WaterPool::doSomething() {
	// check if WaterPool is alive
	if (!isAlive()) return; // returns if WaterPool is dead

	StudentWorld* world = getWorld();
	// Check if TunnelMan is within radius 3 of WaterPool
	// If so will become dead, plays sound effect, adds 5 squirts of water to TunnelMan, and increase score by 100
	if (getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 3.00) {
		setDead();
		world->playSound(SOUND_GOT_GOODIE);
		world->getTunnelMan()->foundWater();
		world->increaseScore(100);
		return;
	}

	// Otherwise check if tick lifetime has elapsed
	else if(m_Ticks<=1){ // Becomes dead if m_Ticks is less than or equal to 1
		setDead();
		return;
	}

	// Else decrement m_Ticks
	else {
		m_Ticks--;
		return;
	}

}

// SonarKit Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void SonarKit::doSomething() {
	// check if SonarKit is alive
	if (!isAlive()) return; // returns if SonarKit is dead

	StudentWorld* world = getWorld();
	// Check if TunnelMan is within radius 3 of SonarKit
	// If so will become dead, plays sound effect, add 1 SonarKit to TunnelMan, and increase score by 75
	if (getRadius(getX(), getY(), world->getTunnelMan()->getX(), world->getTunnelMan()->getY()) <= 3.00) {
		setDead();
		world->playSound(SOUND_GOT_GOODIE);
		world->getTunnelMan()->foundSonar();
		world->increaseScore(75);
		return;
	}

	// Otherwise check if tick lifetime has elapsed
	else if (m_Ticks <= 1) { // Becomes dead if m_Ticks is less than or equal to 1
		setDead();
		return;
	}

	// Else decrement m_Ticks
	else {
		m_Ticks--;
		return;
	}
}

// Squirt Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////
void Squirt::doSomething() {
	// check if Squirt is alive
	if (!isAlive()) return; // returns if Squirt is dead

	StudentWorld* world = getWorld();

	// check if it is within a radius of 3 of one or more protesters
	if (world->annoyProtestersHelper(2, getX(), getY())) {
		setDead(); // if so will set state to dead
	}

	// Ensures that Squirt object can't exceede the max travel distance
	if (maxRange > 0) {
		int x = getX();
		int y = getY();
		if (getDirection() == left && isValidMove(x - 1, y)) { // Attempt to move leftward
			
			if (world->isOccupied(x - 1, y)) { // Check if target position is occupied by Earth or Boulder
				setDead();
				return;
			}
			
			moveTo(x - 1, y); // If not occupied will move
		}
		else if (getDirection() == right && isValidMove(x + 1, y)) { // Attempt to move rightward
			if (world->isOccupied(x + 1, y)) { // Check if target position is occupied by Earth or Boulder
				setDead();
				return;
			}
			moveTo(x + 1, y); // If not occupied will move
		}
		else if (getDirection() == down && isValidMove(x, y-1)) { // Attempt to move downward
			if (world->isOccupied(x, y - 1)) { // Check if target position is occupied by Earth or Boulder
				setDead();
				return;
			}
			moveTo(x, y-1); // If not occupied will move
		}
		else if (getDirection() == up && isValidMove(x, y + 1)) { // Attempt to move upward
			if (world->isOccupied(x, y + 1)) { // Check if target position is occupied by Earth or Boulder
				setDead();
				return;
			}
			moveTo(x, y + 1); // If not occupied will move
		}
		maxRange--; // Decrement maxRange
	}

	else {
		setDead();
		return;
	}
}

// TunnelMan Implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// fireSquirt function
void TunnelMan::fireSquirt(int x, int y, Direction dir) {
	StudentWorld* world = getWorld();
	switch (dir) {
	case(left):
		if (isValidMove(x - 4, y) && !world->isOccupied(x-4, y)) {
			world->newActor(new Squirt(world, x - 4, y, dir));
		}
		break;
	case(right):
		if (isValidMove(x + 4, y) && !world->isOccupied(x + 4, y)) {
			world->newActor(new Squirt(world, x + 4, y, dir));
		}
		break;
	case(down):
		if (isValidMove(x, y-4) && !world->isOccupied(x, y - 4)) {
			world->newActor(new Squirt(world, x, y - 4, dir));
		}
		break;
	case(up):
		if (isValidMove(x, y + 4) && !world->isOccupied(x, y + 4)) {
			world->newActor(new Squirt(world, x, y + 4, dir));
		}
		break;
	default:
		break;
	}
}

// doSomething function
void TunnelMan::doSomething() {
	// check if Tunnel Man is Alive
	if (!isAlive()) return; // returns if TunnelMan is dead

	// check TunnelMan overlaps any Earth objects
	// will destroy any Earth objects from x,y to x+3, y+3
	StudentWorld* world = getWorld();
	int x = getX();
	int y = getY();

	// will take inputs user
	int ch;
	if (world->getKey(ch)) {
		switch (ch) {
			case KEY_PRESS_ESCAPE:
				this->setDead(); // If user presses escape key sets player as dead and decrease lives
				break;
			case KEY_PRESS_LEFT: // if facing left and can move left will move left
				if ((getDirection() == left && isValidMove(x - 1, y)) && !world->withinBoulderRadius(x-1, y)) { // Also check that it won't move to a boulder
					moveTo(x - 1, y);
				}
				else setDirection(left); // otherwise sets direction to left
				break;
			case KEY_PRESS_RIGHT: // if facing right and can move right will move right
				if ((getDirection() == right && isValidMove(x + 1, y)) && !world->withinBoulderRadius(x + 1, y)) { // Also check that it won't move to a boulder
					moveTo(x + 1, y);
				}
				else setDirection(right); // otherwise sets direction to right
				break;
			case KEY_PRESS_DOWN: // if facing down and can move down will move down
				if ((getDirection() == down && isValidMove(x, y-1)) && !world->withinBoulderRadius(x, y - 1)) { // Also check that it won't move to a boulder
					moveTo(x, y-1);
				}
				else setDirection(down); // otherwise sets direction to down
				break;
			case KEY_PRESS_UP: // if facing up and can move up will move up
				if ((getDirection() == up && isValidMove(x, y+1)) && !world->withinBoulderRadius(x, y + 1)) { // Also check that it won't move to a boulder
					moveTo(x, y+1);
				}
				else setDirection(up); // otherwise sets direction to up
				break;
			case 'Z': // If user presses z or Z and has sonar objects in his inventory will decrement sonar kits and illuminate the objects within a 12 square radius around him
			case 'z':
				if (m_Sonar > 0) {
					m_Sonar--;
					world->illuminate(getX(), getY());
				}
				break;
			case KEY_PRESS_TAB: // If user presses tab and has gold objects in inventory will decrement gold and add a gold object at that coordinates
				if (m_Gold > 0) {
					m_Gold--;
					world->dropGold(getX(), getY());
				}
				break;
			case KEY_PRESS_SPACE: // If user presses space and has water in its inventory will fire a squirt in the direction it's facing
				if (m_Squirts > 0) {
					m_Squirts--;
					world->playSound(SOUND_PLAYER_SQUIRT);
					fireSquirt(getX(), getY(), getDirection());
				}
				break;
			default:
				break;
		}
	}
	
	// During each tick TunnelMan will atempt to dig
	// If he digs an Earth object will play sound
	if (world->dig(getX(), getY())) {
		world->playSound(SOUND_DIG);
	}
	
	return;
}

// getAnnoyed function
void TunnelMan::getAnnoyed(int n) {
	damage(n); // damage by n hits
	if (getHealth() <= 0) { // check if dead
		setDead(); // sets player as dead
		getWorld()->playSound(SOUND_PLAYER_GIVE_UP); // plays give up sound
	}
}

// setDead function
void TunnelMan::setDead() {
	StudentWorld* world = getWorld();
	world->decLives(); // decreases lives by one
	Actor::setDead(); // sets TunnelMan to dead
}

// Protester implementation
///////////////////////////////////////////////////////////////////////////////////////////////////////////

// Protester Constructor
Protester::Protester(StudentWorld* sWorld, int imageID, int hits, bool isHard):Human(sWorld, imageID, 60, 60, left, hits) {
	int level = sWorld->getLevel();
	isHardCore = isHard;
	m_leaveState = false;
	m_restingTick = 0;
	m_ticksToWaitBetweenMoves = max(0, 3 -(level/ 4));
	m_ticksSinceLastShout = 0;
	m_ticksSinceLastPerpMove = 0;
}

// stunned function
// sets m_ticksToWaitBetweenMoves to a longer than normal value
void Protester::stunned() {
	int level = getWorld()->getLevel();
	m_ticksToWaitBetweenMoves = max(50, 100 - level * 10);
}

// getAnnoyed function
// decrements Protester's health by corresponding amount
// if Protester's health is less than or equal to zero will enter a leave the oil field state
void Protester::getAnnoyed(int n) {
	if (m_leaveState) {
		return; // can't be further annoyed
	}
	damage(n); // decrement health by n
	
	// check if fully annoyed
	if (getHealth() <= 0) {
		if (n == 10) {
			getWorld()->increaseScore(500); // if annoyed by boulder will increase score by 500
		}
		else if(isHardCore){
			getWorld()->increaseScore(250); // if HardCore Protester is fully annoyed by a squirt will increase score by 250
		}
		else {
			getWorld()->increaseScore(100); // if regular Protester is fully annoyed by a squirt will increase score by 100
		}
		m_leaveState = true; // if so enters leave the oil field state
		getWorld()->playSound(SOUND_PROTESTER_GIVE_UP);
		m_restingTick = 0;
	}

	// if not fully annoyed will play annoyed sound and will stun the protester
	else {
		getWorld()->playSound(SOUND_PROTESTER_ANNOYED);
		stunned();
		return;
	}
}

// doSomething function
void Protester::doSomething() {
	// Check if Protester is alive
	if (!isAlive()) return; // Return if Protester is dead

	StudentWorld* world = getWorld();
	int level = world->getLevel();
	TunnelMan* tMan = world->getTunnelMan();

	// Check if Protester is in rest state
	if (m_ticksToWaitBetweenMoves > 0) {
		m_ticksToWaitBetweenMoves--; // if so decrements the waiting ticks
		return;
	}

	// If not increment tick counts
	else {
		m_ticksToWaitBetweenMoves = max(0, 3 - (level / 4));
		if (!m_leaveState) {
			m_ticksSinceLastPerpMove++;
			m_ticksSinceLastShout++;
		}
	}

	// Check if protester is in a leave the oil field state
	 if(m_leaveState) {
		// Check if at x=60 y=60
		 if (getX() == 60 && getY() == 60) { // if so sets dead and returns
			 setDead();
			 return;
		}
		 // if not will move there using the most optimal route from its position
		 else {
			 Direction dir = world->getRouteToExit(getX(), getY());
			 setDirection(dir);
			 move(dir);
			 return;
		 }
	 }
	 // If not in leave the oil field state check if within radius of 4 of Tunnel Man and facing Tunnel Man
	 // If so check if its been at least 15 non resting ticks since protester last shouted
	 int radius = tMan->getRadius(tMan->getX(), tMan->getY(), getX(), getY());
	 if ((radius <= 4 && facingTunnelMan()) && m_ticksSinceLastShout > 15) {
		 world->playSound(SOUND_PROTESTER_YELL);
		 tMan->getAnnoyed(2);
		 m_ticksSinceLastShout = 0;
		 return;
	 }

	 // If TunnelMan is within M legal moves of a HardCore Protester will utilize a maze algorithm to move toward TunnelMan
	 
	 if (isHardCore) {
		 int M = 16 + level * 2;
		 // Check if can reach TunnelMan within M legal moves
		 int moves = world->getMovesToTunnelMan(getX(), getY());
		 if (moves >= 0 && moves <= M) {
			 Direction dir = world->getRouteToMan(getX(), getY()); // If can get to TunnelMan in M moves will change direction to the most optimal route
			 setDirection(dir);
			 move(dir); // Will move according to most optimal route
			 return;
		 }
	 }
	 

	 //  Check if tunnel man is in a straight line of sight from protester
	 if (radius > 4 && scan() != none) { // radius must be greater than 4
		 setDirection(scan()); // set new direction
		 move(getDirection()); // move in that direction
		 m_numSquaresToMove = 0; // set numSquaresToMove to 0
		 return;
	 }

	 // Otherwise protester can't directly see the TunnelMan
	 // Will pick a new random direction
	 m_numSquaresToMove--;
	 if (m_numSquaresToMove <= 0) {
		 setDirection(randomDirection(getX(), getY())); // finds random direction where it can at least take one step
		 m_numSquaresToMove = 8 + rand() % 53; // Can move between 8 to 60 steps in its current direction
	 }

	 // Check if it's time to make a perpendicular turn if Protester can
	 if (m_ticksSinceLastPerpMove > 200 && randomTurn(getX(), getY()) != none) {
		 setDirection(randomTurn(getX(), getY()));
		 m_numSquaresToMove = 8 + rand() % 53; // Can move between 8 to 60 steps in its current direction
		 m_ticksSinceLastPerpMove = 0;
		 return;
	 }


	 // Protester will move in current direction
	 if (move(getDirection()) == false) { // if its path becomes blocked will set m_numSquaresToMove to 0 so it can pick a new direction on the next non-resting tick
		 m_numSquaresToMove = 0;
	 }
	 return;
}

// facingTunnelMan function
bool Protester::facingTunnelMan() {
	StudentWorld* world = getWorld();
	TunnelMan* tMan = world->getTunnelMan();
	GraphObject::Direction dir = getDirection();

	return (dir == left && tMan->getX() <= getX()) || // if tunnel man is to the left of protester, protester must be facing left to face the tunnel man
		(dir == right && tMan->getX() >= getX()) || // if tunnel man is to the right of protester, protester must be facing right to face the tunnel man
		(dir == down && tMan->getY() <= getY()) ||  // if tunnel man is below the protester, protester must be facing down to face the tunnel man
		(dir == up && tMan->getY() >= getY()); // if tunnel man is above the protester, protester must be facing up to face the tunnel man
}

// scan function
GraphObject::Direction Protester::scan() {
	StudentWorld* world = getWorld();
	TunnelMan* tMan = world->getTunnelMan();

	// Check if in horizontal line of sight
	if (getY() == tMan->getY()) {
		bool canReachTarget = true;
		// Check path right
		if (tMan->getX() > getX()) {
			int steps = 0;
			while (canReachTarget) {
				if (getX() + steps == tMan->getX()) { // if can reach tunnel man's position will return right
					return GraphObject::right;
				}
				if (isValidMove(getX() + steps, getY()) && !world->isOccupied(getX() + steps, getY())) { // otherwise will check next position right
					steps++;
				}
				else { // if next position right is an invalid coordinate or occupied will stop the loop
					canReachTarget = false;
				}
			}
		}
		// Check path left
		else {
			int steps = 0;
			while (canReachTarget) {
				if (getX() - steps == tMan->getX()) { // if can reach tunnel man's position will return left
					return GraphObject::left;
				}
				if (isValidMove(getX() - steps, getY()) && !world->isOccupied(getX() - steps, getY())) { // otherwise will check next position left
					steps++;
				}
				else { // if next position left is an invalid coordinate or occupied will stop the loop
					canReachTarget = false;
				}
			}
		}
	}

	// Check if in vertical line of sight
	else if (getX() == tMan->getX()) {
		bool canReachTarget = true;
		// Check path up
		if (tMan->getY() > getY()) {
			int steps = 0;
			while (canReachTarget) {
				if (getY() + steps == tMan->getY()) { // if can reach tunnel man's position will return up
					return GraphObject::up;
				}
				if (isValidMove(getX(), getY() + steps) && !world->isOccupied(getX(), getY() + steps)) { // otherwise will check next position up
					steps++;
				}
				else { // if next position up is an invalid coordinate or occupied will stop the loop
					canReachTarget == false;
					break;
				}
			}
		}
		// Check path down
		else {
			int steps = 0;
			while (canReachTarget) {
 				if (getY() - steps == tMan->getY()) { // if can reach tunnel man's position will return down
					return GraphObject::down;
				}
				if (isValidMove(getX(), getY() - steps) && !world->isOccupied(getX(), getY() - steps)) { // otherwise will check next position down
					steps++;
				}
				else { // if next position down is an invalid coordinate or occupied will stop the loop
					canReachTarget == false;
					break;
				}
			}
		}
	}
	// if reaches here not in line of sight
	return GraphObject::none;
}

// move function
// will attempt to move the protester if the direction is valid
bool Protester::move(GraphObject::Direction dir) {
	if (dir == none) return false; // must be valid direction

	StudentWorld* world = getWorld();

	switch (dir) {
	case up:
		if (isValidMove(getX(), getY() + 1) && !world->isOccupied(getX(), getY() + 1)) {
			moveTo(getX(), getY() + 1);
			return true;
		}
		break;
	case down:
		if (isValidMove(getX(), getY() - 1) && !world->isOccupied(getX(), getY() - 1)) {
			moveTo(getX(), getY() - 1);
			return true;
		}
		break;
	case right:
		if (isValidMove(getX() + 1, getY()) && !world->isOccupied(getX() + 1, getY())) {
			moveTo(getX() + 1, getY());
			return true;
		}
		break;
	case left:
		if (isValidMove(getX() - 1, getY()) && !world->isOccupied(getX() - 1, getY())) {
			moveTo(getX() - 1, getY());
			return true;
		}
		break;
	default:
		break;
	}
	return false; // returns false if path becomes blocked
}

// randomDirection function
// finds a randomDirection for the Protester to turn when it can't directly see the TunnelMan
GraphObject::Direction Protester::randomDirection(int x, int y) {
	bool loop = true;
	StudentWorld* world = getWorld();
	while (loop) { // Will loop until a valid direction where the protester can take at least one step is found
		int rando = 1 + rand() % 4;
		switch (rando) {
		case 1: // up
			if (isValidMove(getX(), getY() + 1) && !world->isOccupied(getX(), getY() + 1)) {
				return up;
			}
			break;
		case 2: // down
			if (isValidMove(getX(), getY() - 1) && !world->isOccupied(getX(), getY() - 1)) {
				return down;
			}
			break;
		case 3: // right
			if (isValidMove(getX() + 1, getY()) && !world->isOccupied(getX() + 1, getY())) {
				return right;
			}
			break;
		case 4: // left
			if (isValidMove(getX() - 1, getY()) && !world->isOccupied(getX() - 1, getY())) {
				return left;
			}
			break;
		default:
			break;
		}
	}
}

// randomTurn function
// will return a valid direction to do a perpendicualr turn
// If turn is not possible will return none
GraphObject::Direction Protester::randomTurn(int x, int y) {
	StudentWorld* world = getWorld();
	// if currently facing left or right will attempt to turn up or down
	if (getDirection() == left || getDirection() == right) {
		// First check if up and down are both valid moves
		if ((isValidMove(getX(), getY() + 1) && !world->isOccupied(getX(), getY() + 1)) && (isValidMove(getX(), getY() - 1) && !world->isOccupied(getX(), getY() - 1))) { // if so randomly pick one
			if (rand() % 2 == 0) {
				return up;
			}
			else return down;
		}
		// Check if up is viable move
		else if (isValidMove(getX(), getY() + 1) && !world->isOccupied(getX(), getY() + 1)) {
			return up;
		}
		// Check if down is viable move
		else if (isValidMove(getX(), getY() - 1) && !world->isOccupied(getX(), getY() - 1)) {
			return down;
		}
		// If no viable moves returns none
		else {
			return none;
		}
	}

	// if currently facing up or down will attempt to move left or right
	else {
		// First check if left and right are both viable moves
		if ((isValidMove(getX() + 1, getY()) && !world->isOccupied(getX() + 1, getY())) && (isValidMove(getX() - 1, getY()) && !world->isOccupied(getX() - 1, getY()))) { // if so randomly pick one
			if (rand() % 2 == 0) {
				return right;
			}
			else return left;
		}
		// Check if right is viable move
		else if (isValidMove(getX() + 1, getY()) && !world->isOccupied(getX() + 1, getY())) {
			return right;
		}
		// Check if left is viable move
		else if (isValidMove(getX() - 1, getY()) && !world->isOccupied(getX() - 1, getY())) {
			return left;
		}
		// If no viable move returns none
		else {
			return none;
		}
	}
}

// foundGold function for RegularProtester 
void Protester::foundGold() {
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD); // play sound effect
	getWorld()->increaseScore(25); // increase player's score by 25
	m_leaveState = true;
}

// foundGold function for hardCoreProtester
void HardCoreProtester::foundGold() {
	getWorld()->playSound(SOUND_PROTESTER_FOUND_GOLD); // play sound effect
	getWorld()->increaseScore(50); // increase player's score by 50
	stunned(); // Will stun the HardCore Protester
}