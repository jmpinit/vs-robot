import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;

public class BasicRobot extends Robot {
	public BasicRobot(PBox2D box2d, float _x, float _y) {
		super(box2d, _x, _y);

		target = new Vec2(0, 0);

		//**** CONSTRUCT THE ROBOT ****

		//motors
		motors = new Motor[2];
		leftMotor = new Motor(0, -h/2);
		rightMotor = new Motor(0, h/2);

		motors[0] = leftMotor;
		motors[1] = rightMotor;

		//sensors
		vps = new VPS();
	}

	public void think() {
		//try to move to the top of the arena
		Vec2 loc = vps.getPosition();

		float angleTarget = (float)Math.atan2(target.y-loc.y, target.x-loc.x); //calc angle to target
		float angleFacing = vps.getBearing();
		float error = (float)Math.abs((angleTarget-angleFacing+Math.PI)%(2.0f*Math.PI));

		motor_set_vel(1, (int)(1023-error/(2.0*Math.PI)*1023));
		motor_set_vel(0, (int)(error/(2.0*Math.PI)*1023));
	}
}
