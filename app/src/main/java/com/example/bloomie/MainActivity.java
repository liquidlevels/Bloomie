package com.example.bloomie;

import android.os.Bundle;
import android.util.Log;
import android.widget.TextView;
import android.view.WindowManager;

import androidx.annotation.NonNull;
import androidx.appcompat.app.AppCompatActivity;
import androidx.core.graphics.Insets;
import androidx.core.view.ViewCompat;

import com.google.firebase.database.DataSnapshot;
import com.google.firebase.database.DatabaseError;
import com.google.firebase.database.DatabaseReference;
import com.google.firebase.database.FirebaseDatabase;
import com.google.firebase.database.ValueEventListener;

import androidx.core.view.WindowInsetsCompat;



public class MainActivity extends AppCompatActivity {

    private TextView id_temperatura;
    private TextView id_humedad;
    private TextView id_suelo;
    private DatabaseReference dht11;
    private DatabaseReference ground_humidity;

    private static final String TAG = "MainActivity"; // Define tu etiqueta de registro
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN );
        setContentView(R.layout.activity_main);

        ViewCompat.setOnApplyWindowInsetsListener(findViewById(R.id.id_suelo), (v, insets) -> {
            Insets systemBars = insets.getInsets(WindowInsetsCompat.Type.systemBars());
            v.setPadding(systemBars.left, systemBars.top, systemBars.right, systemBars.bottom);
            return insets;
        });

        // Write a message to the database
        id_temperatura = findViewById(R.id.id_temperatura);
        id_humedad = findViewById(R.id.id_humedad);
        id_suelo = findViewById(R.id.id_suelo);
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        dht11 = FirebaseDatabase.getInstance().getReference().child("DHT11");
        ground_humidity = FirebaseDatabase.getInstance().getReference().child("GROUND_HUMIDITY");

        // Lee desde la base de datos
        dht11.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Este método se llama una vez con el valor inicial y nuevamente
                // cada vez que los datos en esta ubicación se actualizan.
                Float  value = dataSnapshot.child("temperature").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + value);
                id_temperatura.setText(value+"");

                Float  valueh = dataSnapshot.child("humidity").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + valueh);
                id_humedad.setText(valueh+"");

                //Float  values = dataSnapshot.child("GROUND_HUMIDITY").child("value").getValue(Float.class);
               // Log.d(TAG, "Value is: " + values);
                //id_suelo.setText(values+"");
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
}