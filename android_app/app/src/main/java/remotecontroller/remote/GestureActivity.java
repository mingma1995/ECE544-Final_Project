/*
    Ram Bhattarai
    Ming Ma
    Zhe Lu
    ECE 544
    The class that handles the all the gestures
    Updates the Gestures retrieved from firebase

 */

package remotecontroller.remote;
import android.support.annotation.NonNull;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.widget.EditText;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

public class GestureActivity extends AppCompatActivity {

    EditText gesture;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_gesture);
        gesture = findViewById(R.id.gesture);

        // Write a message to the database
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        final DatabaseReference myRef = database.getReference("MOTOR_SELECT");
        myRef.addValueEventListener(new ValueEventListener() {
            @Override
            //Based on what values received from firebase it, updates the textview
            public void onDataChange(@NonNull DataSnapshot dataSnapshot) {
                int value = dataSnapshot.getValue(Integer.class);
                Log.d("TEST","TEST"+value);

                //Got a Signal of Fist so displays CAR FWD on Textview
                if(value==1)
                {
                    gesture.setText("Fist: CAR FWD: 1");

                }

                //Got a Signal of Fist so displays CAR REVERSE on Textview
                else if(value == 2)
                {
                    gesture.setText("Palm: CAR REVERSING: 2");

                }

                //Got a Signal of Swing so displays CAR Turning Left on Textview
                else if(value == 3)
                {
                    gesture.setText("SWING: CAR TURNING LEFT: 3");
                }

                //Got a Signal of Peace so displays CAR Turning right on Textview
                else if(value == 4)
                {
                    gesture.setText("PEACE: CAR TURNING RIGHT: 4");

                }

                //No signal received so displays no Gestures and car is stopped
                else
                {
                    gesture.setText("NO GESTURES: CAR STOPPED: 0");

                }
            }

            @Override
            public void onCancelled(@NonNull DatabaseError databaseError) {

            }
        });

    }
}
