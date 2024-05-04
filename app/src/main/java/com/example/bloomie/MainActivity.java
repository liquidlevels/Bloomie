package com.example.bloomie;

import android.content.Intent;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;
import android.view.WindowManager;

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
        FirebaseDatabase database = FirebaseDatabase.getInstance();
        myRef = FirebaseDatabase.getInstance().getReference().child("DHT11");

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
        myRef.addValueEventListener(new ValueEventListener() {
            @Override
            public void onDataChange(DataSnapshot dataSnapshot) {
                // Este método se llama una vez con el valor inicial y nuevamente
                // cada vez que los datos en esta ubicación se actualizan.
                Float value = dataSnapshot.child("humidity").child("value").getValue(Float.class);
                Log.d(TAG, "Value is: " + value);
                id_temperatura.setText(value + "");
            }

            @Override
            public void onCancelled(DatabaseError error) {
                // Error de lectura de datos
                Log.w(TAG, "Failed to read value.", error.toException());
            }
        });
    }
}