import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;

public abstract class Robot {
	public PBox2D box2d;
	public Body body;
	protected float w, h;

	public Vec2 target;

	//robot parts
	protected Motor leftMotor, rightMotor;
	protected Motor motors[];
	protected VPS vps;

	public Robot(PBox2D b2d, float x, float y) {
		box2d = b2d;

		w = 16;
		h = 16;

		//make the robot's body and add to Box2D world
		makeBody(new Vec2(x, y), w, h);
	}

	//remove from world
	void killBody() {
		box2d.destroyBody(body);
	}

	//check if particle is ready to be removed
	boolean done() {
		//we're a robot so we don't magically disappear
		return false;
	}

	//where the control logic goes in the child classes
	public abstract void think();

	public void update() {
		leftMotor.affect(body);
		rightMotor.affect(body);	

		//apply friction force
		Vec2 vel = body.getLinearVelocity();
		//get horizontal component
		float moveAngle = (float)Math.atan2(vel.y, vel.x);
		float faceAngle = body.getAngle();
		float newspeed = vel.length()*(float)Math.cos(moveAngle-faceAngle);
		body.setLinearVelocity(new Vec2((float)(newspeed*Math.cos(faceAngle)), (float)(newspeed*Math.sin(faceAngle))));
	}

	private void makeBody(Vec2 center, float _w, float _h) {
		// Define a polygon (this is what we use for a rectangle)
		PolygonShape sd = new PolygonShape();
		float box2dW = box2d.scalarPixelsToWorld(_w/2);
		float box2dH = box2d.scalarPixelsToWorld(_h/2);
		sd.setAsBox(box2dW, box2dH);

		// Define a fixture
		FixtureDef fd = new FixtureDef();
		fd.shape = sd;
		// Parameters that affect physics
		fd.density = 1;
		fd.friction = 0.3f;
		fd.restitution = 0.5f;

		// Define the body and make it from the shape
		BodyDef bd = new BodyDef();
		bd.type = BodyType.DYNAMIC;
		bd.position.set(box2d.coordPixelsToWorld(center));

		body = box2d.createBody(bd);
		body.createFixture(fd);
		body.setLinearDamping(0.1f);
		body.setAngularDamping(0.8f);
	}

	//**** API ****
	//motor.c
	public void motor_init() { /*does nothing*/ }
	public void motor_set_vel(int motor, int vel) {
		motors[motor%motors.length].setSpeed(vel);
	}
	public void motor_brake(int motor) {
		motors[motor%motors.length].setSpeed(0);
	}
	public int motor_get_current(int motor) {
		//TODO simulate actual motor current
		return (int)motors[motor%motors.length].getSpeed();
	}

	public int motor_get_current_MA(int motor) {
		//TODO do an actual conversion
		return motor_get_current(motor);
	}
	
	//gyro.c
	/*public void gyro_init() {}
	public void gyro_update() {}
	public int gyro_get_degrees() {}
	public void gyro_set_degrees() {}

	//lcd.c
	public void lcd_init();
	public void lcd_write(boolean is_data, char val) {}
	public void lcd_print(String msg) {}
	*/
	//**** HELPERS ****

	public float getWidth() { return w; }
	public float getHeight() { return h; }

	//measures distance
	class SharpSensor {
		public SharpSensor() {

		}
	}

	class Gyro {
		private float direction;

		public Gyro() {
			direction = 0;
		}

		public float getAngle() {
			return body.getAngle();
		}
	}

	//really models a motor connected to a wheel
	//directly impacts movement
	class Motor {
		private static final float scale = 0.01f;
		private static final float torque = 1;		//TODO affect strength of force
		private static final float maxspeed = 255;

		private float x, y;
		private float speed;

		public Motor(float _x, float _y) {
			x = _x;
			y = _y;

			speed = 0;
		}

		public void affect(Body b) {
			if(speed>0.001) {
				Vec2 force = b.getWorldVector(new Vec2(speed*scale, 0));
				Vec2 point = b.getWorldPoint(new Vec2(x, y));
				b.applyForce(force, point);
			} else {
				//TODO brake
			}
		}

		public void setSpeed(float val) {
			if(Math.abs(val)<=maxspeed)
				speed = val;
		}

		public float getSpeed() { return speed; }
		public float getTorque() { return torque; }
		public float getMaxSpeed() { return maxspeed; }
	}

	//gives the same information the VPS would
	class VPS {
		public Vec2 getPosition() {
			return body.getWorldCenter();
		}

		public float getBearing() {
			return bound(body.getAngle(), (float)(2.0*Math.PI));
		}

		public boolean isRoundStarted() {
			//TODO add round logic
			//for now round is always going
			return true;
		}

		public boolean isRoundEnded() {
			//TODO add round logic
			//for now round is always going
			return false;
		}

		private float bound(float val, float max) {
			if(val<0) {
				while(val<0)
					val += max;
			} else if(val>max) {
				while(val>max)
					val -= max;
			}

			return val;
		}
	}
}
