import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;

// A reference to our box2d world
PBox2D box2d;

// A list for all of our rectangles
ArrayList<Box> boxes;

Surface surface;

void setup() {
	size(640, 480);
	smooth();

	// Initialize box2d physics and create the world
	box2d = new PBox2D(this);
	box2d.createWorld();
	// We are setting a custom gravity
	box2d.setGravity(0, 0);

	// Create ArrayLists	
	boxes = new ArrayList<Box>();

	surface = new Surface();
}

void draw() {
	background(255);

	// We must always step through time!
	box2d.step();

	// Boxes fall from the top every so often
	if (random(1) < 0.2) {
		Box p = new Box(width/2,30);
		boxes.add(p);
	}

	// Display all the boundaries
	for (Boundary wall: boundaries) {
		wall.display();
	}

	// Display all the boxes
	for (Box b: boxes) {
		b.display();
	}

	// Boxes that leave the screen, we delete them
	// (note they have to be deleted from both the box2d world and our list
	for (int i = boxes.size()-1; i >= 0; i--) {
		Box b = boxes.get(i);
		if (b.done()) {
			boxes.remove(i);
		}
	}
}

class Surface {
	// We'll keep track of all of the surface points
	ArrayList<Vec2> surface;


	Surface() {
		surface = new ArrayList<Vec2>();
		// Here we keep track of the screen coordinates of the chain
		surface.add(new Vec2(width,height/2));
		surface.add(new Vec2(width/2,height/2+50));
		surface.add(new Vec2(0,height/2+50));

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
		strokeWeight(1);
		stroke(0);
		fill(200);
		beginShape();
		for (Vec2 v: surface) {
			vertex(v.x,v.y);
		}
		vertex(0,height);
		vertex(width,height);
		endShape();
	}
}

class Box {
	// We need to keep track of a Body and a width and height
	Body body;
	float w;
	float h;

	// Constructor
	Box(float x, float y) {
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
