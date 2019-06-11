/*
    Ram Bhattarai
    Ming Ma
    Zhe Lu
    Class that handles the splash screen
    After completing the animation, opens up a option activity
 */
package remotecontroller.remote;

import android.animation.Animator;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.ViewPropertyAnimator;
import android.view.animation.AccelerateDecelerateInterpolator;
import android.widget.ImageView;

public class SplashActivity extends AppCompatActivity implements Animator.AnimatorListener {

    private ImageView img;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_splash);

        // set up what image it will use
        img = findViewById(R.id.img);
        ViewPropertyAnimator animate = img.animate();
        // set up the parameter that how long the image showing and how large it will scale.
        animate.scaleX(1.5f);
        animate.scaleY(1.5f);
        animate.setDuration(3500L);
        animate.setInterpolator(new AccelerateDecelerateInterpolator());
        animate.setListener(this);
        animate.start();
    }

    @Override
    public void onAnimationStart(Animator animator) {

    }

    @Override
    public void onAnimationEnd(Animator animator) {
        Intent intent = new Intent(SplashActivity.this,OptionActivity.class);
        startActivity(intent);

    }

    @Override
    public void onAnimationCancel(Animator animator) {

    }

    @Override
    public void onAnimationRepeat(Animator animator) {

    }
}
