package com.example.bloomie;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;

import com.google.firebase.auth.FirebaseAuth;
import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import androidx.core.view.WindowInsetsCompat;



public class MainActivity extends AppCompatActivity {
    FirebaseAuth mAuth;
    private TextView id_temperatura;

    private TextView id_humedad;
    private TextView id_suelo;
    private DatabaseReference dht11;
    private DatabaseReference ground_humidity;

    private String userID;


    Button btn_logout;
    private DatabaseReference myRef;


    private static final String TAG = "MainActivity"; // Define tu etiqueta de registro
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

        // Después de inflar el layout, puedes obtener la referencia al botón de logout
        btn_logout = findViewById(R.id.btn_logout);

        mAuth = FirebaseAuth.getInstance();
        // Write a message to the database
        id_temperatura = findViewById(R.id.id_temperatura);
        id_humedad = findViewById(R.id.id_humedad);
        id_suelo = findViewById(R.id.id_suelo);
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        dht11 = FirebaseDatabase.getInstance().getReference().child("DHT11");
        ground_humidity = FirebaseDatabase.getInstance().getReference().child("GROUND_HUMIDITY");

        btn_logout.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                // Aquí puedes agregar el código para cerrar sesión
                mAuth.signOut();
                finish();
                startActivity(new Intent( MainActivity.this, loginActivity.class));
            }
        });

        // Lee desde la base de datos
        dht11.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Este método se llama una vez con el valor inicial y nuevamente

                Float  value = dataSnapshot.child("temperature").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + value);
                id_temperatura.setText(value+"");

                Float  valueh = dataSnapshot.child("humidity").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + valueh);
                id_humedad.setText(valueh+"");

            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Error de lectura de datos
                Log.w(TAG, "Failed to read value.", error.toException());
            }
        });


        ground_humidity.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(@NonNull DataSnapshot snapshot) {
                //Float  values = dataSnapshot.child("GROUND_HUMIDITY").child("value").getValue(Float.class);
                // Log.d(TAG, "Value is: " + values);
                //id_suelo.setText(values+"");
            }

            @Override
            public void onCancelled(@NonNull DatabaseError error) {
                // Error de lectura de datos
                Log.w(TAG, "Failed to read value.", error.toException());

            }
        });



    }

    private void obtnerId(){

    }
}