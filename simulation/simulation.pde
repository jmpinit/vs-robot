import java.util.*;

Polygon level;

void setup() {
	size(640, 480);
	stroke(255);

	level = new Polygon();

	for(float i=0; i<256; i++) {
		level.addPoint(100.0*cos((PI/256.0)*i), 100.0*sin((PI/256.0)*i));
	}
}

void draw() {
	//update

	//draw
	background(0);

	Ray lastp = level.getPoints().get(0);
	for(Ray p: level.getPoints()) {
		line(p.x, p.y, lastp.x, lastp.y);
		lastp = p;
	}
}

class Ray {
	public float x, y;

	public Ray() { x = 0; y = 0; }

	public Ray(float _x, float _y) {
		x = _x;
		y = _y;
	}
}

class Polygon {
	protected Vector<Ray> points;

	public Polygon() {
		points = new Vector<Ray>();
	}

	public void addPoint(float x, float y) {
		points.add(new Ray(x, y));
	}

	public Vector<Ray> getPoints() { return points; }
}
