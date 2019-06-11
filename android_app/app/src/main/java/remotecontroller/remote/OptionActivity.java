/*
    Ram Bhattarai
    Ming Ma
    Zhe Lu
    Provides two options, Control car via phone and control via gestures
    Control via gestures triggers the prediction of gesture on the Laptop
 */


package remotecontroller.remote;

import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;

import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;

public class OptionActivity extends AppCompatActivity {

    Button phonecontrol;
    Button gesturecontrol;
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_option);
        phonecontrol = findViewById(R.id.phone);
        gesturecontrol = findViewById(R.id.gesture);

        //Open a new activity called car activity which handles the car motions
        phonecontrol.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                Intent intent = new Intent(OptionActivity.this,CarActivity.class);
                startActivity(intent);
            }
        });

        //Send a signal on firebase that it is time to process gesture and also open up a gesture activity
        gesturecontrol.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {
                // Write a message to the database
                FirebaseDatabase database = FirebaseDatabase.getInstance();
                final DatabaseReference myRef = database.getReference("GESTURE").child("GESTURE");
                myRef.setValue(1);
                Intent intent = new Intent(OptionActivity.this,GestureActivity.class);
                startActivity(intent);
            }
        });
    }
}
