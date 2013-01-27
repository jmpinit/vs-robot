import pbox2d.*;
import org.jbox2d.collision.shapes.*;
import org.jbox2d.common.*;
import org.jbox2d.dynamics.*;

//robot designed to move between points using a PID algorithm for course correction
public class PIDbot extends Robot {
	private float pre_error;
	
	public PIDbot(PBox2D box2d, float _x, float _y) {
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

		pre_error = 0;
	}

	public void think() {
		//try to move to the top of the arena
		Vec2 loc = vps.getPosition();

		float angleTarget = (float)((Math.atan2(target.y-loc.y, target.x-loc.x)+Math.PI)%(2.0*Math.PI)); //calc angle to target
		if(angleTarget<0) angleTarget += (float)(2.0*Math.PI);
		float angleFacing = (float)((vps.getBearing()+Math.PI)%(2.0*Math.PI));

		final float epsilon	= 0.01f;
		final float dt		= 0.01f;
		final float MAX		= 128.0f;
		final float MIN		= 0.0f;
		final float Kp		= 3.0f;
		final float Kd		= 0.5f;
		final float Ki		= 0.1f;

		//Caculate P,I,D
		float error = (float)(angleTarget - angleFacing);
		//System.out.println(angleTarget+" - "+angleFacing+" = "+error);

		float integral = 0;
		if(Math.abs(error) > epsilon)
			integral = integral + error*dt;
		float derivative = (error - pre_error)/dt;
		float output = Kp*error + Ki*integral + Kd*derivative;

		pre_error = error;

		float motor_bias = min((float)Math.sqrt(Math.pow(target.y-loc.y, 2)+Math.pow(target.x-loc.x, 2)), 64.0f);

		motor_set_vel(0, (int)(64+output));
		motor_set_vel(1, (int)(64-output));
	}

	private float min(float a, float b) {
		if(a>b) return b;
		return a;
	}
}
