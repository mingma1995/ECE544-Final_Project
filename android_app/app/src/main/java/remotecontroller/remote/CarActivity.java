/**
 * Ram Bhattarai
 * Ming Ma
 * Zhe Lu
 * ECE 544
 * Android App activity that handles the control signal of the car
 * Sends a control signal to firebase which will be used later to control the car
 */

package remotecontroller.remote;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.GestureDetector;
import android.view.MotionEvent;
import android.view.View;
import android.widget.Button;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

/*
    Class that handles the control signal
 */
public class CarActivity extends AppCompatActivity {

    //Available Buttons for control
    Button left;
    Button right;
    Button up;
    Button down;


    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_car);
        left = findViewById(R.id.left);
        right = findViewById(R.id.right);
        up    = findViewById(R.id.up);
        down  = findViewById(R.id.down);


        // Create an instance of the firebase for sending signal from firebase
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        final DatabaseReference myRef = database.getReference("MOTOR_SELECT");

        //Button Ontouch listener to update the control signal for forward motion of the car
        up.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                myRef.setValue(1);
                return false;
            }
        });

        //Button Ontouch Listener to update the control signal for the Reverse motion of the car
        down.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                myRef.setValue(2);
                return false;
            }
        });

        //Button Ontouch Listener to update the control signal for the Turn Left motion of the car
        left.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                myRef.setValue(3);
                return false;
            }
        });

        //Button Ontouch Listener to update the control signal for the Turn Right motion of the car
        right.setOnTouchListener(new View.OnTouchListener() {
            @Override
            public boolean onTouch(View view, MotionEvent motionEvent) {
                myRef.setValue(4);
                return false;
            }
        });

    }
}
