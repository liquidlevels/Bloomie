package com.example.bloomie;

import android.content.Intent;
import android.os.Bundle;
import android.os.Handler;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.widget.ImageView;

import androidx.activity.EdgeToEdge;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;
import androidx.core.view.WindowInsetsCompat;

public class splash_activity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        EdgeToEdge.enable(this);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN );
        setContentView(R.layout.activity_splash);

        Animation animacion1= AnimationUtils.loadAnimation(this, R.anim.desplazamiento_arriba);
        Animation animacion2= AnimationUtils.loadAnimation(this, R.anim.desplazamiento_abajo);

        ImageView logo1= findViewById(R.id.logoFlores);
        ImageView logo2= findViewById(R.id.logo);

        logo1.setAnimation(animacion2);
        logo2.setAnimation(animacion1);

        new Handler().postDelayed(new Runnable() {
            @Override
            public void run() {
                Intent intent =new Intent(splash_activity.this, loginActivity.class);
                startActivity(intent);
                finish();
            }
        }, 4000);

    }
}