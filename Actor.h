#ifndef ACTOR_H_
#define ACTOR_H_
#include "GraphObject.h"
class StudentWorld;

// Actor class
// Will serve as the base class for all game objects
// Derived from graph object
class Actor : public GraphObject {
	public:
		// Constructor
		Actor(StudentWorld* sWorld,int imageID, int startX, int startY, Direction dir, double size, unsigned int depth) : GraphObject(imageID, startX, startY, dir, size, depth), m_Alive(true), m_World(sWorld) {
			setVisible(true); // calls set visible making the game object visible when its created
		}

		// Virtual Destructor
		virtual ~Actor() {}

		// Virtual Functions
		virtual void doSomething() {}
		virtual void getAnnoyed(int n) { return; }
		virtual void setDead() { m_Alive = false; } // Sets m_Alive to false
		virtual bool isBoulder() { return false; } // Will return false for all non Boulder objects
		virtual bool isProtester() { return false; } // Will return false for all non Protester elements

		// Accessor Functions
		bool isAlive() { return m_Alive; } // returns true if game object is alive
		StudentWorld* getWorld() { return m_World; } // returns a pointer to the game world

		// Helper Functions
		double getRadius(double x1, double y1, double x2, double y2); // Returns the magnitude of the radius between two positions on the grid
		bool isValidMove(int x, int y); // Returns true if move cordinates are inside (0,0) to (60,60)

	private:
		bool m_Alive; // true when object is alive and false when object is annoyed/dead
		StudentWorld *m_World;
};

// Terrain
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Earth class
// Directly derived from Actor
class Earth : public Actor {
	public:
		// Constructor
		Earth(StudentWorld* sWorld, int startX, int startY) : Actor(sWorld, TID_EARTH, startX, startY, right, 0.25, 3) {}

		// Virtual Functions
		
		// Virtual Destructor
		virtual ~Earth() {}
};

// Boulder class
// Directly derived from Actor
class Boulder : public Actor {
	public:
		// Constructor
		Boulder(StudentWorld* sWorld, int startX, int startY) : Actor(sWorld, TID_BOULDER, startX, startY, down, 1, 1), isStable(true), isWaiting(false), isFalling(false), waitingTicks(30) {}

		// Virtual Functions
		// Each boulder object can do something during a tick
		virtual void doSomething();
		virtual bool isBoulder() { return true; } // returns true because it is a boulder!
		
		// Virtual Destructor
		virtual ~Boulder() {}

	private:
		bool isStable; // when created boulders start off in a stable state
		bool isWaiting; // true when boulder enters waiting state
		bool isFalling;  // true when boulder enters falling state
		int waitingTicks;
};

// Squirt class
// Directly derived from Actor
class Squirt : public Actor {
	public:
		// Constructor
		Squirt(StudentWorld* sWorld, int startX, int startY, Direction dir) : Actor(sWorld, TID_WATER_SPURT, startX, startY, dir, 1, 1), maxRange(4) {}

		// Virtual Functions
		// Each squirt object can do something duringf a tick
		virtual void doSomething();

		// Virtual Destructor
		virtual ~Squirt() {}
	private:
		int maxRange;
};

// Treasure
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Treasure class
// Directly derived from Actor
class Treasure : public Actor {
	public:
		// Constructor
		Treasure(StudentWorld* sWorld, int imageID, int startX, int startY) : Actor(sWorld, imageID, startX, startY, right, 1.0, 2) {}

		// Virtual Functions
		// Each Treasure object can do something during a tick
		virtual void doSomething() {}

		// Virtual Destructor
		virtual ~Treasure() {}
};

// Barrel Class
class Barrel : public Treasure {
	public:
		// Constructor
		Barrel(StudentWorld* sWorld, int startX, int startY)  : Treasure(sWorld, TID_BARREL, startX, startY) {
			setVisible(false); // barrel objects start off invisible to user
		}

		// Virtual Functions
		virtual void doSomething();

		// Virtual Destructor
		virtual ~Barrel() {};
};

// Gold Class
class Gold : public Treasure {
public:
	// Constructor
	Gold(StudentWorld* sWorld, int startX, int startY, bool playerDropped) : Treasure(sWorld, TID_GOLD, startX, startY), permanentState(!playerDropped), m_Ticks(100) {
		setVisible(playerDropped); // barrel objects start off either visible or invisible depending on type of creation
	}

	// Virtual Functions
	virtual void doSomething();

	// Virtual Destructor
	virtual ~Gold() {};
private:
	bool permanentState;
	int m_Ticks;
};

// WaterPool Class
class WaterPool : public Treasure {
	public:
	//Constructor
	WaterPool(StudentWorld* sWorld, int startX, int startY, int tick) : Treasure(sWorld, TID_WATER_POOL, startX, startY), m_Ticks(tick) {}
	// Virtual Functions
	virtual void doSomething();

	// Virtual Destructor
	virtual ~WaterPool() {};
private:
	int m_Ticks;
};

// SonarKit Class
class SonarKit : public Treasure {
public:
	//Constructor
	SonarKit(StudentWorld* sWorld, int tick) : Treasure(sWorld, TID_SONAR, 0, 60), m_Ticks(tick) {}
	// Virtual Functions
	virtual void doSomething();

	// Virtual Destructor
	virtual ~SonarKit() {};
private:
	int m_Ticks;
};

// Humans
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Human class
// Directly derived from Actor
// Will serve as the base class for tunnel man, protestor, and hardcore protestor
class Human : public Actor {
	public:
		// Constructor
		Human(StudentWorld* sWorld, int imageID, int startX, int startY, Direction dir, int health) : Actor(sWorld, imageID, startX, startY, dir, 1.0, 0), m_Health(health) {}

		// Virtual Destructor
		virtual ~Human() {}

		// Virtual Functions
		// Each human game object can do something and can be annoyed
		virtual void doSomething(){}
		virtual void getAnnoyed(int n){}

		// Accessor Functions
		int getHealth() { return m_Health; } // Returns the health of the human object

		// Mutator Function
		void damage(int n) { m_Health -= n; } // Remove n hits from human's health
	private:
		int m_Health; // stores number of hits left before human dies
};

// TunnelMan class
// Directly derived from human
class TunnelMan : public Human {
	public:
		// Constructor
		TunnelMan(StudentWorld* sWorld) : Human(sWorld, TID_PLAYER, 30, 60, right, 10), m_Gold(0), m_Squirts(0), m_Sonar(0) {} // set squirts to 10 for testing

		// Virtual Destructor
		virtual ~TunnelMan(){}

		// Virtual Functions
		virtual void doSomething();
		virtual void getAnnoyed(int n);
		virtual void setDead();

		// Accessor Functions
		int getGold() { return m_Gold; }
		int getSquirts() { return m_Squirts; }
		int getSonar() { return m_Sonar; }

		// Mutator Functions
		void foundGold() { m_Gold++; }
		void foundWater() { m_Squirts += 5; }
		void foundSonar() { m_Sonar += 1; }

		// fireSquirt function
		// when called TunnelMan will fire a squirt object onto the oil field
		void fireSquirt(int x, int y, GraphObject::Direction dir);
	private:
		int m_Gold;
		int m_Squirts;
		int m_Sonar;
};

// Protester class
// Directly derived from human
class Protester : public Human {
	public:
		// Constructor
		Protester(StudentWorld* sWorld, int imageID, int hits, bool isHard);

		// Virtual Destructor
		virtual ~Protester() {}

		// Virtual Functions
		virtual bool isProtester() { return true; } // returns true because is a protester
		virtual void doSomething();
		virtual void getAnnoyed(int n);
		virtual void foundGold();
		virtual void stunned();

		bool facingTunnelMan();
		GraphObject::Direction randomDirection(int x, int y);  // finds a randomDirection for the Protester to turn when it can't directly see the TunnelMan
		GraphObject::Direction randomTurn(int x, int y); // Will allow the Protester to make a perpendicular turn every 200 non resting ticks
		GraphObject::Direction scan(); //  if tunnel man is in a direct line of sight with no obstacles in the way will return the direction for protester to move
		bool move(GraphObject::Direction dir); // will return true if it completed the move and false if not

	
	private:
		bool m_leaveState;
		bool isHardCore;
		int m_restingTick;
		int m_ticksToWaitBetweenMoves;
		int m_ticksSinceLastShout;
		int m_ticksSinceLastPerpMove;
		int m_numSquaresToMove;
};

// RegularProtester class
// Directly derived from Protester
class RegularProtester : public Protester {
	public:
	// Constructor
	RegularProtester(StudentWorld* sWorld) : Protester(sWorld, TID_PROTESTER, 5, false) {}

	// Virtual Destructor
	virtual ~RegularProtester() {}
};

// HardCoreProtester class
// Directly derived from Protester
class HardCoreProtester : public Protester {
public:
	// Constructor
	HardCoreProtester(StudentWorld* sWorld) : Protester(sWorld, TID_HARD_CORE_PROTESTER, 20, true) {}

	// Virtual Functions
	virtual void foundGold();

	// Virtual Destructor
	virtual ~HardCoreProtester() {}
};

#endif // ACTOR_H
