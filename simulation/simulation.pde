import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;

PBox2D box2d;

ArrayList<Wall> walls;
ArrayList<Robot> robots;

Pt[] lvl_outer, lvl_inner;

void setup() {
	size(640, 480);
	smooth();

	// Initialize box2d physics and create the world
	box2d = new PBox2D(this);
	box2d.createWorld();
	// We are setting a custom gravity
	box2d.setGravity(0, 0);

	// Create ArrayLists	
	walls = new ArrayList<Wall>();
	robots = new ArrayList<Robot>();

	//load the level
	lvl_outer = new Pt[6];
	lvl_inner = new Pt[6];
	
	lvl_outer[0] = new Pt(125, 0);
	lvl_outer[1] = new Pt(375, 0);
	lvl_outer[5] = new Pt(0, 217);
	lvl_outer[2] = new Pt(500, 217);
	lvl_outer[4] = new Pt(125, 434);
	lvl_outer[3] = new Pt(375, 434);

	lvl_inner[0] = new Pt(250, 162);
	lvl_inner[1] = new Pt(298, 190);
	lvl_inner[2] = new Pt(298, 244);
	lvl_inner[3] = new Pt(250, 272);
	lvl_inner[4] = new Pt(202, 244);
	lvl_inner[5] = new Pt(202, 190);

	for(int i=0; i<lvl_outer.length-1; i++)
		walls.add(new Wall(lvl_outer[i].x, lvl_outer[i].y, lvl_outer[i+1].x, lvl_outer[i+1].y));
	walls.add(new Wall(lvl_outer[lvl_outer.length-1].x, lvl_outer[lvl_outer.length-1].y, lvl_outer[0].x, lvl_outer[0].y));

	for(int i=0; i<lvl_outer.length-1; i++)
		walls.add(new Wall(lvl_inner[i].x, lvl_inner[i].y, lvl_inner[i+1].x, lvl_inner[i+1].y));
	walls.add(new Wall(lvl_inner[lvl_inner.length-1].x, lvl_inner[lvl_inner.length-1].y, lvl_inner[0].x, lvl_inner[0].y));

	//add a robot for testing!
	Robot robby = new Robot(box2d, width/2, height/2);
	robots.add(robby);
}

void draw() {
	background(255);

	//let the robots think
	for(Robot r: robots)
		r.update();

	//increment simulation
	box2d.step();

	//draw the walls
	for(Wall w: walls)
		w.display();

	//display all the robots
	for (Robot r: robots) {
		//get position and angle
		Vec2 pos = r.box2d.getBodyPixelCoord(r.body);
		float a = r.body.getAngle();

		float w = r.getWidth();
		float h = r.getHeight();

		rectMode(CENTER);
		pushMatrix();
		translate(pos.x, pos.y);
		rotate(-a);
		fill(175);
		stroke(0);
		strokeWeight(1);
		rect(0, 0, w, h);
		strokeWeight(3);
		line(0, 0, w/2, 0);
		popMatrix();
	}
}

void keyPressed() {
	if(key=='q') {
		robots.get(0).left(0.5f);
	} else if(key=='e') {
		robots.get(0).right(0.5f);
	} else if(key=='a') {
		robots.get(0).left(-0.5f);
	} else if(key=='d') {
		robots.get(0).right(-0.5f);
	} else {
		robots.get(0).left(0.0f);
		robots.get(0).right(0.0f);
	}
}

class Wall {
	ArrayList<Vec2> surface;

	public Wall(float x1, float y1, float x2, float y2) {
		float angle = atan2(y2-y1, x2-x1);

		surface = new ArrayList<Vec2>();

		surface.add(new Vec2(x1, y1));
		surface.add(new Vec2(x2, y2));
		surface.add(new Vec2(x2+4*cos(angle-PI/2), y2+4*sin(angle-PI/2)));
		surface.add(new Vec2(x1+4*cos(angle-PI/2), y1+4*sin(angle-PI/2)));

		// This is what box2d uses to put the surface in its world
		ChainShape chain = new ChainShape();

		// We can add 3 vertices by making an array of 3 Vec2 objects
		Vec2[] vertices = new Vec2[surface.size()];
		for (int i = 0; i < vertices.length; i++) {
			vertices[i] = box2d.coordPixelsToWorld(surface.get(i));     
		}

		chain.createChain(vertices,vertices.length);

		// The edge chain is now a body!
		BodyDef bd = new BodyDef();
		Body body = box2d.world.createBody(bd);
		// Shortcut, we could define a fixture if we
		// want to specify frictions, restitution, etc.
		body.createFixture(chain,1);
	}

	// A simple function to just draw the edge chain as a series of vertex points
	void display() {
		fill(0);
		noStroke();
		beginShape();
		for (Vec2 v: surface) {
			vertex(v.x,v.y);
		}
		endShape();
	}
}

//for testing purposes
class Box {
	protected Body body;
	protected float w;
	protected float h;

	public Box() {
	}

	public Box(float x, float y) {
		w = random(4, 16);
		h = random(4, 16);
		// Add the box to the box2d world
		makeBody(new Vec2(x, y), w, h);
	}

	// This function removes the particle from the box2d world
	void killBody() {
		box2d.destroyBody(body);
	}

	// Is the particle ready for deletion?
	boolean done() {
		// Let's find the screen position of the particle
		Vec2 pos = box2d.getBodyPixelCoord(body);
		// Is it off the bottom of the screen?
		if (pos.y > height+w*h) {
			killBody();
			return true;
		}
		return false;
	}

	// Drawing the box
	void display() {
		// We look at each body and get its screen position
		Vec2 pos = box2d.getBodyPixelCoord(body);
		// Get its angle of rotation
		float a = body.getAngle();

		rectMode(CENTER);
		pushMatrix();
		translate(pos.x, pos.y);
		rotate(-a);
		fill(175);
		stroke(0);
		rect(0, 0, w, h);
		popMatrix();
	}

	// This function adds the rectangle to the box2d world
	void makeBody(Vec2 center, float w_, float h_) {

		// Define a polygon (this is what we use for a rectangle)
		PolygonShape sd = new PolygonShape();
		float box2dW = box2d.scalarPixelsToWorld(w_/2);
		float box2dH = box2d.scalarPixelsToWorld(h_/2);
		sd.setAsBox(box2dW, box2dH);

		// Define a fixture
		FixtureDef fd = new FixtureDef();
		fd.shape = sd;
		// Parameters that affect physics
		fd.density = 1;
		fd.friction = 0.3;
		fd.restitution = 0.5;

		// Define the body and make it from the shape
		BodyDef bd = new BodyDef();
		bd.type = BodyType.DYNAMIC;
		bd.position.set(box2d.coordPixelsToWorld(center));

		body = box2d.createBody(bd);
		body.createFixture(fd);

		// Give it some initial random velocity
		body.setLinearVelocity(new Vec2(random(-5, 5), random(2, 5)));
		body.setAngularVelocity(random(-5, 5));
	}
}

class Pt {
	public float x, y;
	
	public Pt(float _x, float _y) {
		x = _x;
		y = _y;
	}
}
